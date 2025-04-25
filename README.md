# Camover, a MAC Address Randomizer
Camover is a command-line tool for generating network packets with randomized source MAC addresses to overflow switch CAM tables

## Usage 
```
Usage: camover [OPTION...]
-d, --delay=DELAY          Specifys a time in miliseconds to delay between
                             sending packets
  -i, --interface=INTERFACE  Specifys which internet interface to send packets
                             out of
  -n, --number=NUMBER_REPEAT How many unique MAC addresses will be sent
  -p, --packet=PACKET_FILE   Specify a .bin file to use as the base packet
  -v, --verbose              Produce verbose output
  -?, --help                 Give this help list
      --usage                Give a short usage message
```

Example:
```
camover -i eno0 -n 6500 -p exploit_packet.bin 
```
Sends 6500 packets over the interface eno0 using the packet file exploit_packet.bin

### Disclaimer
This program was designed for a cybersecurity lab, and should only be used on networks that you have explicit permission to run it on. This program serves as a demonstration for how specific exploits can operate and as a learning tool for cybersecuirty. It should never be used in a malicious manor. 
