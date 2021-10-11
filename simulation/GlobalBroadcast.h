#ifndef INC_210825_GLOBALBROADCAST_H
#define INC_210825_GLOBALBROADCAST_H

#include "BaseRound.h"
#include "Const.h"
#include "utils.h"
#include "statistics.h"
#include <random>
#include <algorithm>

using namespace std;

class GlobalBroadcast : public BaseCircle {

    double p_leaderElection, p_global;
    vector<bool> bitmap, notSafe;
    vector<int> unUsedNode;
    map<int, vector<int>> nxtRoundNodeID;
    set<pair<int, int>> finish; // gridID, BlockID
    map<pair<int, int>, int> leader;
    int JammerID, BroadcastID, receiveNumber;
    long long JammerRound;
    Node Eavesdropper;

    /**
     * 在一个小时间片内的广播
     * @param Sender    所有有机会发送的点
     * @param BlockNum  记录 Block 内点的个数
     * @param p         leaderElection 静默功率
     * @return 选择出来的 leader 节点
     */
    vector<int> leaderElection(const vector<int> &Sender, map<int, int> BlockNum) {
        uniform_real_distribution<double> randomGen(0, 1);
        vector<int> leaderID;
        vector<Node> SendNode;
        SINR sinrCalculate(R);


        // 初始化
        for (const auto &x:Sender) nodes[x].setState(Active);

        while (true) {
            bool bk = true;
            for (const auto &x:BlockNum) if (x.second > 1) bk = false;
            if (bk) break;

            int sendNum = 0;
            SendNode.clear();

            vector<int> sendNodeIndex;
            // 进行标记
            for (const auto &x: Sender) {
                if (nodes[x].getState() == Active) {
                    if (randomGen(rand_eng) <= p_leaderElection * p_global) {
                        SendNode.emplace_back(nodes[x]);
                        sendNum++;
                    } else nodes[x].setState(Receive);
                }
            }

            // 判断是否收到消息，且改变对应的状态
            for (const auto &x: Sender) {
                if (nodes[x].getState() == Receive) {
                    if (sendNum && sinrCalculate.Listen(nodes[x], SendNode) != -1)
                        nodes[x].setState(Inactive), BlockNum[nodes[x].getBlockId()] -= 1;
                }
            }

            // 设置没有收到消息的节点为初始状态
            for (const auto &x: Sender) {
                if (nodes[x].getState() == Receive) {
                    nodes[x].setState(Active);
                }
            }

            // 统计当前计算轮数
            RunRound++;
        }

        for (const auto &x:Sender) {
            if (nodes[x].getState() == Active) {
                leaderID.emplace_back(x);
                leader[{nodes[x].getGridId(), nodes[x].getBlockId()}] = x;
            }
        }
        return leaderID;
    }


    // 执行一个小时间片的操作
    void broadcast(int gridID) {
        SINR sinrCalculate(R);
        uniform_real_distribution<double> randomGen(0, 1);

        if (nxtRoundNodeID.count(gridID)) {
            // 统计出当前回合会进行发信的点
            vector<int> WorkNode;
            set<int> saveLeader;
            map<int, int> BlockNum;

            const auto &ID_list = nxtRoundNodeID.find(gridID)->second;
            for (const auto &x : ID_list) {
                const auto &nd = nodes[x];
                if (leader.count({nd.getGridId(), nd.getBlockId()})) {
                    saveLeader.emplace(leader[{nd.getGridId(), nd.getBlockId()}]);
                } else {
                    WorkNode.emplace_back(x);
                    BlockNum[nd.getBlockId()]++;
                }
            }

            // 执行领导人选举
            auto rt = leaderElection(WorkNode, BlockNum);

            // 选出实际进行传信的节点
            vector<Node> Sender;
            // Jammer 始终发送信号
            if (JammerID != -1)
                Sender.emplace_back(nodes[JammerID]);
            for (const auto &x:saveLeader) rt.emplace_back(x);
            for (const auto &x:rt) {
                const auto &nd = nodes[x];
                if (randomGen(rand_eng) <= p_global) {
                    Sender.emplace_back(nd);
                    finish.emplace(nd.getGridId(), nd.getBlockId());
                    // 确定当前点要执行通信之后，这个区块不再进行传输
                }
            }


            // 检查所有没有收到消息的节点，判断是否能成功接收到消息
            vector<int> ns, nw;
            for (auto x:unUsedNode) {
                if (!bitmap[x]) {
                    int rcvID = sinrCalculate.Listen(nodes[x], Sender);
                    if (rcvID != -1 && (JammerID == -1 || rcvID != 0)) {
                        bitmap[x] = true, ns.emplace_back(x);
                        if (!notSafe[x]) receiveNumber++;
                    }
                }
            }
            nw.resize(unUsedNode.size());
            nw.resize(set_difference(unUsedNode.begin(), unUsedNode.end(),
                                     ns.begin(), ns.end(), nw.begin()) - nw.begin());
            swap(unUsedNode, nw);
            RunRound++;
        }
    }

public:
    /**
     * @param BroadcastID 初始广播者 ID
     * @return 时间复杂度
     */
    long long run(double r) {
        // 1  p工作(运行算法)  1-p静默
        // 这个部分直接加到了 领导人选举和传播里面

        // 初始化 bitmap
        bitmap.clear(), notSafe.clear(), unUsedNode.clear();
        bitmap.resize(n, false), notSafe.resize(n, false);
        receiveNumber = 0;

        // 把 safe zone 以内的点标记
        SINR sinr(R);
        double safeZoneR = get_SafeZoneR(sinr, r);

        if (JammerID != -1) {
            for (int i = 0; i < n; i++) {
                if ((nodes[JammerID] - nodes[i]).get_disFromOri() <= safeZoneR) {
                    receiveNumber += 1;
                    notSafe[i] = true;
                }
            }
        }
        // 广播起始点不在 SafeZone 内
        if (notSafe[BroadcastID]) return -1;

        bitmap[BroadcastID] = true;
        receiveNumber += 1;

        for (int i = 0; i < n; i ++) if (!bitmap[i]) unUsedNode.emplace_back(i);

        long long TotRound = 0;

        while (receiveNumber < n) {
            nxtRoundNodeID.clear();
            // 2  统计已收到信号的单元
            for (int i = 0; i < n; i++) {
                if (bitmap[i]
                    && i != JammerID
                    && !finish.count({nodes[i].getGridId(), nodes[i].getBlockId()})) {
                    nxtRoundNodeID[nodes[i].getGridId()].emplace_back(i);
                }
            }

            long long max_Round = 0;
            for (int i = 0; i < C * C; i++) {
                RunRound = 0;
                broadcast(i);
                max_Round = max(max_Round, RunRound);
            }

            TotRound += max_Round * C * C;
//            cerr << "TotRound  -> " << TotRound << endl;
//            cerr << receiveNumber << endl;
        }
        return TotRound + JammerRound;
    }

public:

