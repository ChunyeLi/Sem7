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

struct ans {
  int ans;
};

int h1serverport = 8567;

int main(int argc, char *argv[]) {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  int int1 = atoi(argv[1]), int2 = atoi(argv[2]), op = atoi(argv[3]);
  struct sockaddr_in servAddrh2;
  if (sock < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  int portno;
  if (op == 0) {
      portno = 12500; // ADD
  } else if (op == 1) {
    portno = 12501; // SUB
  } else if (op == 2) {
    portno = 12502; // MUL
  } else {
    portno = 12503; // IDIV
  }
  servAddrh2.sin_family = AF_INET;
  servAddrh2.sin_port = htons(portno);
  if (inet_pton (AF_INET, "192.168.1.3", &servAddrh2.sin_addr) <= 0) {
    printf("Unable to open this IP");
    exit (1);
  }
  int optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  if (connect(sock, (struct sockaddr *)&servAddrh2, sizeof (servAddrh2)) < 0) {
    printf("Unable to connect!");
    exit (1);
  }
  char *buf = new char[10000];
  operands ops;
  ops.operands[0] = int1; ops.operands[1] = int2;
  ops.port = h1serverport;
  write(sock, (char *)&ops, sizeof (ops));
  // Now reading part from h3, thus acting now as server.
  int sockh3 = socket(AF_INET, SOCK_STREAM, 0);
  if (sockh3 < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  struct sockaddr_in servAddrh1, cliAddrh3;
  servAddrh1.sin_family = AF_INET;
  servAddrh1.sin_port = htons(h1serverport);
  servAddrh1.sin_addr.s_addr = htonl(INADDR_ANY);
  setsockopt(sockh3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  if (bind(sockh3, (struct sockaddr *)&servAddrh1, sizeof (servAddrh1))) {
    printf("Socket bind failed");
    exit (1);
  }
  if ((listen(sockh3, 100))) { 
    printf("Listen failed...\n"); 
    exit (1); 
  }
  socklen_t clilenh3 = sizeof(cliAddrh3); 
  int h3sock = accept(sockh3, (struct sockaddr *)&cliAddrh3, &clilenh3);
  ans toRecv;
  read(h3sock, &toRecv, sizeof(toRecv));
  printf("Answer obtained from h3: %d\n", toRecv.ans);
  close(sockh3);
  close(sock);
  return 0;
}