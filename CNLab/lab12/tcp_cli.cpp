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
  int sock2 = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in servAddrh3;
  if (sock2 < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  int optval = 1;
  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  // -------- host 3: Now act as client
  servAddrh3.sin_family = AF_INET;
  servAddrh3.sin_port = htons(9567);
  if (inet_pton (AF_INET, "192.168.30.2", &servAddrh3.sin_addr) <= 0) {
    printf("Unable to open this IP");
    exit (1);
  }
  cout << "Sending to host h3\n";
  numIP toSend;
  strcpy(toSend.ip, "192.168.20.2");
  toSend.num = 5;
  toSend.port = 9567;
  if (connect(sock2, (struct sockaddr *)&servAddrh3, sizeof (servAddrh3)) < 0) {
    printf("Unable to connect!\n");
    exit (1);
  }
  write(sock2, (char *)&toSend, sizeof (toSend));
  char buf[1000];
  socklen_t len;
  int n = read(sock2, buf, 1000);
  printf("got at h1: %d\n", n);
  buf[n] = '\0';
  printf("got: %s\n", buf);
  close(sock2);
  return 0;
}