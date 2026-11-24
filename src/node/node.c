#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <poll.h>
#include "parser.h"
#include "packet.h"
#include "serializer.h"

#define PORT 50000
#define MAX_PAYLOAD_SIZE 1200



//node struct which holds client and local socket addresses.
typedef struct{
    int sockfd;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    uint32_t sequence_number;
    atomic_bool running;
}Node;


void message_displayer(const Packet *packet){
    size_t message_len = packet->header.payload_length;
    if (message_len > MAX_PAYLOAD_SIZE){
        printf("display error!\n");
        return;
    }

    const uint8_t *message = packet->payload;
    printf("Message arrived!\n");
    fwrite(message, 1, message_len, stdout);
    printf("========================================\n");

}


//core initilization func
int node_init(Node *node){
    char reciever_ip[16];
    int sockfd;

    //socket creation
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("Socket allocation error!");
        return -1;
    }

    //Target IP input
    printf("Please enter the target IP adress you want to communicate: ");
    if (scanf("%15s", reciever_ip) != 1){
        close(sockfd);
        return -1;
    }

    int ch;
    while((ch = getchar()) != '\n' && ch != EOF){
        continue;
    }


    struct sockaddr_in sendaddr;
    struct sockaddr_in recaddr;
    struct sockaddr_in *sendaddr_ptr = &sendaddr;
    struct sockaddr_in *recaddr_ptr = &recaddr;

    memset(sendaddr_ptr, 0, sizeof(sendaddr));
    memset(recaddr_ptr, 0, sizeof(recaddr));
    //sendaddr configuration

    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(PORT);

    struct in_addr * sin_addr_ptr = &sendaddr.sin_addr;

    int pton_t;

    if((pton_t = inet_pton(AF_INET, reciever_ip, sin_addr_ptr)) != 1){
        close(sockfd);
        printf("Invalid IP address!\n");
        return -1;

    }

    recaddr.sin_family = AF_INET;
    recaddr.sin_port = htons(PORT);
    recaddr.sin_addr.s_addr = INADDR_ANY;
    

    int bind_t;

    if((bind_t = bind(sockfd, (struct sockaddr *)recaddr_ptr, sizeof(*recaddr_ptr))) < 0){
        close(sockfd);
        printf("Binding error!\n");
        return -1;
    }

    node->sockfd = sockfd;
    node->local_addr = recaddr;
    node->remote_addr = sendaddr;
    atomic_init(&node->running, 1);

    return 0;


}
//welcomer func
int welcomer(Node *node){
    printf("======================================\n");
    printf("Welcome to SARP communication tool!\n");


    int node_t;
    if((node_t = node_init(node)) < 0){
        printf("Fatal Error!");
        return -1;
    }
    return 0;
}


//sender func
ssize_t node_send(Node *node, const uint8_t *message, size_t message_length){

    if(message_length > MAX_PAYLOAD_SIZE){
        return -1;
    }

    Packet packet = {0};
    Packet *pack_ptr = &packet;

    

    packet.header.version = 1;
    packet.header.type = 1;
    pack_ptr->header.sequence_num = node->sequence_number;
    pack_ptr->header.payload_length = message_length;
    pack_ptr->payload = message;


    size_t buffer_size = 0;
    size_t written_amnt = 0;

    uint8_t *buffer = create_buffer(&packet, &buffer_size);
    if(buffer == NULL){
        return -1;
    }

    written_amnt = serialize_packet(&packet, buffer, buffer_size);

    if(written_amnt == 0){
        free(buffer);
        return -1;
    }

    ssize_t sent_bytes;

    struct sockaddr_in *remote_addr_ptr = &node->remote_addr;

    sent_bytes = sendto(
        node->sockfd,
        buffer,
        written_amnt,
        0,
        (struct sockaddr *)&node->remote_addr,
        sizeof(*remote_addr_ptr)
    );

    if(sent_bytes < 0){
        printf("Message send error!\n");
        free(buffer);
        return -1;
    }
    else{
        printf("message has been send successfully!\n");
        free(buffer);
        node->sequence_number++;

        return sent_bytes;
    }

    
}


