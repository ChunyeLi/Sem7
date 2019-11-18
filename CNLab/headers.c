#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>  // fork, exec, etc...
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <netinet/in.h>		 
#include <netinet/if_ether.h>    // for ethernet header
#include <netinet/ip.h>		// for ip header
#include <netinet/udp.h>		// for udp header
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <arpa/inet.h>           // to avoid warning at inet_ntoa
#include <netdb.h>
