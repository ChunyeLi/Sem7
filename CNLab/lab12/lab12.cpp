/* 
---------SOURABH-------------------
-----------INSTRS------------------
* To write a NAT at router 1.
* map (192.168.20.2, somePort1) to (192.168.30.1, someOtherPort1)
* map (192.168.20.3, somePort2) to (192.168.30.1, someOtherPort2)
* similarly when r1 receives this, it has to reverse map it.
* very important: https://www.ibm.com/support/knowledgecenter/en/SSB27U_6.4.0/com.ibm.zvm.v640.kiml0/asonetw.html
* https://www.geeksforgeeks.org/user-datagram-protocol-udp/  
* ^- udp length includes the length of the header
------------------------------------
*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <assert.h>

#include <linux/if_packet.h>
#include <linux/if.h>
#include <netinet/in.h>		 
#include <netinet/if_ether.h>    // for ethernet header
#include <netinet/ip.h>		// for ip header
#include <netinet/udp.h>		// for udp header
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>           // to avoid warning at inet_ntoa
#include <sys/ioctl.h>
#include <iostream>
#include <map>

using namespace std;

char sourceAddr[100], destAddr[100];

void errorExit(char *err) {
  perror(err);
  exit (-1);
}

// **************************
// * CHECKSUM CODE IS TAKEN FROM https://gist.github.com/david-hoze/0c7021434796997a4ca42d7731a7073a ******

/* Compute checksum for count bytes starting at addr, using one's complement of one's complement sum*/
static unsigned short compute_checksum(unsigned short *addr, unsigned int count) {
  register unsigned long sum = 0;
  while (count > 1) {
    sum += * addr++;
    count -= 2;
  }
  //if any bytes left, pad the bytes and add
  if(count > 0) {
    sum += ((*addr)&htons(0xFF00));
  }
  //Fold sum to 16 bits: add carrier to result
  while (sum>>16) {
      sum = (sum & 0xffff) + (sum >> 16);
  }
  //one's complement
  sum = ~sum;
  return ((unsigned short)sum);
}

/* set ip checksum of a given ip header*/
void compute_ip_checksum(struct iphdr* iphdrp){
  iphdrp->check = 0;
  iphdrp->check = compute_checksum((unsigned short*)iphdrp, iphdrp->ihl<<2);
}

/* set tcp checksum: given IP header and UDP datagram */
void compute_udp_checksum(struct iphdr *pIph, unsigned short *ipPayload) {
  register unsigned long sum = 0;
  struct udphdr *udphdrp = (struct udphdr*)(ipPayload);
  unsigned short udpLen = htons(udphdrp->len);
  //printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~udp len=%dn", udpLen);
  //add the pseudo header 
  //printf("add pseudo headern");
  //the source ip
  sum += (pIph->saddr>>16)&0xFFFF;
  sum += (pIph->saddr)&0xFFFF;
  //the dest ip
  sum += (pIph->daddr>>16)&0xFFFF;
  sum += (pIph->daddr)&0xFFFF;
  //protocol and reserved: 17
  sum += htons(IPPROTO_UDP);
  //the length
  sum += udphdrp->len;

  //add the IP payload
  //printf("add ip payloadn")
  //initialize checksum to 0
  udphdrp->check = 0;
  while (udpLen > 1) {
      sum += * ipPayload++;
      udpLen -= 2;
  }
  //if any bytes left, pad the bytes and add
  if(udpLen > 0) {
      //printf("+++++++++++++++padding: %dn", udpLen);
      sum += ((*ipPayload)&htons(0xFF00));
  }
    //Fold sum to 16 bits: add carrier to result
  //printf("add carriern");
    while (sum>>16) {
        sum = (sum & 0xffff) + (sum >> 16);
    }
  //printf("one's complementn");
    sum = ~sum;
  //set computation result
  udphdrp->check = ((unsigned short)sum == 0x0000)?0xFFFF:(unsigned short)sum;
}

