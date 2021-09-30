#include <bits/stdc++.h>
#include "LocalBroadcast.h"
#include "GlobalBroadcast.h"
#include "utils.h"

using namespace std;


int main() {
    int R = 50;
    double p = 0.6;

    //// Local Broadcast
//    LocalBroadcastStaticTime(R, p, "Time.json");
//    LocalBroadcastStaticData(R, p,
//                             "local_statistics.json",
//                             Range<>(5000, 10000, 500),
//                             Range<>(3, 10, 1),
//                             10
//                             );
    auto v = GlobalBroadcast(50,
                             300,
                             10,
                             10000,
                             0.6,
                             0.6,
                             0.4);
    cout << v.run() << endl;
}