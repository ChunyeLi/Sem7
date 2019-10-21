#include <bits/stdc++.h>

using namespace std;

const float eps = 1e-5;

int main (int argc, char *argv[]) {
  float B = atof(argv[1]);
  float R = atof(argv[2]);
  float arri, curT = 0, curB = (float)B;
  float id, len;
  cout << fixed << setprecision(2);
  while (cin >> arri >> id >> len) {
    arri = max(arri, curT);
    curB = min(curB + (arri - curT) * R, (float)B);
    if ((float)len > curB) {
      curT = arri + ((float)len - curB) / R;
      curB = 0;
    } else {
      curT = arri;
      curB = curB - len;
    }
    cout << curT << " " << int(id + eps) << " " << int(len + eps) << "\n";
  }
  return 0;
}
