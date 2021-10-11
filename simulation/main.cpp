#include <bits/stdc++.h>
#include "LocalBroadcast.h"
#include "GlobalBroadcast.h"
#include "utils.h"

using namespace std;


int main() {
    int R = 50;
    double p = 0.6;

    //// Local Broadcast
    cout << maxThread << endl;
//    LocalBroadcastStaticTime(R, p, "Time.json");
//    LocalBroadcastStaticData(R, p,
//                             "local_statistics.json",
//                             Range<>(5000, 10000, 500),
//                             Range<>(3, 10, 1),
//                             200
//                             );
//    auto v = GlobalBroadcast(50,
//                             300,
//                             10,
//                             10000,
//                             0.6,
//                             0.6,
//                             0.4);
//    cout << v.run() << endl;
    GlobalBroadcastTime(50,
                        300,
                        10,
                        0.6,
                        "n3-GlobalTime",
                        Range<>(5000, 10000, 200),
                        Range<double>(0.4, 0.81, 0.05),
                        Range<>(10, 10, 1),
                        2000);
}