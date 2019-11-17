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

void errorExit(char *err) {
  printf("%s\n", err);
  exit (1);
}

int main(int argc, char *argv[]) {
  int h2ServersSock[4], optval = 1, ports[] = {12500, 12501, 12502, 12503};
  struct sockaddr_in h2ServersAddr[4];
  for (int i = 0; i < 4; i++) {
    h2ServersSock[i] = socket(AF_INET, SOCK_STREAM, 0);
    if (h2ServersSock[i] < 0) {
      errorExit("Unable to create socket\n");
    }
    setsockopt(h2ServersSock[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const void *)&optval, sizeof(int));
    h2ServersAddr[i].sin_family = AF_INET;
    h2ServersAddr[i].sin_port = htons(ports[i]);
    h2ServersAddr[i].sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(h2ServersSock[i], (struct sockaddr *)&h2ServersAddr[i], sizeof (h2ServersAddr[i]))) {
      errorExit("Socket bind failed");
    }
    if ((listen(h2ServersSock[i], 4))) { 
      errorExit("Listen failed\n");
    }
  }
  
  while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    int maxfd = -1;
    for (int i = 0; i < 4; i++) {
      FD_SET(h2ServersSock[i], &readfds);
      maxfd = max(maxfd, h2ServersSock[i]);
    }
    int status = select(maxfd + 1, &readfds, NULL, NULL, NULL);
    if (status < 0) {
      errorExit("Status invalidated");
    }
    int currentSock = -1;
    for (int i = 0; i < 4; i++) {
      if (FD_ISSET(h2ServersSock[i], &readfds)) {
        currentSock = i;
        break;
      }
    }
    if (currentSock == -1) {
      continue;
    }
    // FD_CLR(h2ServersSock[currentSock], &readfds);
    cout << "Current sock: " << currentSock << "\n";
    if (fork() != 0) {
      sleep(1);
      continue;
    }
    for (int i = 0; i < 4; i++) {
      if (i == currentSock) continue;
      close(h2ServersSock[i]);
    }
    if (currentSock == 0) {
      char str[100];
      sprintf(str, "%d", h2ServersSock[currentSock]);
      char *args[] = {"./add", str, NULL};
      printf("sending fd = %s\n", args[1]);
      execvp("./add", args);
    } else if (currentSock == 1) {
      char str[100];
      sprintf(str, "%d", h2ServersSock[currentSock]);
      char *args[] = {"./sub", str, NULL};
      printf("sending fd = %s\n", args[1]);
      execvp("./sub", args);
    } else if (currentSock == 2) {
      char str[100];
      sprintf(str, "%d", h2ServersSock[currentSock]);
      char *args[] = {"./mul", str, NULL};
      printf("sending fd = %s\n", args[1]);
      execvp("./mul", args);
    } else {
      char str[100];
      sprintf(str, "%d", h2ServersSock[currentSock]);
      char *args[] = {"./idiv", str, NULL};
      printf("sending fd = %s\n", args[1]);
      execvp("./idiv", args);
    }
    break;  // will not be executed
  }
  for (int i = 0; i < 4; i++) {
    if (h2ServersSock[i] > 0) 
      close(h2ServersSock[i]);
  }
  return 0;
}