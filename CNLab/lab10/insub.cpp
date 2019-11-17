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

struct operands {
  int operands[2], port;
};

struct numIP {
  int num, port;
  char ip[1000];
};

void errorExit(char *err) {
  printf("%s\n", err);
  exit (1);
}

int main(int argc, char *argv[]) {
  int optval = 1;
  int h2ServersSock = atoi(argv[1]);
  int sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock2 < 0) {
    errorExit("Unable to create socket\n");
  }
  int h1sock; 
  struct sockaddr_in cliAddrh1, servAddrh3;
  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  socklen_t clilenh1 = sizeof(cliAddrh1); 
  h1sock = accept(h2ServersSock, (struct sockaddr *)&cliAddrh1, &clilenh1);
  // -------- host 3: Now act as client
  servAddrh3.sin_family = AF_INET;
  servAddrh3.sin_port = htons(9567);
  if (inet_pton (AF_INET, "192.168.1.4", &servAddrh3.sin_addr) <= 0) {
    printf("Unable to open this IP");
    exit (1);
  }
  char *buf = new char[100];
  operands op;
  read(h1sock, &op, sizeof(op));
  cout << "Sending to host h3\n";
  struct in_addr tmpaddr = {cliAddrh1.sin_addr.s_addr};
  numIP toSend;
  strcpy(toSend.ip, inet_ntoa(tmpaddr));
  toSend.num = op.operands[0] - op.operands[1];
  toSend.port = op.port;
  sendto(sock2, (char *)&toSend, sizeof (toSend), 0, (struct sockaddr *)&servAddrh3, sizeof (servAddrh3));
  close(sock2);
}