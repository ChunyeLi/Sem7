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
  
  int tcpsockh3 = socket(AF_INET, SOCK_STREAM, 0); 
  if (tcpsockh3 < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  // write server side code; h3 acting as a server for h2.
  struct sockaddr_in servAddrh3, cliAddrh2; 
  bzero(&cliAddrh2, sizeof(cliAddrh2)); 
  bzero(&servAddrh3, sizeof(servAddrh3)); 
  servAddrh3.sin_addr.s_addr = htonl(INADDR_ANY); 
  servAddrh3.sin_port = htons(9567); 
  servAddrh3.sin_family = AF_INET;  
  // bind server address to socket descriptor 
  int optval = 1;
  setsockopt(tcpsockh3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
  if (bind(tcpsockh3, (struct sockaddr *) &servAddrh3, sizeof(servAddrh3))) {
    printf("Unable to bind!");
    exit (1);
  } 
  if (listen(tcpsockh3, 10)) {
    printf("Listen failde\n");
    exit (1);
  } 
  socklen_t clilen = sizeof(cliAddrh2); 
  char buffer[1000]; 
  numIP torecv;
  int newsock = accept(tcpsockh3, (struct sockaddr *)&cliAddrh2, &clilen);
  read(newsock, &torecv, sizeof (torecv));
  printf("Result obtained at h3: %d with IP: %s and port: %d\n", torecv.num, torecv.ip, torecv.port); 
  char hello[100];
  strcpy(hello, "hello from r2\n");
  printf("Size of hello is: %d\n", sizeof(hello));
  if (write(newsock, hello, sizeof (hello)) < 0) {
    printf("Unable to send back to client\n");
  }
  // writing client side code, h3 acting as a client for h1
  close(tcpsockh3);
  return 0;
}