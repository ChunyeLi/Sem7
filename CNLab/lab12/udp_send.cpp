#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <stack>
#include <string>
#include <iostream>

using namespace std;

struct numIP {
  int num, port;
  char ip[1000];
};

int main(int argc, char *argv[]) {
  int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  // First argument is the "domain" which specifies the communication domain, "AF_INET" stands for ipv4 internet protocols
  // Second argument tells the "type" which specifies the communication semantics.
  // Third argument is for "protocol" which specifies a particular protocol to be used with the socket, normally only a single protocol exists to support a particular socket.
  struct sockaddr_in servAddrh3;
  if (sock2 < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  int optval = 1;
  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
  // When manipulating socket options, the level at which the option resides and the name of the option must be  specified. To manipulate  options  at  the sockets API level, level is specified as SOL_SOCKET. 
  // Most socket-level options utilize an int argument for optval.  For setsockopt(), the argument should be nonzero to enable a boolean option, or zero if the option is to be disabled.
  // -------- host 3: Now act as client
  servAddrh3.sin_family = AF_INET;
  servAddrh3.sin_port = htons(9567);
  if (inet_pton (AF_INET, "192.168.30.2", &servAddrh3.sin_addr) <= 0) {
    // inet_pton converts ipv4 and ipv6 addresses from text to binary form
    printf("Unable to open this IP");
    exit (1);
  }
  cout << "Sending to host h3\n";
  numIP toSend;
  strcpy(toSend.ip, "192.168.20.2");
  toSend.num = 5;
  toSend.port = 9567;
  sendto(sock2, (char *)&toSend, sizeof (toSend), 0, (struct sockaddr *)&servAddrh3, sizeof (servAddrh3));
  // that fourth argument is flags, which we don't need.
  char buf[1000];
  socklen_t len;
  int n = recvfrom(sock2, buf, 1000, 0, NULL, NULL);
  // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
  // len contains the size of the buffer.
  // If  src_addr  is  not  NULL, and the underlying protocol provides the source address of the message, that source address is placed in the buffer pointed to by src_addr.  In this case, addrlen is a value-result argument.  Before the call, it should be  initialized  to the size of the buffer associated with src_addr.  Upon return, addrlen is updated to contain the actual size of the source address.  The returned address is truncated if the buffer provided is too small; in this  case,  addrlen will return a value greater than was supplied to the call. If the caller is not interested in the source address, src_addr and addrlen should be specified as NULL.
  // recvfrom, recv, read all return the length of the message on successful completion. If a message is too long to fit in the supplied buffer, excess bytes may be discarded depending on the type of socket the message is received from.
  printf("got at h1: %d\n", n);
  buf[n] = '\0';
  printf("got: %s\n", buf);
  close(sock2);
  return 0;
}