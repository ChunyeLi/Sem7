#include <bits/stdc++.h>

using namespace std;

const float eps = 1e-6;
typedef pair<double, int> ii;

void shape(int B, float R) {
  // freopen("arrivals.txt", "r", stdin);
  // freopen("arrivals_shape.txt", "w", stdout);
  ofstream o("arrivals_shape.txt");
  ifstream f("arrivals.txt");
  float arri, curT = 0;
  int curB = B;
  int id, len;
  cout << fixed << setprecision(2);
  while (f >> arri >> id >> len) {
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
    o << curT << " " << id << " " << len << "\n";
  }
}


bool fifo(int B, float R, float x) {
  shape(500, x);
  // freopen("arrivals_shape.txt", "r", stdin);
  ifstream f("arrivals_shape.txt");
  float arri, oldTS;
  int curB = B;
  int id, len;
  cout << fixed << setprecision(2);
  set<ii> s;
  bool init = true;
  bool ok = false;
  while (f >> arri >> id >> len) {
    ok = true;
    if (init) {
      if (len > B) {
        return false;
      }
      init = false;
      oldTS = len / R + arri;
      // cout << oldTS << " " << id << " " << len << "\n";
      curB -= len;
      s.insert(ii(oldTS, len));
      continue;
    }
    if (len > B) {
      return false;
    }
    while (!s.empty() and (*s.begin()).first  < arri + eps) {
      curB += (*s.begin()).second;
      s.erase(s.begin());
    }
    if (s.empty()) {
      arri = max(arri, oldTS);
      oldTS = len / R + arri;
      // cout << oldTS << " " << id << " " << len << "\n";
      curB -= len;
      s.insert(ii(oldTS, len));
      continue;
    }
    if (len > curB) {
      return false; // drop this
    }
    arri = max(arri, oldTS);
    oldTS = len / R + arri;
    // cout << oldTS << " " << id << " " << len << "\n";
    curB -= len;
    s.insert(ii(oldTS, len));
  }
  return true;
  return ok;
  // return true;
} 

int main () {
  float low = 0, high = 100;
  int count = 300;
  while (count--) {
    float mid = (low + high) / 2;
    if (fifo(1000, 10.0, mid)) {
      low = mid;
    } else {
      high = mid;
    }
  }
  cout << fixed << setprecision(6) << low << " " << high << " " << (low + high) / 2 << "\n";
  return 0;
}
