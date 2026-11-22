#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stddef.h>
#include "packet.h"

//signatures
Packet packet_creator(void);

ssize_t packet_parser(
    Packet *packet,
    const uint8_t *buffer,
    size_t buffer_size
);

#endif