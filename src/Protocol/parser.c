//SarProtocol 21-11-2026
//Packet Parser Implementation by Deniz Sarp Yazicioglu
//all rights reserved


//libs
#include <stdint.h>
#include <stdio.h>
#include "packet.h"
#include <stdlib.h>
#include <stddef.h>

int main(void){

    return 0;
}

//funcs
Packet packet_creator(void){
    //empty packet creation
    Packet packet = {0};
    return packet;
}


size_t packet_parser(Packet *packet, const uint8_t *buffer, size_t buffer_size){
    //this function will read buffer byte by byte and fill the packet format properly.
    //The function returns an size_t which represents number of bytes read.

    //NULL checks
    if(buffer == NULL || packet == NULL){
        return 0;
    }

    //size check
    if(buffer_size < 8){
        return 0;
    }


    int index_counter = 0;
    int read_amnt = 0;

    while(true){
        //parsing all bytes
        if(index_counter == 0){
            uint8_t current_byte = buffer[index_counter];
            if(current_byte == NULL){
                return 0;
            }
            packet->header.version = current_byte;

            read_amnt++;
            index_counter++;
            continue;
        }
        if(index_counter == 1){
            uint8_t current_byte = buffer[index_counter];
            if(current_byte == NULL){
                return 0;
            }
            packet->header.type = current_byte;

            read_amnt++;
            index_counter++;
            continue;
        }
        if(index_counter == 2){
            packet->header.sequence_num = ((uint32_t)buffer[2] << 24) |
            ((uint32_t)buffer[3] << 16) | 
            ((uint32_t)buffer[4] << 8) |
            ((uint32_t)buffer[5]);

            index_counter = index_counter + 4;
            read_amnt = read_amnt + 4;
            continue;
        }
        if(index_counter == 6){
            packet->header.payload_length = ((uint32_t)buffer[6] << 8) |
            ((uint32_t)buffer[7]);

            index_counter = index_counter + 2;
            read_amnt = read_amnt + 2;
            continue;
        }
        if(index_counter == 8){
            packet->payload = buffer + 8;
            //payload is already a pointer copied with buffer's pointer + 8.
            read_amnt = read_amnt + packet->header.payload_length;
        }
        break;
    }
    return read_amnt;
} 