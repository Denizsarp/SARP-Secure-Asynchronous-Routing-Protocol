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

#define LISTEN_PORT 50000



int reciever_welcomer(void){
    printf("======================================\n");
    printf("SaRProtocol Message Reciever\n");
    printf("======================================\n");
    printf("\n");
    printf("\n");
    printf("All primary systems online\n");

    return 1;
}

void message_displayer(const uint8_t *message, size_t length){
    printf("======================================\n");
    printf("\n");
    fwrite(message, 1, length, stdout);
}




int main(void){

    reciever_welcomer();

    int sockfd;
    int int_bind;

    struct sockaddr_in recaddr; // should use sockaddr_un instead if we communicate between interdevice unix threads.
    struct sockaddr_in* sock_ptr = &recaddr;

    struct sockaddr_in clientaddr;


    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        printf("socket could not created!\n");
        return 1;
    }

    memset(sock_ptr, 0, sizeof(*sock_ptr));
    
    //recaddr config
    recaddr.sin_family = AF_INET; //should use AF_UNIX instead if we communicate between interdevice processes.
    recaddr.sin_port = htons(LISTEN_PORT);
    recaddr.sin_addr.s_addr = INADDR_ANY;

    int_bind = bind(
        sockfd,
        (struct sockaddr *)sock_ptr,
        sizeof(*sock_ptr)
    );

    if(int_bind < 0){
        printf("Error during socket binding!");
        close(sockfd);
        return 1;
    }


    ssize_t recieved_bytes;
    uint8_t buffer[1208]; // max packet size is 1208 bytes.
    size_t buffer_size = sizeof(buffer);

    socklen_t clientaddr_len = sizeof(clientaddr);
    socklen_t * socklen_ptr = &clientaddr_len;

    recieved_bytes = recvfrom(
        sockfd,
        buffer,
        buffer_size,
        0,
        (struct sockaddr *)&clientaddr,
        socklen_ptr
    );

    if(recieved_bytes < 0){
        printf("Could not recieve message!\n");
        close(sockfd);
        return 1;
    }
    else{
        Packet packet = packet_creator();
        Packet * packet_ptr = &packet;
        
        ssize_t parsed_bytes;

        parsed_bytes = packet_parser(packet_ptr, buffer, recieved_bytes);

        if(parsed_bytes < 0){
            printf("parsing error!");
            close(sockfd);
            return 1;
        }
        message_displayer(packet_ptr->payload, packet_ptr->header.payload_length);
        close(sockfd);
        return 0;
    }


}