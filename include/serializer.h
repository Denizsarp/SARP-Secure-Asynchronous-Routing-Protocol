#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <stdint.h>
#include <stddef.h>
#include "packet.h"


//signatures
uint8_t *create_buffer(const Packet *packet, size_t *buffer_size);

//serializing function signature
size_t serialize_packet(
    const Packet *packet,
    uint8_t *buffer,
    size_t buffer_size
);

#endif

