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

    double p_leaderElection, p_broadCast, p_global;
    vector<bool> bitmap;
    map<int, vector<int>> nxtRoundNodeID;
    int JammerID, BroadcastID, receiveNumber;
    long long JammerRound;

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
                    if (sendNum && sinrCalculate.canGetSignal(nodes[x], SendNode))
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
            map<int, int> BlockNum;

            const auto &ID_list = nxtRoundNodeID.find(gridID)->second;
            for (const auto &x : ID_list) {
                WorkNode.emplace_back(x);
                BlockNum[nodes[x].getBlockId()]++;
            }

            // 执行领导人选举
            auto rt = leaderElection(WorkNode, BlockNum);

            // 选出实际进行传信的节点
            vector<Node> Sender;
            // Jammer 始终发送信号
            if (JammerID != -1)
                Sender.emplace_back(nodes[JammerID]);

            for (const auto &x:rt) {
                if (randomGen(rand_eng) <= p_broadCast * p_global)
                    Sender.emplace_back(nodes[x]);
            }


            // 检查所有没有收到消息的节点，判断是否能成功接收到消息
            for (int i = 0; i < n; i++) {
                if (!bitmap[i]) {
                    int rcvID = sinrCalculate.Listen(nodes[i], Sender);
                    if (rcvID != -1 && (JammerID == -1 || rcvID != 0)) {
                        bitmap[i] = true;
                        receiveNumber++;
                    }
                }
            }
            RunRound++;
        }
    }

public:
    /**
     * @param BroadcastID 初始广播者 ID
     * @return 时间复杂度
     */
    long long run() {
        // 1  p工作(运行算法)  1-p静默
        // 这个部分直接加到了 领导人选举和传播里面

        // 初始化 bitmap
        bitmap.clear();
        bitmap.resize(n, false);
        for (int i = 0; i < n; i++) bitmap[i] = false;

        bitmap[BroadcastID] = true;
        receiveNumber = 1;

        long long TotRound = 0, lstReceiveNumber = receiveNumber, zeroRound = 0;

        while (true) {

            // 若没有新增的收到消息的点，记录
            if (lstReceiveNumber == receiveNumber) {
                zeroRound++;
            } else {
                lstReceiveNumber = receiveNumber;
                zeroRound = 0;
            }

            // 若连续两次都没有节点收到新的点 且 超过 n/10 的点已经收到了消息  退出
            if (zeroRound >= 3 && receiveNumber > n / 10) {
                break;
            }

            nxtRoundNodeID.clear();
            // 2  统计已收到信号的单元
            for (int i = 0; i < n; i++) {
                if (bitmap[i] && i != JammerID)
                    nxtRoundNodeID[nodes[i].getGridId()].emplace_back(i);
            }

            long long max_Round = 0;
            for (int i = 0; i < 99; i++) {
                RunRound = 0;
                broadcast(i);
                max_Round = max(max_Round, RunRound);
            }

            TotRound += max_Round * 100;
//            cerr << "TotRound  -> " << TotRound << endl;
            cerr << receiveNumber << endl;
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
                    double p_broadCase,
                    double p_global,
                    bool need_Eavesdropper = true) :
            BaseCircle(communicationRadius,
                       fieldRadius, n,
                       ceil(eps * communicationRadius / sqrt(2))),
            p_leaderElection(p_leaderElection),
            p_broadCast(p_broadCase),
            p_global(p_global) {

        if (need_Eavesdropper) {
            // 多生成一个点作为窃听者
            generateNodeWithUniform(true, 1);
            BroadcastID = 0;

            // 获取窃听者
            auto Eavesdropper = nodes.back();
            nodes.pop_back();

            // 选择阻塞点的位置
            RunRound = 0;
            JammerID = selectJammerIndex(r, p_leaderElection, Eavesdropper);
            JammerRound = RunRound;
            cerr << "Jammer " << JammerID << endl;
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
                        double grid_eps,
                        double p_leaderElection,
                        double p_broadCase,
                        const string &output_name,
                        Range<> n_Range,
                        Range<double> global_p_Range,
                        int repNum) {
        ofstream out(output_name);

        statistics withJammer("withJammer"), withoutJammer("withoutJammer");
        out << "{";
        R_for(n, n_Range) {
            cerr << " n " << n << endl;
            R_for_d(golbal_p, global_p_Range) {
                cerr << " p " << golbal_p << endl;

                int rep = repNum;


            }
        }

    }
};

#endif //INC_210825_GLOBALBROADCAST_H
