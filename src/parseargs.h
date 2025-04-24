#ifndef HEADERFILE_PARSEARGS
#define HEADERFILE_PARSEARGS
#include <argp.h>

error_t parse_opt(int key, char *arg, struct argp_state *state);
void get_valid_interfaces(); 

static char doc[] ="Mac address randomizer and flooder for filling switch cam tables";
struct arguments{
  int verbose;              /* The -v flag */
  int random;               //Random flag
  int number_repeat;        //number of repeat times
  char *packet_file;        //Packet template file
  char *interface;          //interface name
  int time;
};

static struct argp_option options[] ={
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"number",   'n', "NUMBER_REPEAT", 0,"How many unique MAC addresses will be sent"},
  {"packet",   'p', "PACKET_FILE", 0,"Specify a .bin file to use as the base packet"},
  {"interface", 'i', "INTERFACE", 0, "Specifys which internet interface to send packets out of"},
  {"delay", 'd', "DELAY", 0, "Specifys a time in miliseconds to delay between sending packets"},
  {0}
};

void setup_parser(int argc, char **argv, struct arguments *arguments);
int validate_interface(struct arguments *arguments);
#endif