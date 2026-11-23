#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "parser.h"
#include "packet.h"
#include "serializer.h"

#define PORT 50000
#define MAX_PAYLOAD_SIZE 1200




typedef struct Node{
    int sockfd;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    uint32_t sequence_number;
}Node;



void message_displayer(const Packet *packet){
    size_t message_len = packet->header.payload_length;
    if (message_len > 1200){
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

    return 0;


}
//welcomer func
int welcomer(void){
    printf("======================================\n");
    printf("Welcome to SARP communication tool!\n");
    Node node = {0};
    Node *node_ptr = &node;


    int node_t;
    if((node_t = node_init(node_ptr)) < 0){
        printf("Fatal Error!");
        return -1;
    }
    return 1;
}


//sender func
ssize_t node_send(Node *node, const uint8_t *message, size_t message_length){
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
        (struct sockaddr_in *)&node->remote_addr,
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










































int main(void){
    int welcome_t;
    if((welcome_t = welcomer()) < 0){
        return 1;
    }
    
    while(1){
        char message[1200];
        printf("Enter the message you want to send: ");

    }



    return 0;
}