//reciever func
ssize_t node_recieve(Node *node){
    if(node->sockfd < 0){
        return -1;
    }

    ssize_t recieved_bytes;
    uint8_t buffer[1208];
    size_t buffer_size = sizeof(buffer);
    struct sockaddr_in clientaddr = {0};

    socklen_t clientaddr_len = sizeof(clientaddr);
    //socklen_t *socklen_ptr = &clientaddr_len;(alternative version)

    recieved_bytes = recvfrom(
        node->sockfd,
        buffer,
        buffer_size,
        0,
        (struct sockaddr *)&clientaddr,
        &clientaddr_len
    );

    if(recieved_bytes < 0){
        printf("could not recieve message!\n");
        return -1;
    }
    else{
        Packet packet = packet_creator();
        Packet *packet_ptr = &packet;

        ssize_t parsed_bytes;

        parsed_bytes = packet_parser(packet_ptr, buffer, (size_t)recieved_bytes);

        if(parsed_bytes < 0){
            printf("Parsing error!\n");
            return -1;
        }
        //packet is configurated now.


        message_displayer(&packet);
        
        return recieved_bytes;       
    }
}


int node_close(Node *node){

    if(node == NULL){
        return -1;
    }
    if(node->sockfd < 0){
        return -1;
    }


    int close_t;
    close_t = close(node->sockfd);

    if(close_t < 0){
        return -1;
    }

    node->sockfd = -1;
    return 0;
}



//thread routine for send message
void *sender_work(void *args){
    Node *node = (struct Node *)args;
    if(node->running != 1){
        return NULL;
    }

    if(node->sockfd < 0){
        printf("Invalid socket value!\n");
        return NULL;
    }

    char message[1200];

    while(1){
        printf("Enter the message: ");

        if(fgets(message, sizeof(message), stdin) == NULL){
            atomic_store(&node->running, false);
            break;
        }

        size_t index_t = strcspn(message, "\n");
        message[index_t] = '\0'; //null terminate

        if(strcmp(message, "exit") == 0){
            atomic_store(&node->running, false);
            break;
        }

        size_t message_len = strlen(message);

        if(message_len == 0){
            continue;
        }
        
        ssize_t send_bytes;
        send_bytes = node_send(node, message, message_len);

        if(send_bytes < 0){
            printf("Message could not be sent!\n");
        }
    }
    return NULL;
}









//thread routine for recieve message
void *reciever_work(void *args){
    Node *node = (struct Node *)args;

    if(!atomic_load(&node->running)){
        return NULL;
    }

    if(node->sockfd < 0){
        printf("Invalid socket value!\n");
        atomic_store(&node->running, false);
        return NULL;
    }

    struct pollfd pfd;
    pfd.fd = node->sockfd;
    pfd.events = POLLIN;
    pfd.revents = 0;

    struct pollfd *poll_ptr = &pfd;

    ssize_t recieved_bytes;

    while(atomic_load(&node->running)){
        int p_result = poll(poll_ptr, 1, 500);

        if(p_result < 0){
            perror("poll");
            atomic_store(&node->running, false);
            break;
        }

        if(p_result == 0){
            continue;
        }

        if(pfd.revents & POLLIN){
            node_recieve(node);
        }
    }
    return NULL;



    

}



int main(void){
    Node node = {0};
    node.sockfd = -1;

    int welcome_t;
    if((welcome_t = welcomer(&node)) < 0){
        return 1;
    }

    if(node.sockfd < 0){
        fprintf(stderr, "Node initialization failed!\n");
        return 1;
    }

    pthread_t sender_thread;
    pthread_t reciever_thread;

    int sender_create_t;
    int reciever_create_t;

    //create threads

    reciever_create_t = pthread_create(&reciever_thread, NULL, reciever_work, &node);
    if(reciever_create_t != 0){
        printf("Error during reciever thread creation\n");
        node_close(&node);
        return 1;
    }


    sender_create_t = pthread_create(&sender_thread, NULL, sender_work, &node);
    if(sender_create_t != 0){
        printf("Error during sender thread creation!\n");
        atomic_store(&node.running, false);

        pthread_join(reciever_thread, NULL);
        node_close(&node);
        return 1;
    }





    //wait until threads finish.

    int sender_join_t;
    int reciever_join_t;

    sender_join_t = pthread_join(sender_thread, NULL);

    reciever_join_t = pthread_join(reciever_thread, NULL);

    if(sender_join_t != 0){
        printf("Sender thread could not joint!");
        return 1;
    }
    if(reciever_create_t != 0){
        printf("Reciever thread could not joint!");
        return 1;
    }
    
    int close_t;

    if((close_t = node_close(&node)) < 0){
        printf("Node could not be closed!\n");
        return 1;
    }

    return 0;
}