/* set tcp checksum: given IP header and tcp segment */
void compute_tcp_checksum(struct iphdr *pIph, unsigned short *ipPayload) {
    register unsigned long sum = 0;
    unsigned short tcpLen = ntohs(pIph->tot_len) - (pIph->ihl<<2);
    struct tcphdr *tcphdrp = (struct tcphdr*)(ipPayload);
    //add the pseudo header 
    //the source ip
    sum += (pIph->saddr>>16)&0xFFFF;
    sum += (pIph->saddr)&0xFFFF;
    //the dest ip
    sum += (pIph->daddr>>16)&0xFFFF;
    sum += (pIph->daddr)&0xFFFF;
    //protocol and reserved: 6
    sum += htons(IPPROTO_TCP);
    //the length
    sum += htons(tcpLen);
 
    //add the IP payload
    //initialize checksum to 0
    tcphdrp->check = 0;
    while (tcpLen > 1) {
        sum += * ipPayload++;
        tcpLen -= 2;
    }
    //if any bytes left, pad the bytes and add
    if(tcpLen > 0) {
        //printf("+++++++++++padding, %dn", tcpLen);
        sum += ((*ipPayload)&htons(0xFF00));
    }
      //Fold 32-bit sum to 16 bits: add carrier to result
      while (sum>>16) {
          sum = (sum & 0xffff) + (sum >> 16);
      }
      sum = ~sum;
    //set computation result
    tcphdrp->check = (unsigned short)sum;
}

