#include <stdio.h>
#include "parseargs.h"
#include <stdlib.h>     /* atoi */
#include <sys/types.h>
#include <ifaddrs.h>
#include <string.h>

static char args_doc[] = "";
static struct argp argp = {options, parse_opt, args_doc, doc};

error_t parse_opt (int key, char *arg, struct argp_state *state){
    struct arguments *arguments = state->input;
  
    switch (key)
      {
      case 'v':
        arguments->verbose = 1;
        break;
      case 'n':
        arguments->number_repeat = atoi(arg);
        break;
      case 'p':
        arguments->packet_file = arg;
        break;
      case 'i':
        arguments->interface = arg;
        break;
      case 'd':
        arguments->time = atoi(arg);
        break;
      case ARGP_KEY_END:
        break;
      default:
        return ARGP_ERR_UNKNOWN;
      }
    return 0;
  }

void get_valid_interfaces(){
    struct ifaddrs *addrs,*tmp;
    getifaddrs(&addrs);
    tmp = addrs;
    
    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
            printf("%s\n", tmp->ifa_name);
    
        tmp = tmp->ifa_next;
    }
    freeifaddrs(addrs);
}

void setup_parser(int argc, char **argv, struct arguments *arguments){
  const char *argp_program_bug_address ="<eli.stuenkel@gmail.com>";
  const char *argp_program_version = "camover 1.0";
  arguments->verbose = 0; //value of 1 means verbose is enabled
  arguments->number_repeat = 0; //default of 0, send packets until the world ends
  arguments->packet_file = ""; //Default to the default_packet
  arguments->interface = ""; //specify interface rather than ip
  arguments->time = 0; //no delay between packets by default
  argp_parse (&argp, argc, argv, 0, 0, arguments);
}

int validate_interface(struct arguments *arguments){
  if(strcmp(arguments->interface, "") == 0){
    printf("Specify a valid interface with -i:\n");
    get_valid_interfaces(); //print out a list of all valid interfaces
    return 1;
  }else{
    if(arguments->number_repeat!=0){
      printf("Sending %d packet(s) on interface %s\n", arguments->number_repeat, arguments->interface);
    }else{
      printf("Sending unlimited packets on interface %s\n", arguments->interface);
    }
    return 0;
  }
}