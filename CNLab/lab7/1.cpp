#include <bits/stdc++.h>

using namespace std;

int V, s;
string filename;
const int inf = 10001;
vector<vector<pair<int, int> > > glist;
vector<int> p;

struct node {
    int v, c;
    node () {}
    node (int vv, int cc) {
        v = vv; c = cc;
    }
    bool operator < (const node &other) const {
        return c > other.c;
    }
};

void recur(int at, vector<int> &pth) {
    if (at == s) {
        pth.push_back(at);
    } else {
        recur(p[at], pth);
        pth.push_back(at);
    }
}


void getPath(int to) {
    vector<int> pth;
    recur(to, pth);
    for (auto &pthe : pth)  {
        cout << pthe << " ";
    }
}

int main () {
    cin >> V >> s >> filename;
    freopen(filename.c_str(), "r", stdin);
    int a, b, c;
    glist.resize(V + 1);
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            cin >> a >> b >> c;
            // cout << a << " " << b << " " << c << "\n";
            glist[a].push_back(make_pair(b, c));
        }
    }
    priority_queue<node> pq;
    // cout << s << "\n";
    pq.push(node(s, 0));
    vector<int> dist(V + 1, inf);
    dist[s] = 0;
    p.assign(V + 1, -1);
    p[s] = s;
    while (!pq.empty()) {
        int at = pq.top().v;
        
        int atc = pq.top().c;
        // cout << " HI " << at << " " << atc << "\n";
        pq.pop();
        if (dist[at] != atc) {
            continue;
        }
        // cout << "hi";
        for (auto &to : glist[at]) {
            if (to.second == 10001) {
                continue;
            }
            if (dist[to.first] > dist[at] + to.second) {
                dist[to.first] = dist[at] + to.second;
                p[to.first] = at;
                pq.push(node(to.first, dist[to.first]));
            }
        }
    }
    for (int i = 1; i <= V; i++) {
        cout << "Distance to " << i << " is: ";
        if (dist[i] == inf) {
            cout << "inf\n";
        } else {
            cout << dist[i] << " and path is: "; getPath(i); cout << "\n";
        }
    }
    return 0;
}