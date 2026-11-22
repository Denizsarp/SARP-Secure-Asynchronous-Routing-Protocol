// 20-11-2026
//PACKET DEFINING 
//Deniz Sarp Yazicioglu



#include <stdio.h>
#include <stdint.h>

//header = 8 byte
typedef struct{
    uint8_t version;
    uint8_t type;
    uint32_t sequence_num;
    uint16_t payload_length;  
}Header;


//packet = header + payload length byte
typedef struct{
    Header header;
    const uint8_t *payload;
}Packet;









