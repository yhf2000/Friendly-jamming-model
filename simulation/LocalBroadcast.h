#ifndef INC_210825_LOCALBROADCAST_H
#define INC_210825_LOCALBROADCAST_H

#include "SINR.h"
#include "BaseRound.h"
#include "statistics.h"
#include "utils.h"
#include "ThreadPool.h"
#include "Const.h"

class LocalBroadcast : public BaseCircle {

    // 曾经可以通信的位置
    unordered_set<int> SafeNodeIndex;

    // 返回可以到达点的数量，如果窃听者可以到达返回 0
    // SafeZone 中收到消息的数量，Three Zone 中的节点数，SafeZone 中的节点数, 在 R 内的节点数量, R 内能收到消息的节点数量
    tuple<int, int, int, int, int> run(int JammerIndex, int BroadcastIndex) {

        const auto &B = nodes[BroadcastIndex], &J = nodes[JammerIndex];
        // 定义 SINR 计算类
        SINR sinrCalculate(R);

//        cerr << get_SafeZoneR(sinrCalculate, J.get_disFromOri()) << " " << R << endl;

        //                        0  1
        vector<Node> SendNodes = {J, B};

        // 窃听者的坐标
        Node A(0, 0);

        // 窃听者可以听到消息
        if (sinrCalculate.Listen(A, SendNodes) == 1) return {0, 0, 0, 0, 0};

        int SafeZoneNum = 0, ThreeZoneNum = 0;
        int ReceiveNumInSafeZone = 0, ReceiveNumInR = 0;
        int CommunicationRadiusNumber = 0;
        for (int i = 0; i < n; i++) {
            if (i != JammerIndex && i != BroadcastIndex) {
                if (sinrCalculate.Listen(nodes[i], SendNodes) == 1) {
//                    SafeNodeIndex.insert(i);
                    if (isInSafeZoneAndCom(nodes[i], B, sinrCalculate, J.get_disFromOri()))
                        ReceiveNumInSafeZone++;
                    if (isInCommunicationRadius(nodes[i], B)) ReceiveNumInR++;
                }
                if (isInSafeZoneAndCom(nodes[i], B, sinrCalculate, J.get_disFromOri())) SafeZoneNum++;
                if (isInThreeZone(nodes[i], B, sinrCalculate, J.get_disFromOri())) ThreeZoneNum++;
                if (isInCommunicationRadius(nodes[i], B)) CommunicationRadiusNumber++;
            }
        }
//        cerr << " +++ "<< ReceiveNumInSafeZone << " " << ThreeZoneNum << " " << SafeZoneNum << endl;
        return {ReceiveNumInSafeZone, ThreeZoneNum, SafeZoneNum, CommunicationRadiusNumber, ReceiveNumInR};
    }


public:
    LocalBroadcast(int CommunicationRadius, int FieldRadius, int n) : BaseCircle(CommunicationRadius, FieldRadius, n) {
        generateNodeWithUniform();
    }

    // 返回 <real_r, 轮数,
    // 覆盖率[r1~(r1+CircleR)/2], 成功率[r1~(r1+CircleR)/2],
    // 覆盖率[(r1+CircleR)/2~CircleR], 成功率[(r1+CircleR)/2]~CircleR>
    tuple<int, int, double, double, double, double, double, double> runWith_r(int r, double p, int repeat = 1) {
        RunRound = 0;
        int JammerIndex = selectJammerIndex(r, p);
        if (JammerIndex == -1) return {-1, -1, -1, -1, -1, -1, -1, -1};

        uniform_int_distribution<int> randomGen(0, n - 1);
        SINR sinr(R);
        // Broadcast 距离 Jammer 的距离，接受数
        // 并行
//        vector<future<pair<double, tuple<int, int, int, int, int>>>> res;
//        ThreadPool pool(maxThread);
//        for(int rep = 0; rep < repeat; rep ++){
//            res.emplace_back(pool.enqueue([&]{
//                int BroadcastIndex = randomGen(rand_eng);
//                while (BroadcastIndex == JammerIndex
//                       || nodes[BroadcastIndex].get_disFromOri() > R
//                       || nodes[BroadcastIndex].get_disFromOri() <
//                          sinr.getMinBroadcasterDis(nodes[JammerIndex].get_disFromOri()))
//                    BroadcastIndex = randomGen(rand_eng);
//                return make_pair(nodes[BroadcastIndex].get_disFromOri(), run(JammerIndex, BroadcastIndex));
//            }));
//        }
        // 串行
        vector<pair<double, tuple<int, int, int, int, int>>> res;
        for (int rep = 0; rep < repeat; rep++) {
            int BroadcastIndex = randomGen(rand_eng);
            while (BroadcastIndex == JammerIndex
                   || nodes[BroadcastIndex].get_disFromOri() > R
                   || nodes[BroadcastIndex].get_disFromOri() <
                      sinr.getMinBroadcasterDis(nodes[JammerIndex].get_disFromOri()))
                BroadcastIndex = randomGen(rand_eng);
            res.emplace_back(make_pair(nodes[BroadcastIndex].get_disFromOri(), run(JammerIndex, BroadcastIndex)));
        }

        // 计算 r1
        double r1 = R, hf;
        for (auto &&re: res) {
            auto x = re;
            if (get<0>(x.second) != 0) r1 = min(r1, x.first);
        }
        hf = (r1 + R) / 2;


        // 统计数据
        double ReceiveNumSum_a_cg = 0;
        double ReceiveNumSum_b_cg = 0;
        double ReceiveNumSum_a_fg = 0;
        double ReceiveNumSum_b_fg = 0;
        double ReceiveNumSum_a_sx = 0;
        double ReceiveNumSum_b_sx = 0;

        int ReceiveNum_a = 0, ReceiveNum_b = 0;

        // 0,                      1,                   2                  3               4
        // SafeZone 中收到消息的数量，Three Zone 中的节点数，SafeZone 中的节点数，在 R 内的节点数量, R 内能收到消息的节点数量
        for (auto &re: res) {
            auto x = re;
            if (r1 <= x.first && x.first <= hf) {
                if (get<0>(x.second) != 0) {
                    ReceiveNumSum_a_cg += 1.0 * get<0>(x.second) / get<2>(x.second);
                    ReceiveNumSum_a_fg += 1.0 * get<0>(x.second) / get<3>(x.second);
                    ReceiveNumSum_a_sx += 1.0 * get<4>(x.second) / get<3>(x.second);
                }
                ReceiveNum_a++;
            } else if (hf < x.first && x.first <= CircleR) {
                if (get<0>(x.second) != 0) {
                    ReceiveNumSum_b_cg += 1.0 * get<0>(x.second) / get<2>(x.second);
                    ReceiveNumSum_b_fg += 1.0 * get<0>(x.second) / get<3>(x.second);
                    ReceiveNumSum_b_sx += 1.0 * get<4>(x.second) / get<3>(x.second);
                }
                ReceiveNum_b++;
            }
        }

        return {
                nodes[JammerIndex].get_disFromOriSqr(),
                RunRound + 1,
                1.0 * ReceiveNumSum_a_fg / ReceiveNum_a,
                1.0 * ReceiveNumSum_a_cg / ReceiveNum_a,
                1.0 * ReceiveNumSum_a_sx / ReceiveNum_a,
                1.0 * ReceiveNumSum_b_fg / ReceiveNum_b,
                1.0 * ReceiveNumSum_b_cg / ReceiveNum_b,
                1.0 * ReceiveNumSum_b_sx / ReceiveNum_b,
        };
    }

