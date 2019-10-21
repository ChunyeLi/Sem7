#include <bits/stdc++.h>

using namespace std;

const float eps = 1e-6;
typedef pair<double, int> ii;

int main (int argc, char *argv[]) {
  int B = atof(argv[1]);
  float R = atof(argv[2]);
  float arri, oldTS;
  int curB = B;
  int id, len;
  cout << fixed << setprecision(2);
  set<ii> s;
  bool init = true;
  while (cin >> arri >> id >> len) {
    if (init) {
      if (len > B) {
        continue;
      }
      init = false;
      oldTS = len / R + arri;
      cout << oldTS << " " << id << " " << len << "\n";
      curB -= len;
      s.insert(ii(oldTS, len));
      continue;
    }
    if (len > B) {
      continue;
    }
    while (!s.empty() and (*s.begin()).first  < arri + eps) {
      curB += (*s.begin()).second;
      s.erase(s.begin());
    }
    if (s.empty()) {
      arri = max(arri, oldTS);
      oldTS = len / R + arri;
      cout << oldTS << " " << id << " " << len << "\n";
      curB -= len;
      s.insert(ii(oldTS, len));
      continue;
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
