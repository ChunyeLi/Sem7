#include <bits/stdc++.h>

using namespace std; 

const float eps = 1e-4;


struct packet {
  float arrivalTime;
  int pkID, pLen, qNo;
  packet() {}
  packet(float a, int p, int q, int l) {
    arrivalTime = a; pkID = p; qNo = q; pLen = l;
  }
  bool operator < (const packet &other) const {
    return arrivalTime < other.arrivalTime;
  }
  bool operator == (const packet &other) const {
    return (abs(arrivalTime - other.arrivalTime) < eps and pkID == other.pkID and qNo == other.qNo and pLen == other.pLen);
  }
};

queue<packet> q[4];
multiset<packet> s;

int main (int argc, char *argv[]) {
  float SR = atof(argv[1]);
  float arrivalTime;
  cout << fixed << setprecision(2);
  int pkID, qNo, pLen;
  while (cin >> arrivalTime) {
    cin >> pkID >> qNo >> pLen;
    qNo--;
    s.insert(packet(arrivalTime, pkID, qNo, pLen));
    q[qNo].push(packet(arrivalTime, pkID, qNo, pLen));
  }
  float curTime = 0;
  while (!s.empty()) {
    packet startFrom = *s.begin();
    curTime = max(curTime, startFrom.arrivalTime);
    bool ok = true;
    int i = startFrom.qNo;
    while (ok) {
      int cnt = 0; 
      int four = 4;
      while (four--) {
        int qNo = i % 4;
        i++;
        if (!q[qNo].empty()) {
          packet pkt = q[qNo].front();
          if (pkt.arrivalTime > curTime + eps) {
            cnt++;
            continue;
          }
          q[qNo].pop();
          s.erase(s.find(pkt));
          curTime = curTime + (pkt.pLen / SR);
          cout << curTime << " ";
          cout << pkt.pkID << "\n";
        } else {
          cnt++;
        }
      }
      if (cnt == 4) {
        ok = false;
      }
    }
  }

  return 0;
}
