#include <net/ethernet.h> //ether_header struct type
#include <stdlib.h> //rand function
#include <stdio.h>

#include "getmac.h"

void get_random_mac(struct ether_header *header){
    header->ether_shost[0] = 0x08;
    header->ether_shost[1] = 0x00;
    header->ether_shost[2] = 0x27;
    header->ether_shost[3] = rand() % (255 + 1 - 0) + 0;
    header->ether_shost[4] = rand() % (255 + 1 - 0) + 0;
    header->ether_shost[5] = rand() % (255 + 1 - 0) + 0;
    return;
}

int read_packet_file(char *file_name, char *packet_data, int index){
    FILE *file;
    char *file_buffer;
    unsigned long file_len;
    file = fopen(file_name, "rb");
    if(!file){
       return -1; 
    }
    fseek(file, 0, SEEK_END);
    file_len = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    file_buffer = (char *)malloc(file_len+1);
    if(!file_buffer){
        return -1;
    }
    fread(file_buffer, file_len, 1, file);
    fclose(file);
    for(int i=0; i<file_len; i++){
        packet_data[index++] = file_buffer[i];
    }
    free(file_buffer);
    return index;
}