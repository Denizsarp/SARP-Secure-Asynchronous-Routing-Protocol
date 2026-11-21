#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "serializer.h"
#include "packet.h"

#define DEST_PORT 50000





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

int create_socket(void){

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
    sendaddr.sin_addr.s_addr = INADDR_ANY;

    
    char reciever_ip[16];
    welcomer();
    scanf("%s", reciever_ip);

    int sequence_number = 0;
    while(true){
        
        //taking string input
        char message[1000];
        printf("===================================\n");
        printf("Enter the message you want to sent(type 'terminate to exit'): ");

        scanf("%999s", message);

        if(strcmp(message, "exit") == 0){
            printf("process terminated!\n");
            break;
        }
        
        
        size_t message_size = strlen(message);

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

        sendto(
            sockfd,
            buffer,
            buffer_size,
            0,
            (struct sockaddr *)&sendaddr,
            sizeof(sendaddr)
        );
        printf("Message has been sent successfully!\n");
        continue;

        
    }
    close(sockfd);
    

    return 1;
}




