#include "ip_utils.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>

int ip_build_packet(ip_packet_t *packet, char *src_ip, char *dst_ip)
{
    ip_hdr_set_common(packet);

    ip_hdr_set_addr(packet, ADDR_SRC, src_ip);
    ip_hdr_set_addr(packet, ADDR_DST, dst_ip);

    packet->payload = NULL;

    return 0;
}

void ip_hdr_set_common(ip_packet_t *packet)
{
    // set ip proto version to IPv4
    // and set internet header len to 20
    packet->first[0] = 4 << 4 | 5;

    // set type of service to zero - we don't use it
    packet->first[1] = 0;

    // set initial total length just to header len (in bytes)
    ip_set_len(packet, 20);

    // set id, fragment num and fragmentation bits to 0
    memset(packet->second, 0, 4);

    // set TTL to 16
    packet->third[0] = 16;

    // set protocol to UDP
    packet->third[1] = PROTO_UDP;
}

void ip_print_packet(ip_packet_t *packet)
{
    puts("\nDUMPING IP PACKET");
    printf("Version:\t%d\n", ip_get_version(packet));
    printf("IHL:\t\t%u\n", ip_get_ihl(packet));
    printf("Len:\t\t%u\n", ip_get_len(packet));
    printf("TOS:\t\t%u\n", ip_get_tos(packet));
    printf("TTL:\t\t%u\n", ip_get_ttl(packet));
    printf("Proto:\t\t%u\n", ip_get_proto(packet));
    printf("Src:\t\t%s\n", ip_hdr_get_addr_s(packet, ADDR_SRC));
    printf("Dst:\t\t%s\n", ip_hdr_get_addr_s(packet, ADDR_DST));
}

uint8_t ip_get_version(ip_packet_t *packet)
{
    return packet->first[0] >> 4;
}

uint8_t ip_get_ihl(ip_packet_t *packet)
{
    return packet->first[0] & 0xF;
}

uint8_t ip_get_tos(ip_packet_t *packet)
{
    return packet->first[1];
}

uint8_t ip_get_ttl(ip_packet_t *packet)
{
    return packet->third[0];
}

uint8_t ip_get_proto(ip_packet_t *packet)
{
    return packet->third[1];
}

uint16_t ip_get_crc(ip_packet_t *packet)
{
    // return crc
    // return get_octet(&packet->third)
    return -1;
}

int ip_hdr_set_addr(ip_packet_t *packet, int way, char *addr)
{
    uint32_t a;
    inet_pton(AF_INET, addr, &a);
    memcpy(packet->first+way*4, &a, 4);

    return 0;
}

uint32_t ip_hdr_get_addr(ip_packet_t *packet, int way)
{
    uint32_t res;
    memcpy(&res, packet->first+way*4, 4);
    res = htonl(res);
    return res;
}

char* ip_hdr_get_addr_s(ip_packet_t *packet, int way)
{
    struct in_addr addr;
    uint32_t my_addr = ip_hdr_get_addr(packet, way);
    uint32_t rev = htonl(my_addr);
    memcpy(&addr, &rev, 4);

    return inet_ntoa(addr);
}

uint16_t ip_get_len(ip_packet_t *packet)
{
    return packet->first[2] << 8 | packet->first[3];
}

uint16_t ip_set_len(ip_packet_t *packet, uint16_t len)
{
    packet->first[2] = len >> 8;
    packet->first[3] = len & 0xFF;
    return 0;
}

int ip_packet2chars(ip_packet_t *packet, unsigned char **buff)
{
    uint16_t len = ip_get_len(packet);
    *buff = malloc(len);

    // header len in bytes
    uint8_t  ihl_b       = ip_get_ihl(packet)*4;
    uint32_t payload_len = ip_get_len(packet)-ihl_b;

    memcpy(*buff, packet, ihl_b);
    memcpy(*buff+ihl_b, packet->payload, payload_len);

    return 0;
}

int ip_set_data(ip_packet_t *packet, unsigned char *data, int len)
{
    if(packet->payload)
        free(packet->payload);

    packet->payload = malloc(len);
    memcpy(packet->payload, data, len);

    uint16_t packet_len = len + ip_get_ihl(packet)*4;
    ip_set_len(packet, packet_len);

    return 0;
}