    // 返回 <real_r, 轮数>
    tuple<int, int> runWith_r_one(int r, double p) {
        RunRound = 0;
        int JammerIndex = selectJammerIndex(r, p);
        if (JammerIndex == -1) return {-1, -1};
        return {
                nodes[JammerIndex].get_disFromOriSqr(),
                RunRound + 1,
        };
    }

};


class LocalBroadcastStaticTime {

public:
    /**
     * @param R 通信半径
     * @param p 通信概率
     */
    LocalBroadcastStaticTime(int R, double p, const string &output_name, Range<> n_Range, Range<> r_Range, int repNum) {
        ofstream out(output_name);

        // 时间复杂度，当前部分与 Broadcast，Jammer 之间的距离无关
        statistics<int, double> localTime("Time");
        ThreadPool pool(maxThread);

        out << "{";

        R_for (n, n_Range) {
            cerr << " n " << n << endl;
            R_for (r, r_Range) {
                cerr << " r " << r << endl;
                vector<future<tuple<int, int>>> res;
                for (int rep = 0; rep < repNum; rep++) {
                    res.emplace_back(pool.enqueue(
                            [&] {
                                LocalBroadcast d(R, R * 2, n);
                                return d.runWith_r_one(r, p);
                            }
                    ));
                }
                for (auto &&re : res) {
                    auto rt = re.get();
                    if (get<0>(rt) != -1) {
                        localTime.add(r, get<1>(rt));
                    }
                }
            }
            out << "\"" << n << "\":[\n";
            localTime.print(out);
            localTime.clear();
            out << "]";
            if (n + n_Range.getStep() <= n_Range.getMax()) out << ",\n";

        }

        out << "}";
    }

};


class LocalBroadcastStaticData {

    static void run(int R, double p, int n, int r, int i) {

    }

public:
    /**
     * @param R 通信半径
     * @param p 通信概率
     */
    LocalBroadcastStaticData(int R, double p, const string &output_name, Range<> n_Range, Range<> r_Range,
                             int repNum) {
        ofstream out(output_name);

        statisticsPair <int, double> cover("Cover"), success("Success"), receive("Receive");
        out << "{";
        R_for (n, n_Range) {
            cerr << " n " << n << endl;
            R_for(r, r_Range) {
                cerr << " r " << r << endl;

                vector<future<tuple<int, int, double, double, double, double, double, double>>> res;
                ThreadPool pool(maxThread);

                for (int rep = 0; rep < repNum; rep++) {
                    res.emplace_back(pool.enqueue([&] {
                        LocalBroadcast d(R, R * 2, n);
                        return d.runWith_r(r, p, 100);
                    }));
                }

                for (auto &&rs:res) {
                    auto rt = rs.get();
                    if (get<0>(rt) != -1) {
                        int rn = r;
                        cover.add(0, rn, get<2>(rt));
                        cover.add(1, rn, get<5>(rt));
                        success.add(0, rn, get<3>(rt));
                        success.add(1, rn, get<6>(rt));
                        receive.add(0, rn, get<4>(rt));
                        receive.add(1, rn, get<7>(rt));
                    }
                }

            }
            out << "\"" << n << "\":[\n";
            cover.print(out), out << ",\n";
            success.print(out), out << ",\n";
            receive.print(out);
            out << "]";
            if (n + n_Range.getStep() <= n_Range.getMax()) out << ",\n";

            cover.clear(), success.clear(), receive.clear();
        }
        out << "}";
    }
};

#endif //INC_210825_LOCALBROADCAST_H
