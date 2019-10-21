#include <bits/stdc++.h>

using namespace std;

const float eps = 1e-4;


struct packet {
    float arrivalTime, departureTime;
    int pkID, pLen, qNo;
    packet() {}
    packet(float a, int p, int q, int l, float d) {
        arrivalTime = a; pkID = p; qNo = q; pLen = l; departureTime = d;
    }
    bool operator < (const packet &other) const {
        return departureTime > other.departureTime;
    }
//    bool operator == (const packet &other) const {
//        return (abs(arrivalTime - other.arrivalTime) < eps and pkID == other.pkID and qNo == other.qNo and pLen == other.pLen);
//    }
};

vector<packet> q[4];

int main (int argc, char *argv[]) {
    freopen("arrivals.txt", "r", stdin);
    float SR = atof(argv[1]);
    float W[4];
    for (int i = 0; i < 4; i++) {
        W[i] = atof(argv[2 + i]);
    }
    float arrivalTime;
    cout << fixed << setprecision(2);
    int pkID, qNo, pLen;
    int pcnt = 0;
    while (cin >> arrivalTime) {
        pcnt++;
        cin >> pkID >> qNo >> pLen;
        qNo--;
        q[qNo].push_back(packet(arrivalTime, pkID, qNo, pLen, -1));
    }
    priority_queue<packet> pq;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < q[i].size(); j++) {
            float finTime;
            if (j == 0) {
                finTime = q[i][j].arrivalTime + q[i][j].pLen / W[i];
            } else {
                finTime = max(q[i][j].arrivalTime, q[i][j - 1].departureTime) + q[i][j].pLen / W[i];
            }
            q[i][j].departureTime = finTime;
            pq.push(packet(q[i][j].arrivalTime, q[i][j].pkID, q[i][j].qNo, q[i][j].pLen, finTime));
        }
    }
    float curTime = 0;
    while (!pq.empty()) {
        auto at = pq.top();
        pq.pop();
        curTime = max(curTime, at.arrivalTime);
        curTime = curTime + at.pLen / SR;
        cout << curTime << " " << at.pkID << " " << at.qNo + 1 << "\n";
    }

    return 0;
}
