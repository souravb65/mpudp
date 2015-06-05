#include "tests/ip_utils.h"
#include <CUnit/CUnit.h>
#include "ip_utils.h"

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
    CU_ASSERT_EQUAL(ip_get_tos(&packet), 0);
    CU_ASSERT_EQUAL(ip_get_ttl(&packet), 16);
    CU_ASSERT_EQUAL(ip_get_proto(&packet), PROTO_UDP);
}

void test_ip_build_packet()
{
    ip_packet_t packet;

    char src_ip[] = "192.168.101.1";
    char dst_ip[] = "192.168.101.2";

    ip_build_packet(&packet, src_ip, dst_ip);

    ip_print_packet(&packet);

    uint32_t src_addr = ip_hdr_get_addr(&packet, ADDR_SRC);
    uint32_t dst_addr = ip_hdr_get_addr(&packet, ADDR_DST);

    CU_ASSERT_EQUAL(src_addr >>  24, 192);
    CU_ASSERT_EQUAL(src_addr & 0xFF, 1);

    CU_ASSERT_EQUAL(dst_addr >>  24, 192);
    CU_ASSERT_EQUAL(dst_addr & 0xFF, 2);
}

void test_ip_packet2chars()
{
    CU_FAIL("Finish the test!");
}

void test_ip_packet_len()
{
    CU_FAIL("Finish the test!");
}