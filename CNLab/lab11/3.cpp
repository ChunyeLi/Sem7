#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <linux/if_packet.h>
#include <linux/if.h>
#include <netinet/in.h>		 
#include <netinet/if_ether.h>    // for ethernet header
#include <netinet/ip.h>		// for ip header
#include <netinet/udp.h>		// for udp header
#include <netinet/tcp.h>
#include <arpa/inet.h>           // to avoid warning at inet_ntoa
#include <sys/ioctl.h>
#include <iostream>

using namespace std;

void errorExit(char *err) {
  perror(err);
  exit (-1);
}

// unsigned short checksum(unsigned short *buff, int _16bitword) {
//     unsigned long sum;
//     for (sum = 0; _16bitword > 0; _16bitword--)
//         sum += htons(*(buff)++);
//     do {
//         sum = ((sum >> 16) + (sum & 0xFFFF));
//     } while (sum & 0xFFFF0000);
//     return (~sum);
// }

int main (int argc, char *argv[]) {

  int sockRaw = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP)), sockSend = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sockRaw < 0 or sockSend < 0) {
    errorExit("Unable to open socket\n");
  }
  unsigned char *buffer = (unsigned char *) malloc(65536);
  struct sockaddr saddr;
  int saddr_len = sizeof (saddr);
  struct sockaddr_in dest;
  while (1) {
    memset(&dest, 0, sizeof (dest));
    memset(buffer, 0, sizeof (buffer));
    int buflen = recvfrom(sockRaw, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
    if (buflen < 0) {
      errorExit("Unable to receive message\n");
    }
    // ---------- set ip
    struct iphdr *ip = (struct iphdr *)buffer;
    dest.sin_addr.s_addr = ip->saddr;  // but first set dest
    dest.sin_port = htons(12000);
    dest.sin_family = AF_INET;
    printf("a...\n");
    if (strcmp("10.0.2.2", inet_ntoa(dest.sin_addr)) == 0 or strcmp("10.0.2.15", inet_ntoa(dest.sin_addr)) == 0) {  // dont want that 10.xxx~
      continue;
    }
    printf("%s\n", inet_ntoa(dest.sin_addr));
    dest.sin_addr.s_addr = ip->daddr;  // but first set dest
    if (strcmp ("192.168.20.1", inet_ntoa(dest.sin_addr)) == 0 or strcmp ("192.168.30.1", inet_ntoa(dest.sin_addr)) == 0) {
      continue;
    }
    printf("b...\n");
    // ip->ttl = ip->ttl - 1;
    // ip->check = 0;
    // ip->check = htons(checksum((unsigned short *)(buffer), (sizeof(struct iphdr) / 2)));
    if (sendto(sockSend, buffer, buflen, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
      errorExit("Unable to send");
    } else {
      printf("Eureka!\n");
    }
  }

  close(sockRaw);
  close(sockSend);    
}