    /**
     *
     * @param communicationRadius 通信半径
     * @param fieldRadius 场地半径
     * @param n 节点数
     * @param grid_eps grid = grid_eps * R / sqrt(2)
     * @param p_leaderElection 领导人选举静默概率
     * @param p_broadCase 广播静默概率
     * @param p_global 全局静默概率
     * @param r jammer election 选择半径
     */
    GlobalBroadcast(int communicationRadius,
                    int fieldRadius,
                    int r,
                    int n,
                    double p_leaderElection,
                    double p_global,
                    int C,
                    bool need_Eavesdropper = true) :
            BaseCircle(communicationRadius,
                       fieldRadius, n,
                       ceil(eps * communicationRadius / (2 * sqrt(2))), C),
            p_leaderElection(p_leaderElection),
            p_global(p_global) {

        finish.clear();
        leader.clear();

        if (need_Eavesdropper) {
            // 多生成一个点作为窃听者
            generateNodeWithUniform(true, 1);
            BroadcastID = 0;

            // 获取窃听者
            Eavesdropper = nodes.back();
            nodes.pop_back();

            // 选择阻塞点的位置
            RunRound = 0;
            JammerID = selectJammerIndex(r, p_leaderElection, Eavesdropper);
            JammerRound = RunRound;
//            cerr << "Jammer " << JammerID << endl;
        } else {
            generateNodeWithUniform(true);
            BroadcastID = 0;
            JammerID = -1;
            JammerRound = 0;
        }

    }

};

class GlobalBroadcastTime {
public:
    GlobalBroadcastTime(int communicationRadius,
                        int fieldRadius,
                        int r,
                        double p_leaderElection,
                        const string &output_name,
                        Range<> n_Range,
                        Range<double> global_p_Range,
                        Range<> C_Range,
                        int repNum) {

        R_for(C, C_Range) {
            cerr << " C =" << C << endl;
            ofstream out(output_name + "_with_C=" + to_string(C) + ".json");
            statistics<double, double> withJammer("with"), without("without");

            out << "{";
            R_for(n, n_Range) {
                cerr << " n " << n << endl;
                R_for_d(p_global, global_p_Range) {
                    cerr << " p " << p_global << endl;
                    ThreadPool pool(maxThread);
                    vector<future<long long>> res;
                    for (int rep = 0; rep < repNum; rep++) {
                        res.emplace_back(pool.enqueue([&] {
                            GlobalBroadcast bg(communicationRadius,
                                               fieldRadius,
                                               r, n,
                                               p_leaderElection,
                                               p_global,
                                               C,
                                               true);
                            return bg.run(r);
                        }));
                    }
                    int step = 0;
                    for (auto &re : res) {
                        cerr << "\r" << "[" << (++step) << "/" << repNum << "]";
                        auto rt = re.get();
                        if (rt != -1)
                            withJammer.add(p_global, rt);
                    }
                    cerr << endl;

//                    for (int rep = 0; rep < repNum; rep++) {
//                        GlobalBroadcast bg(communicationRadius,
//                                           fieldRadius,
//                                           r, n,
//                                           p_leaderElection,
//                                           p_global,
//                                           C,
//                                           true);
//                        auto rt = bg.run(r);
//                        if (rt != -1)
//                            withJammer.add(p_global, rt);
//
//                    }
                }
                cerr << " without Jammer " << endl;
                ThreadPool pool(maxThread);
                vector<future<long long>> res;
                for (int rep = 0; rep < repNum; rep++) {
                    res.emplace_back(pool.enqueue([&] {
                        GlobalBroadcast bg(communicationRadius,
                                           fieldRadius,
                                           r, n,
                                           p_leaderElection,
                                           1,
                                           C,
                                           false);
                        return bg.run(r);
                    }));
                }
                int step = 0;
                for (auto &re : res) {
                    auto rt = re.get();
                    cerr << "\r" << "[" << (++step) << "/" << repNum << "]";
                    if (rt != -1)
                        without.add(1, rt);
                }
                cerr << endl;

                out << "\"" << n << "\":[\n";
                withJammer.print(out);
                out << ",";
                without.print(out);
                out << "]";
                if (n + n_Range.getStep() <= n_Range.getMax()) out << ",\n";
                withJammer.clear();
            }
            out << "}";
        }
    }
};

#endif //INC_210825_GLOBALBROADCAST_H
