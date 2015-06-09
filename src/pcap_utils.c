#include <pcap.h>
#include "pcap_utils.h"
#include <stdio.h>
#include <string.h>
#include <net_utils.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int pcapu_find_any(pcap_if_t **dev)
{
    if(dev == NULL)
    {
        return -1;
    }

    pcap_if_t *alldevs, *d;
    char eb[PCAP_ERRBUF_SIZE];

    pcap_findalldevs(&alldevs, eb);

    for(d = alldevs; d != NULL; d = d->next)
    {
        if(strstr(d->name, "eth") || strstr(d->name, "wlan"))
        {
            *dev = d;
            return 0;
        }
    }

    return -1;
}

int pcapu_find_dev_by_name(pcap_if_t **dev, char *name)
{
    if(dev == NULL || name == NULL || strlen(name) == 0)
    {
        return -1;
    }

    pcap_if_t *alldevs, *d;
    char eb[PCAP_ERRBUF_SIZE];

    pcap_findalldevs(&alldevs, eb);

    for(d = alldevs; d != NULL; d = d->next)
    {
        if(strstr(d->name, name))
        {
            *dev = d;
            return 0;
        }
    }
}

char* pcapu_read_if_mac_s(char *dev_name, char **mac_s)
{
    char path[256];
    char *addr = malloc(MAC_LEN_S);
    size_t n = MAC_LEN_S;

    sprintf(path, "%s%s/address", SYSFS_DEV_PATH, dev_name);
    FILE *f = fopen(path, "r");

    getline(&addr, &n, f);
    fclose(f);

    addr[MAC_LEN_S-1] = 0;

    if(mac_s != NULL)
    {
        *mac_s = addr;
    }

    return addr;
}

char* pcapu_read_if_ip_s(pcap_if_t *if_desc, char **ip_s)
{
    pcap_addr_t *addr;
    char *addr_s;

    for(addr = if_desc->addresses; addr != NULL; addr = addr->next)
    {
        if(addr->addr->sa_family == AF_INET)
        {
            addr_s = inet_ntoa(((struct sockaddr_in*)addr->addr)->sin_addr);
        }
    }

    if(ip_s != NULL)
        *ip_s = addr_s;

    return addr_s;
}

void check_root()
{
    if(getuid() != 0)
    {
        puts("You should run this as root!");
        exit(-1);
    }
}

void pcapu_print_all_devs()
{
    pcap_if_t *alldevs, *d;
    char eb[PCAP_ERRBUF_SIZE];

    pcap_findalldevs(&alldevs, eb);

    for(d = alldevs; d != NULL; d = d->next)
    {
        puts(d->name);
    }
}

int pcapu_find_all_devs(char ***dev_arr)
{
    pcap_if_t *alldevs, *d;
    char eb[PCAP_ERRBUF_SIZE];

    int cursor = 0;

    *dev_arr = malloc(sizeof(char*)*3);
    char **tmp = (char**)*dev_arr;

    pcap_findalldevs(&alldevs, eb);

    for(d = alldevs; d != NULL; d = d->next)
    {
        if(strstr(d->name, "wlan") && d->name[4] > '0')
        {
            tmp[cursor] = malloc(5);
            strcpy(tmp[cursor], d->name);
            cursor++;
        }
    }

    return cursor;
}
