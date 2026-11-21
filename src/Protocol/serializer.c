#include <stdint.h>
#include <stdio.h>
#include "packet.h"
#include <stdlib.h>
#include <stddef.h>




uint8_t *create_buffer(const Packet *packet, size_t *buffer_size){
    if(packet == NULL || buffer_size == NULL){
        return NULL;
    }

    size_t payload_size = packet->header.payload_length;
    size_t header_size = 8;

    size_t packet_total_size = header_size + payload_size;

    uint8_t *buffer = malloc(packet_total_size);

    if(buffer == NULL){
        return NULL;
    }
    *buffer_size = packet_total_size;
    return buffer;

}

size_t serialize_packet(const Packet *packet, uint8_t *buffer, size_t buffer_size){
    //every index of buffer is 1 byte exactly this function must serialie all bytes in a single packet and add them into-
    //given buffer. It also returns size of added buffer.

    //NULL check

    size_t write_amnt = 0;

    if(packet == NULL || buffer == NULL){
        return 0;
    }
    if(buffer_size < 8){
        return 0;
    }
    
    


    //size calcualations
    size_t header_size = 8;
    size_t payload_size = packet->header.payload_length;

    size_t packet_size;
    size_t *total_packet_ptr = &packet_size;
    
    *total_packet_ptr = header_size + payload_size;

    if(buffer_size < packet_size){
        return 0;
    }

    //serialization processes
    uint16_t index_counter = 0;
    while(true){
        if(index_counter == 0){
            // 1 byte serializing version.
            buffer[index_counter] = packet->header.version;
            index_counter++;
            write_amnt++;
            continue;
        }
        if(index_counter == 1){
            // 1 byte serializing type.
            buffer[index_counter] = packet->header.type;
            index_counter++;
            write_amnt++;
            continue;
        }
        if(index_counter == 2){
            // 4 byte serializing sequence num.

            int placeholder = index_counter;

            buffer[placeholder] = (packet->header.sequence_num >> 24) & 0xFF;
            buffer[placeholder + 1] = (packet->header.sequence_num >> 16) & 0xFF;
            buffer[placeholder + 2] = (packet->header.sequence_num >> 8) & 0xFF;
            buffer[placeholder + 3] = (packet->header.sequence_num) & 0xFF;

            index_counter = index_counter + 4;
            write_amnt = write_amnt + 4;
            continue;
    
        }
        if(index_counter == 6){
            // 2 bytes serializing payload length.
            int placeholder = index_counter;

            buffer[placeholder] = (packet->header.payload_length >> 8) & 0xFF;
            buffer[placeholder + 1] = (packet->header.payload_length) & 0xFF;

            index_counter = index_counter + 2;
            write_amnt = write_amnt + 2;
            continue;

        }

        if(index_counter == 8){
            // changing bytes payload serializing.
            int placeholder = index_counter;

            for(int i = 0; i < packet->header.payload_length; i++){
                uint8_t current_byte = *(packet->payload + i);
                buffer[placeholder + i] = current_byte;
            }
            //paylaod has been written.
            write_amnt = write_amnt + packet->header.payload_length; 
        }
        break;
    }
    return write_amnt;
    



}





