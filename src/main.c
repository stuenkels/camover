#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <argp.h>
#include <strings.h>
#include "parseargs.h"
#include "getmac.h"
#include "ppb.h"
#include <time.h>
#include <stdlib.h>

int main (int argc, char **argv){

  srand(time(NULL));  //seed the random time, this has to only be done only once

  struct arguments arguments;  // Arugment struct which holds argument values

  setup_parser(argc, argv, &arguments); // Sets up the parser
  char default_packet[] ="default packet"; //very creative packet
    
  struct timespec ts1, ts2; //for dealy
  ts1.tv_sec = 0; //set to default of 0
  
  if(arguments.time <1000){
    ts1.tv_nsec = arguments.time *1000000; // Nanoseconds to miliseconds
  }else{
    printf("Delay time must be less than 1000ms (1 second)\n"); // Nanosleep returns error if nanoseconds value is over 1 second in length 
    return 1;
  }
  
  if(validate_interface(&arguments)==1){ // Checks if interface specified on execution is a valid interface
    return -1;
  }

	struct ifreq if_idx;
	struct ifreq if_mac;
  char sendBuff[1024]; // Maximum packet size, including both data and ethernet headers
  int tx_len = 0; // tx_len holds the index of sendBuff we are currently on

  // the beginning of send buff is now struct type ether_header
  struct ether_header *eh = (struct ether_header *) sendBuff;
  struct sockaddr_ll socket_address;
  
  int sockfd;
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
      printf("Error creating socket, are you root?\n"); // Raw sockets require root privilages
      return 1;
	}

  //Zero out if_idx
  memset(&if_idx, 0, sizeof(struct ifreq));
    //copy IF name from arguments.interface string to if_idx struct
	strncpy(if_idx.ifr_name, arguments.interface, IFNAMSIZ-1);
    //int ioctl(int fd, unsigned long op, ...);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0){  
	    printf("IOCTL call failed, are you root?\n");
      return 1;
  }

  memset(&if_mac, 0, sizeof(struct ifreq)); // zero out if_mac
  strncpy(if_mac.ifr_name, arguments.interface, IFNAMSIZ-1);
  if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0){
    printf("IOCTL call failed, are you root?\n");
    return 1;
  }

  memset(sendBuff, 0, 1024); //Zero out our send buffer
  /* Ethernet header */
  // Broadcast MAC as destination MAC
	eh->ether_dhost[0] = 0xff;
	eh->ether_dhost[1] = 0xff;
	eh->ether_dhost[2] = 0xff;
	eh->ether_dhost[3] = 0xff;
	eh->ether_dhost[4] = 0xff;
	eh->ether_dhost[5] = 0xff;
	/* Ethertype field */
  eh->ether_type = htons(ETH_P_IP); //What protocol
  tx_len += sizeof(struct ether_header); // Incremen tx_len index counter
    
  if(strcmp(arguments.packet_file, "") == 0){ // if there is no packet file specified
    if(arguments.verbose){
      printf("verbose: using the deault packet\n");
    }
    for(int i=0; i<strlen(default_packet); i++){
      sendBuff[tx_len++] = default_packet[i]; //copy default packet to sendbuff, this could be done with strcopy but uses for loop to make incrementing tx_len easier
    }
  }else{
    if(arguments.verbose){
      printf("verbose: using packet file %s\n", arguments.packet_file);
    }
    
    tx_len = read_packet_file(arguments.packet_file, sendBuff, tx_len); //load packet file into sendBuff
    if(tx_len == -1){
      printf("Error loading file %s\n", arguments.packet_file);
      return 1;
    }
  }
  //index
  socket_address.sll_ifindex = if_idx.ifr_ifindex;
  //address len
  socket_address.sll_halen = ETH_ALEN;
  /* Destination MAC */  //honestly have no idea what this is, i should probally figure it out 
	socket_address.sll_addr[0] = 0xcc;
	socket_address.sll_addr[1] = 0xb6;
	socket_address.sll_addr[2] = 0xc8;
	socket_address.sll_addr[3] = 0x08;
	socket_address.sll_addr[4] = 0x77;
	socket_address.sll_addr[5] = 0xbf; 


  int packets_sent = 0; 
  float percent; //exact value of completion in decimal value
  int percent_approx; //approx for drawing progress bar, ranges from 0-10

  while(arguments.number_repeat==0 || packets_sent<arguments.number_repeat){

    get_random_mac(eh); // Sets the source MAC address to a random value
    
    if (sendto(sockfd, sendBuff, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0){
      if(arguments.verbose){
        printf("verbose: send failed\n");
      }
    }
    packets_sent ++;
    if(arguments.verbose){
      if(arguments.number_repeat>0){
        printf("verbose: packet %d/%d sent\r",packets_sent, arguments.number_repeat);
      }
      else{
        printf("verbose: packet %d sent\r",packets_sent);
      }
      fflush(stdout); //whenever we use \r we have to fflush
    }else if(arguments.number_repeat>0){ //print progress bar

      percent = (float)packets_sent/arguments.number_repeat;
      percent_approx = (int)(percent*10);
      draw_progress_bar(percent_approx);
    }
    nanosleep(&ts1 , &ts2); //sleep
  }
  printf("\ndone.\n");
  return 0;
}