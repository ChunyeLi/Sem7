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

int main(int argc, char *argv[]) {
  int sock1 = socket(AF_INET, SOCK_STREAM, 0), h1sock, sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  struct sockaddr_in servAddrh2, cliAddrh1, servAddrh3;
  if (sock1 < 0 or sock2 < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  int optval = 1;
  setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  setsockopt(sock2, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  int portno = 12500;
  // -------- host 2
  servAddrh2.sin_family = AF_INET;
  servAddrh2.sin_port = htons(portno);
  servAddrh2.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(sock1, (struct sockaddr *)&servAddrh2, sizeof (servAddrh2))) {
    printf("Socket bind failed");
    exit (1);
  }
  if ((listen(sock1, 100))) { 
    printf("Listen failed...\n"); 
    exit (1); 
  }
  socklen_t clilenh1 = sizeof(cliAddrh1); 
  h1sock = accept(sock1, (struct sockaddr *)&cliAddrh1, &clilenh1);
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
  printf("Sum is %d, port is: %d\n", op.operands[0] + op.operands[1], op.port);
  cout << "Sending to host h3\n";
  struct in_addr tmpaddr = {cliAddrh1.sin_addr.s_addr};
  numIP toSend;
  strcpy(toSend.ip, inet_ntoa(tmpaddr));
  toSend.num = op.operands[0] + op.operands[1]; // so this is add.cpp, similarly can write other for mul, idiv, sub.
  toSend.port = op.port;
  sendto(sock2, (char *)&toSend, sizeof (toSend), 0, (struct sockaddr *)&servAddrh3, sizeof (servAddrh3));
  close(sock1);
  close(sock2);
  return 0;
}