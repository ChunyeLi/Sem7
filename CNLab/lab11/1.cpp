#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<signal.h>
#include<stdbool.h>
#include<sys/socket.h>
#include<sys/types.h>

#include<linux/if_packet.h>
#include<netinet/in.h>		 
#include<netinet/if_ether.h>    // for ethernet header
#include<netinet/ip.h>		// for ip header
#include<netinet/udp.h>		// for udp header
#include<netinet/tcp.h>
#include<arpa/inet.h>           // to avoid warning at inet_ntoa
#include <iostream>

using namespace std;

void errorExit(char *err) {
  printf("%s\n", err);
  exit (-1);
}

FILE* fp;

int main (int argc, char *argv[]) {
  struct sockaddr saddr;
  struct sockaddr_in source, dest;
  fp = fopen ("out.txt", "w");
  int sock_r = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
  if(sock_r < 0) {
    errorExit("Unable to open socket\n");
  }
  unsigned char *buffer = (unsigned char *) malloc(65536); //to receive data
  int saddr_len = sizeof (saddr);
  while (1) {
    memset(buffer, 0, 65536);
    //Receive a network packet and copy in to buffer
    int buflen = recvfrom(sock_r, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
    if(buflen < 0) {
      errorExit("Unable to receive message\n");
    }
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    fprintf(fp, "Ethernet Header\n");
    fprintf(fp, "-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    fprintf(fp, "-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(fp,"-Protocol		: %d\n", eth->h_proto);

    struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    int iphdrlen = ip->ihl*4;
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = ip->saddr;     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = ip->daddr;     
    fprintf(fp, "\nIP Header\n");
    fprintf(fp, "\t|-Source IP         : %s\n", inet_ntoa(source.sin_addr));
    fprintf(fp, "\t|-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));
    fflush(fp);
  }
  // close(sock_r);
  return 0;
}