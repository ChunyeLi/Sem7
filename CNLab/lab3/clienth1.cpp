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

char xxor (char a, char b) {
  if (a == '0' and b == '1' or (a == '1' and b == '0')) {
    return '1';
  } else return '0';
}

string hamming(string frame, int r) {
  string hammingbits = "";
  int pstart = 1;
  for (int i = 0, fc = 0; i < r + frame.size(); i++) {
    if (i + 1 == pstart) {
      hammingbits += "0";
      pstart *= 2;
    }
    else {      
      hammingbits += frame[fc++];
    }
  }
  pstart = 1;
  for (int i = 0; i < r + frame.size(); i++) {
    if (i + 1 == pstart) {
      pstart *= 2;
    } else {
      for (int k = 0; k < 31; k++) {
        if (((i + 1) & (1 << k))) {
          char nw = xxor (hammingbits[(1 << k) - 1], hammingbits[i]);
          hammingbits[(1 << k) - 1] = nw;
        }
      }
    }
  }
  return hammingbits;
}

int main(int argc, char *argv[]) {
  int sock = socket(AF_INET, SOCK_DGRAM, 0), portno = 8567, m = atoi(argv[2]);
  char *givenMessage = argv[1];
  printf("Obtained message %s, and message length %d\n", givenMessage, m);
  struct sockaddr_in servAddr;
  if (sock < 0) {
    printf("Unable to create socket\n");
    exit (1);
  }
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons(portno);
  if (inet_pton (AF_INET, "192.168.1.2", &servAddr.sin_addr) <= 0) {
    printf("Unable to open this IP");
    exit (1);
  }
  if (connect(sock, (struct sockaddr *)&servAddr, sizeof (servAddr)) < 0) {
    printf("Unable to connect!");
    exit (1);
  }
  int messageSize = strlen(givenMessage) * 8;  
  int actualMessage = m - 1;
  int frames = messageSize / actualMessage;
  int rem = messageSize % actualMessage;
  string bitRep = "";
  for (int i = 0; i < strlen(givenMessage); i++) {
    int at = givenMessage[i];
    stack<int> st;
    while (at) {
      st.push(at % 2);
      at /= 2;
    }
    string tmp = "";
    while (!st.empty()) {
      if (st.top() == 1) tmp += "1";
      else tmp += "0";
      st.pop();
    }      
    int left = 8 - tmp.size();
    string pre = "";
    while (left--) {
      pre += "0";
    }
    bitRep = bitRep + pre + tmp;
  }
  int r;
  for (r = 1; r <= 100; r++) {
    if (m + r + 1 <= (1 << r)) {
      break;
    }
  }
  cout << "Given string in binary is: " << bitRep << "\n";
  string okFrame = "1";
  char *cframe = new char[100000];
  for (int i = 0; i < frames; i++) {
    string frame = "";
    for (int k = 0; k < actualMessage; k++) {
      frame += bitRep[i * actualMessage + k];
    }
    frame += okFrame;
    cout << "Original frame of given size: " << frame << "\n";
    frame = hamming(frame, r);
    cout << "Frame after appending hamming bits: " << frame << "\n";
    strcpy (cframe, frame.c_str());
    sendto(sock, cframe, strlen(cframe), 0, (struct sockaddr *)NULL, sizeof (servAddr));
  }
  string frame = "";
  string ad = "";
  for (int i = 0; i < rem; i++) {
	  frame += bitRep[frames * actualMessage + i];
  }
  frame += "1";
  for (int i = rem + 1; i < m; i++) {
    frame += "0"; 
  }
  cout << "Original frame of given size: " << frame << "\n";
  frame = hamming(frame, r);
  cout << "Frame after appending hamming bits: " << frame << "\n";
  strcpy (cframe, frame.c_str());
  sendto(sock, cframe, strlen(cframe), 0, (struct sockaddr *)NULL, sizeof (servAddr));
  close(sock);
  return 0;
}
