#include <bits/stdc++.h>
#include "LocalBroadcast.h"
#include "GlobalBroadcast.h"

using namespace std;


int main() {
    int R = 150;
    double p = 0.2;

    //// Local Broadcast
//    LocalBroadcastStaticTime(R, p, "Time.json");
    LocalBroadcastStaticData(R, p, "local_statistics.json");
//    auto v = GlobalBroadcast(80, 500, 100000, 15, 0.02, 0.04, 0.4, 30);
//    cout << v.run() << endl;
}