#include <bits/stdc++.h>

using namespace std;


int main (int argc, char *argv[]) {
  int B = atof(argv[1]);
  float R = atof(argv[2]);
  float arri, curT = 0;
  int curB = B;
  int id, len;
  cout << fixed << setprecision(2);
  while (cin >> arri >> id >> len) {
    arri = max(arri, curT);
    curB = curB + (arri - curT) * R;
    curB = min(curB, B);
    // curB = min(curB + (arri - curT) * R, B);
    if (len > curB) {
      curT = arri + (len - curB) / R;
      curB = 0;
    } else {
      curT = arri;
      curB = curB - len;
    }
    cout << curT << " " << id << " " << len << "\n";
  }
  return 0;
}
