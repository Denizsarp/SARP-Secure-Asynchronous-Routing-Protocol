#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "serializer.h"
#include "packet.h"


#define DEST_PORT 50000
#define MAX_PAYLOAD_SIZE 1200





int welcomer(void){
    printf("======================================\n");
    printf("SaRProtocol Message Sender\n");
    printf("======================================\n");
    printf("\n");
    printf("\n");
    printf("All primary systems online\n");
    printf("Enter the destination IP address: ");

    return 1;
}


int main(void){
    struct sockaddr_in sendaddr;

    //socket creation(udp will be used)
    int sockfd;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("socket could not created!\n");
        return 1;
    }

    memset(&sendaddr, 0, sizeof(sendaddr));
    //sendaddr config
    sendaddr.sin_family = AF_INET;
    sendaddr.sin_port = htons(DEST_PORT);
    
    struct in_addr * sin_addr_ptr = &sendaddr.sin_addr;
    

    
    char receiver_ip[16];
    welcomer();
    if(scanf("%15s", receiver_ip) != 1){
        close(sockfd);
        return 1;
    }

    if(inet_pton(AF_INET, receiver_ip, sin_addr_ptr) != 1){
        printf("Invalid IP address!\n");
        close(sockfd);
        return 1;
    }

    uint32_t sequence_number = 0;
    while(1){
        
        //taking string input
        char message[1200];
        printf("===================================\n");
        printf("Enter the message you want to sent(type 'terminate to exit'): ");

        scanf("%999s", message);

        if(strcmp(message, "exit") == 0){
            printf("process terminated!\n");
            break;
        }
        
        
        size_t message_size = strlen(message);
        if(message_size > 1200){
            printf("Message is too long!");
            continue;
        }

        //packet initilization
        Packet packet = {0};
        Packet *pack_ptr = &packet;

        //configuration
        pack_ptr->header.version = 1;
        pack_ptr->header.type = 1;
        pack_ptr->header.sequence_num = sequence_number;
        pack_ptr->header.payload_length = message_size;

        pack_ptr->payload = message;

        sequence_number++;


        size_t buffer_size = 0;
        size_t written_amnt = 0;

        uint8_t *buffer = create_buffer(&packet, &buffer_size);
        written_amnt = serialize_packet(&packet, buffer, buffer_size);
        //buffer serialized with pack now.

        ssize_t send_bytes;

        send_bytes = sendto(
            sockfd,
            buffer,
            written_amnt,
            0,
            (struct sockaddr *)&sendaddr,
            sizeof(sendaddr)
        );

        if(send_bytes < 0){
            printf("Message could not sent!\n");
            free(buffer);
            close(sockfd);
            return 1;
        }
        else{
            printf("Message has been sent successfully!\n");
            free(buffer);
            continue;
        }
        
    }
    close(sockfd);
    

    return 1;
}




