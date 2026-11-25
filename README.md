# 🌐 SARP (Secure Asynchronous Routing Protocol)

A custom communication protocol alternative to TCP/IP-UDP protocols developed from scratch in C.

SARProtocol is a low-level networking project focused on designing and implementing a custom packet format and communication protocol.

## Packet Structure

- Custom packet header
- Sequence number
- Payload length
- Variable-length payload
- Big-endian byte order

## Serialization & Parsing

- Custom packet serialization
- Packet parsing and validation
- Byte-level data handling

## Communication

- UDP-based communication over IPv4
- Local network communication
- Sender and receiver nodes
- Multithreaded message handling

## Technologies

- C
- POSIX Sockets
- POSIX Threads (pthreads)
- UDP / IPv4
- Linux

## Development Status

SARProtocol is actively under development.

Current focus:
- Completing the custom packet protocol
- Improving packet validation and error handling
- Developing a custom transport layer
- Moving beyond UDP-based communication
