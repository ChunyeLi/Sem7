// server h3
#include <stdio.h> 
#include <strings.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <stack>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

char xxor (char a, char b) {
  if (a == '0' and b == '1' or (a == '1' and b == '0')) {
		return '1';
	} else {
		return '0'; 
	}
}

char getChar(string charRep) {
	int at = 0;
	for (int i = 0; i < charRep.size(); i++) {
		at = at * 2 + (charRep[i] == '1');
	}
	return char(at);
}

string process (string given) {
	string bitRep = "";
	string toret = "";
	for (int i = 0, k = 1; i < given.size(); i++, k++) {
		bitRep += given[i];
		if (k == 8) {
			k = 0;
			toret += getChar(bitRep);
			bitRep = "";
		}
	}
  return toret;
}

int main() { 
  int sock = socket(AF_INET, SOCK_DGRAM, 0), port = 9567;
  if (sock < 0) { 
    printf("Unable to create socket\n");
  }

  char buffer[256]; 
  struct sockaddr_in servaddr, cliaddr; 

  bzero(&servaddr, sizeof(servaddr)); 

  servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
  servaddr.sin_port = htons(port); 
  servaddr.sin_family = AF_INET;  

  // bind server address to socket descriptor 
  if (bind(sock, (struct sockaddr *) &servaddr, sizeof(servaddr))) {
    printf("Unable to bind!");
    exit (1);
  } 
  socklen_t clilen = sizeof(cliaddr); 
  int n;
  string totalMsg = "";
  bool done = false;
  while (1) {
    n = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *) &cliaddr, &clilen); 
    buffer[n] = '\0';
    printf("Here is the recieved error message: %s\n", buffer);
    int pstart = 1;
    for (int i = 0; i < strlen(buffer); i++) {
      if (i + 1 == pstart) {
        pstart *= 2;
      } else {
        for (int k = 0; k < 31; k ++) {
          if (((i + 1) & (1 << k))) {
            char nw = xxor (buffer[(1 << k) - 1], buffer[i]);
            buffer[(1 << k) - 1] = nw;
          }
        }
      }
    }
    pstart = 1;
    int bitDiffAt = 0;
    for (int i = 0; i < strlen(buffer); i++) {
      if (i + 1 == pstart) {
        pstart *= 2;
        if (buffer[i] != '0') {
          bitDiffAt += (i + 1);
        }
      }
    }
    if (buffer[bitDiffAt - 1] == '1') {
      buffer[bitDiffAt - 1] = '0';
    } else {
      buffer[bitDiffAt - 1] = '1';
    }
    pstart = 1;
    string actm = "";
    for (int i = 0; i < strlen(buffer); i++) {
      if (i + 1 == pstart) {
        pstart *= 2;
      } else {
        string som(1, buffer[i]);
        actm += som;
      }
    }
    cout << "Here is the recieved correct message: " << actm << "\n";
    string realActm = "";
    string tmpr = actm;
    if (actm[actm.size() - 1] == '0') { // end frame
      done = true;
    }
    reverse(tmpr.begin(), tmpr.end());
    int i;
    for (i = 0; i < tmpr.size(); i++) {
      if (tmpr[i] == '1') {
        i++;
        break;
      }
    }
    for (; i < tmpr.size(); i++) {
      realActm += tmpr[i];
    }
    reverse(realActm.begin(), realActm.end());
    cout << "Actual " << realActm << "\n";
    totalMsg += realActm;
    if (done) {
      cout << process(totalMsg) << "\n";
      break; 
    }
  }
  return 0;
} 

