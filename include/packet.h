
#ifndef PACKET_H
#define PACKET_H
#include <stdint.h>

//header = 8 byte
typedef struct{
    uint8_t version; // 1byte
    uint8_t type;     // 1 byte
    uint32_t sequence_num;  // 4 byte
    uint16_t payload_length;  // 2 byte
}Header;


//packet = header + payload length byte
typedef struct{
    Header header; // 8 byte
    const uint8_t *payload; // changing bytes
}Packet;


#endif