void printIP(struct iphdr *ip) {
  char sourceAddr2[100], destAddr2[100];
  struct sockaddr_in dest;
  memset(&dest, 0, sizeof (dest));
  dest.sin_addr.s_addr = ip->saddr;
  char *tmp = inet_ntoa(dest.sin_addr);
  strcpy(sourceAddr2, tmp);
  dest.sin_addr.s_addr = ip->daddr;  // but first set dest
  tmp = inet_ntoa(dest.sin_addr);
  strcpy(destAddr2, tmp);
  printf("\nIP Header\n");
  printf("\t|-Version              : %d\n", (unsigned int)ip->version);
  printf("\t|-Internet Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl, ((unsigned int)(ip->ihl))*4);
  printf("\t|-Type Of Service   : %d\n", (unsigned int)ip->tos);
  printf("\t|-Total Length      : %d  Bytes\n", ntohs(ip->tot_len));
  printf("\t|-Identification    : %d\n", ntohs(ip->id));
  printf("\t|-Time To Live	    : %d\n", (unsigned int)ip->ttl);
  printf("\t|-Protocol 	    : %d\n", (unsigned int)ip->protocol);
  printf("\t|-Header Checksum   : %d\n", ntohs(ip->check));
  printf("\t|-Source IP         : %s\n", sourceAddr2);
  printf("\t|-Destination IP    : %s\n", destAddr2);
}

void printUDP(struct udphdr *udp) {
	printf("\nUDP Header\n");
	printf("\t|-Source Port    	: %d\n" , ntohs(udp->source));
	printf("\t|-Destination Port	: %d\n" , ntohs(udp->dest));
	printf("\t|-UDP Length      	: %d\n" , ntohs(udp->len));
	printf("\t|-UDP Checksum   	: %d\n" , ntohs(udp->check));
}

void printTCP(struct tcphdr *tcp) {
  printf("\nTCP Header\n");
  printf("\t|-Source Port          : %u\n",ntohs(tcp->source));
  printf("\t|-Destination Port     : %u\n",ntohs(tcp->dest));
  printf("\t|-Sequence Number      : %u\n",ntohl(tcp->seq));
  printf("\t|-Acknowledge Number   : %u\n",ntohl(tcp->ack_seq));
  printf("\t|-Header Length        : %d DWORDS or %d BYTES\n" ,(unsigned int)tcp->doff,(unsigned int)tcp->doff*4);
  printf("\t|----------Flags-----------\n");
  printf("\t\t|-Urgent Flag          : %d\n",(unsigned int)tcp->urg);
  printf("\t\t|-Acknowledgement Flag : %d\n",(unsigned int)tcp->ack);
  printf("\t\t|-Push Flag            : %d\n",(unsigned int)tcp->psh);
  printf("\t\t|-Reset Flag           : %d\n",(unsigned int)tcp->rst);
  printf("\t\t|-Synchronise Flag     : %d\n",(unsigned int)tcp->syn);
  printf("\t\t|-Finish Flag          : %d\n",(unsigned int)tcp->fin);
  printf("\t|-Window size          : %d\n",ntohs(tcp->window));
  printf("\t|-Checksum             : %d\n",ntohs(tcp->check));
  printf("\t|-Urgent Pointer       : %d\n",tcp->urg_ptr);

}

int main (int argc, char *argv[]) {

  int sockRaw = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_IP)), sockSend = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
  if (sockRaw < 0 or sockSend < 0) {
    errorExit("Unable to open socket\n");
  }
  unsigned char *buffer = (unsigned char *) malloc(65536);
  struct sockaddr saddr;
  int saddr_len = sizeof (saddr);
  struct sockaddr_in dest;
  map<pair<uint32_t, uint16_t>, uint16_t> pmap;
  map<uint16_t, pair<uint32_t, uint16_t> > pmaprev;
  uint16_t portCount = 49153;

  while (1) {
    memset(&dest, 0, sizeof (dest));
    memset(buffer, 0, sizeof (buffer));
    int buflen = recvfrom(sockRaw, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_len);
    if (buflen < 0) {
      errorExit("Unable to receive message\n");
    }
    // ---------- set ip
    struct iphdr *ip = (struct iphdr *)buffer;
    dest.sin_addr.s_addr = ip->saddr;
    char *tmp = inet_ntoa(dest.sin_addr);
    strcpy(sourceAddr, tmp);
    if (strcmp("10.0.2.2", sourceAddr) == 0 or strcmp("10.0.2.15", sourceAddr) == 0) {  // dont want that 10.xxx~
      continue;
    }
    dest.sin_addr.s_addr = ip->daddr;  // but first set dest
    tmp = inet_ntoa(dest.sin_addr);
    strcpy(destAddr, tmp);
    if (strcmp("10.0.2.15", destAddr) == 0) {
      continue;
    }
    printf("IP Header before modification\n");    
    printIP(ip);
    if (ip->protocol == IPPROTO_UDP) {
      struct udphdr *udp = (struct udphdr*)(buffer + sizeof(struct iphdr));
      printUDP(udp);
      if (strcmp("192.168.20.2", sourceAddr) == 0 or strcmp("192.168.20.3", sourceAddr) == 0) {
        if (!pmap.count(make_pair(ip->saddr, udp->source))) {
          pmap[make_pair(ip->saddr, udp->source)] = htons(portCount);
          pmaprev[htons(portCount)] = make_pair(ip->saddr, udp->source);
          portCount++;
        }
        udp->source = pmap[make_pair(ip->saddr, udp->source)];
        ip->saddr = inet_addr("192.168.30.1");
      } else {
        tie(ip->daddr, udp->dest) = pmaprev[udp->dest];
      }
      ip->ttl = ip->ttl - 1;
      compute_ip_checksum(ip);
      printf("IP Header after modification\n");
      printIP(ip);
      compute_udp_checksum(ip, (unsigned short *)udp);
      printUDP(udp);
      dest.sin_addr.s_addr = ip->daddr;
      dest.sin_family = AF_INET;
      dest.sin_port = udp->dest;
    } else if (ip->protocol == IPPROTO_TCP) {
      tcphdr *tcp = (struct tcphdr*)(buffer + sizeof(struct iphdr));
      printTCP(tcp);
      if (strcmp("192.168.20.2", sourceAddr) == 0 or strcmp("192.168.20.3", sourceAddr) == 0) {
        if (!pmap.count(make_pair(ip->saddr, tcp->source))) {
          pmap[make_pair(ip->saddr, tcp->source)] = htons(portCount);
          pmaprev[htons(portCount)] = make_pair(ip->saddr, tcp->source);
          portCount++;
        }
        tcp->source = pmap[make_pair(ip->saddr, tcp->source)];
        ip->saddr = inet_addr("192.168.30.1");
      } else {
        tie(ip->daddr, tcp->dest) = pmaprev[tcp->dest];
      }
      ip->ttl = ip->ttl - 1;
      compute_ip_checksum(ip);
      printf("IP Header after modification\n");
      printIP(ip);
      compute_tcp_checksum(ip, (unsigned short *)tcp);
      printTCP(tcp);
      dest.sin_addr.s_addr = ip->daddr;
      dest.sin_family = AF_INET;
      dest.sin_port = tcp->dest;
    }
    if (sendto(sockSend, buffer, buflen, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
      errorExit("Unable to send");
    } else {
      printf("Eureka!\n");
    }
  }

  close(sockRaw);
  close(sockSend);    
}