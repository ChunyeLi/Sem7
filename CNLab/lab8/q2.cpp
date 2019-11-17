#include <bits/stdc++.h>

using namespace std;

const float eps = 1e-6;
typedef pair<double, int> ii;

int main (int argc, char *argv[]) {
  int B = atof(argv[1]);
  float R = atof(argv[2]);
  float arri, oldTS = 0;
  int curB = B;
  int id, len;
  cout << fixed << setprecision(2);
  set<ii> s;
  while (cin >> arri >> id >> len) {
    while (!s.empty() and (*s.begin()).first  < arri + eps) {
      curB += (*s.begin()).second;
      s.erase(s.begin());
    }
    if (len > curB) {
      continue; // drop this
    }
    arri = max(arri, oldTS);
    oldTS = len / R + arri;
    cout << oldTS << " " << id << " " << len << "\n";
    curB -= len;
    s.insert(ii(oldTS, len));
  }
  return 0;
}
