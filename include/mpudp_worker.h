#ifndef WORKER_H
#define WORKER_H

#include <pthread.h>
#include "mpudp_utils.h"
#include "mpudp_monitor.h"
#include <pcap.h>

#define WORKER_NOT_CONNECTED 0
#define WORKER_CONNECTED     1

typedef struct worker worker_t;
typedef struct monitor monitor_t;

struct worker {
    int id;
    pthread_t rx_thread_id;
    pthread_t tx_thread_id;
    pthread_t global_tx_watcher_id;

    mpudp_packet_t *private_tx_buff;
    mpudp_packet_t *wait_ack_buff;

    monitor_t *m;

    int choke;

    char *src_ip;
    char *dst_ip;

    uint16_t src_port;
    uint16_t dst_port;

    char *src_mac;
    char *dst_mac;

    char *bcast_mac;
    char *bcast_ip;

    pcap_t    *if_handle;
    pcap_if_t *if_desc;

    uint8_t state;

    char name[6];

    pthread_mutex_t config_mx;
    pthread_mutex_t private_tx_buff_mx;
    pthread_mutex_t wait_ack_buff_mx;
    pthread_cond_t  tx_ready;
    pthread_cond_t  tx_empty;
};

void* worker_tx_thread(void *arg);
void* worker_rx_thread(void *arg);
void* worker_tx_watcher_thread(void*arg);
worker_t* init_worker(int, char*, monitor_t*, float);
int spawn_worker(worker_t* spawn_worker);
int worker_send_packet(worker_t*, mpudp_packet_t*);
int worker_send_bcast(worker_t*, mpudp_packet_t*);
int worker_send_ack(worker_t*, int8_t);
int watchdog_check_state(worker_t*);

#endif
