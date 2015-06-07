#include "tests/ip_utils.h"
#include <CUnit/CUnit.h>
#include "ip_utils.h"
#include <stdio.h>

char src_ip[] = "192.168.101.1";
char dst_ip[] = "192.168.101.2";

int init_ip_utils()
{
    return 0;
}

int clean_ip_utils()
{
    return 0;
}

void test_ip_set_common()
{
    ip_packet_t packet;
    ip_hdr_set_common(&packet);

    CU_ASSERT_EQUAL(ip_get_version(&packet), 4);
    CU_ASSERT_EQUAL(ip_get_ihl(&packet), 5);
    CU_ASSERT_EQUAL(ip_get_len(&packet), 20);
    CU_ASSERT_EQUAL(ip_get_tos(&packet), 0);
    CU_ASSERT_EQUAL(ip_get_ttl(&packet), 16);
    CU_ASSERT_EQUAL(ip_get_proto(&packet), PROTO_UDP);
}

void test_ip_build_packet()
{
    ip_packet_t packet;

    ip_build_packet(&packet, src_ip, dst_ip);

    uint32_t src_addr = ip_hdr_get_addr(&packet, ADDR_SRC);
    uint32_t dst_addr = ip_hdr_get_addr(&packet, ADDR_DST);

    CU_ASSERT_EQUAL(src_addr >>  24, 192);
    CU_ASSERT_EQUAL(src_addr & 0xFF, 1);

    CU_ASSERT_EQUAL(dst_addr >>  24, 192);
    CU_ASSERT_EQUAL(dst_addr & 0xFF, 2);

    CU_ASSERT_PTR_NULL(packet.payload);
}

void test_ip_packet2chars()
{
    ip_packet_t packet;

    ip_build_packet(&packet, src_ip, dst_ip);

    unsigned char *buff;
    ip_packet2chars(&packet, &buff);

    uint16_t len = ip_get_len(&packet);
    int i = 0;

    CU_ASSERT_EQUAL(*(buff+12), 192);
}

void test_ip_packet_len()
{
    ip_packet_t packet;

    ip_build_packet(&packet, src_ip, dst_ip);
    CU_ASSERT_EQUAL(ip_get_len(&packet), 20);
}

void test_ip_set_data()
{
    ip_packet_t packet;
    unsigned char data[] = "hello world";

    ip_build_packet(&packet, src_ip, dst_ip);
    ip_set_data(&packet, data, strlen(data));

    CU_ASSERT_EQUAL(ip_get_len(&packet)-ip_get_ihl(&packet)*4, strlen(data));
    CU_ASSERT_EQUAL(strncmp(data, packet.payload, strlen(data)), 0);
}

void test_ip_packet2chars_payload()
{
    ip_packet_t packet;
    unsigned char data[] = "hello world";

    ip_build_packet(&packet, src_ip, dst_ip);
    ip_set_data(&packet, data, strlen(data));

    unsigned char *buff;
    ip_packet2chars(&packet, &buff);

    CU_ASSERT_EQUAL(buff[0], 0x45);
    CU_ASSERT_EQUAL(buff[12], 192);
    CU_ASSERT_EQUAL(buff[20], 'h');
}
