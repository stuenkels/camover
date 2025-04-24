#ifndef HEADERFILE_GETMAC
#define HEADERFILE_GETMAC
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <sys/socket.h>
#include <net/if.h>
//very very complex file right here
void get_random_mac(struct ether_header *header);
int read_packet_file(char *file_name, char *packet_data, int index);

#endif