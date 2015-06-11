#include "mpudp_utils.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "mpudp_monitor.h"
#include "net_utils.h"

void init_buffer(mpudp_buff_t *buff)
{
    buff->head = 0;
    buff->tail = 0;
    buff->num  = 0;

    pthread_cond_init(&buff->empty, NULL);
    pthread_cond_init(&buff->full,  NULL);
    pthread_mutex_init(&buff->mx,   NULL);
}

int mpudp_prepare_packet(mpudp_packet_t **packet, uint8_t *data, int len)
{
    // TODO: watch the MTU cap
    *packet = malloc(sizeof(mpudp_packet_t));
    mpudp_packet_t *p = *packet;

    p->id = -1;
    p->payload = malloc(len);
    memcpy(p->payload, data, len);
    p->len = len;
    return len;
}

void mpudp_build_config(monitor_t *m, mpudp_config_t *config)
{
    config->num_if = m->num_workers;

    config->if_list = malloc(sizeof(mpudp_if_desc_t)*config->num_if);

    int i;
    for(i = 0; i < config->num_if; i++)
    {
        strcpy(config->if_list[i].name, m->workers[i]->name);
        mac2chars(m->workers[i]->src_mac, config->if_list[i].mac);
        ip2chars(m->workers[i]->src_ip, &config->if_list[i].ip);

        config->if_list[i].port = m->workers[i]->src_port;
    }
}

int mpudp_config2chars(mpudp_config_t *config, uint8_t **payload)
{
    int PREFIX_LEN = MPUDP_CONFIG_PREFIX_LEN;
    int DESC_LEN = MPUDP_IFACE_DESC_LEN;
    int len = PREFIX_LEN + config->num_if*DESC_LEN;

    *payload = malloc(len);
    char *dest = *payload;

    dest[0] = config->id;
    dest[1] = config->num_if;

    int i;
    for(i = 0; i < config->num_if; i++)
    {
        memcpy(dest+PREFIX_LEN+i*DESC_LEN, &config->if_list[i], DESC_LEN);
    }

    return len;
}

int mpudp_chars2config(mpudp_config_t *config, uint8_t *data, int len)
{
    int DESC_LEN   = MPUDP_IFACE_DESC_LEN;
    int PREFIX_LEN = MPUDP_CONFIG_PREFIX_LEN;

    config->id      = data[0];
    config->num_if  = data[1];
    config->if_list = malloc(DESC_LEN*config->num_if);

    int i;
    for(i = 0; i < config->num_if; i++)
    {
        memcpy(&config->if_list[i], data+PREFIX_LEN+DESC_LEN*i, DESC_LEN);
    }

    return 0;
}

int mpudp_packet2chars(mpudp_packet_t *packet, uint8_t **payload)
{
    *payload = malloc(1+4+4+packet->len);
    uint8_t *dst = *payload;

    memcpy(dst, packet, 9);
    memcpy(dst+9, packet->payload, packet->len);

    return 9+packet->len;
}

int mpudp_chars2packet(mpudp_packet_t *packet, uint8_t *payload, int len)
{
    memcpy(packet, payload, 9);
    packet->payload = malloc(packet->len);
    memcpy(packet->payload, payload+9, packet->len);

    return 0;
}
