#include <bits/stdc++.h>

using namespace std;

int V;
typedef pair<int, int> ii; 
string filename;
const int inf = 10001;
vector<map<int, ii> > dv;
vector<vector<ii> > glist;

int main () {
    cin >> V;
    cin >> filename; 
    freopen(filename.c_str(), "r", stdin);
    glist.resize(V + 1);
    int a, b, c;
    while (cin >> a >> b >> c) {
        cout << a << " " << b << " " << c << "\n";
        glist[a].push_back(make_pair(b, c));
    }
    dv.resize(V + 1);
    for (int i = 1; i <= V; i++) {
        dv[i][i] = ii(0, i);
        for (auto &v : glist[i]) {
            if (dv[i].count(v.first) == 0 or dv[i][v.first].first > v.second) {
                dv[i][v.first] = ii(v.second, v.first);
            }
        }
    }
    bool cont = true;
    while (cont) {
        cont = false;
        for (int i = 1; i <= V; i++) {
            random_shuffle(glist[i].begin(), glist[i].end());
            for (auto &to : glist[i]) {
                for (auto &dvto : dv[to.first]) {
                    if (dv[i].count(dvto.first) == 0 or dv[i][dvto.first].first > to.second + dvto.second.first) {
                        dv[i][dvto.first] = ii(to.second + dvto.second.first, to.first);
                        cont = true;
                    }
                }
            }
        }
    }
    for (int i = 1; i <= V; i++) {
        cout << "Printing table for " << i << ":\n";
        for (auto &d : dv[i]) {
            cout << "to " << d.first << ": ";
            d.second.first == inf ? cout << "inf" : cout << d.second.first;
            d.second.first == 0 ? cout << " -\n" : cout << " " << d.second.second << "\n";
        }
    }
    return 0;
}