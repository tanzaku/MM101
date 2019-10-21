// C++11
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <set>
#include <string>

using namespace std;

class WorldCupLineup {
public:
    vector<string> selectPositions(vector<string> players, vector<string> groups) {
        return {"F 0", "F 1", "F 2", "M 3", "M 4", "M 5", "M 6", "D 7", "D 8", "D 9"};
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v) {
    for (int i = 0; i < v.size(); ++i)
        getline(cin, v[i]);
}

int main() {
    WorldCupLineup sol;
    int H;
    cin >> H;
    vector<string> players(H);
    getVector(players);
    cin >> H;
    vector<string> groups(H);
    getVector(groups);

    vector<string> ret = sol.selectPositions(players, groups);
    cout << ret.size() << endl;
    for (int i = 0; i < (int)ret.size(); ++i)
        cout << ret[i] << endl;
    cout.flush();
}
