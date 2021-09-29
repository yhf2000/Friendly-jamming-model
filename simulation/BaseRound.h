#ifndef INC_210825_BASEROUND_H
#define INC_210825_BASEROUND_H

#include "node.h"

// 基础场地类
class BaseCircle {
protected:
    // 大圆半径，节点数量
    int CircleR, n, gridLength;
    // 通信半径
    double R;
    // 节点向量
    vector<Node> nodes;
    // 轮数统计
    long long RunRound;

    int BlockSize;

    // 获取所在的位置
    int getGrid(const Node &node) const {
        if (gridLength == -1) return -1;
        int xv = ((node.getX() + CircleR) / gridLength) % 10;
        int yv = ((node.getY() + CircleR) / gridLength) % 10;
        return xv * 10 + yv;
    }

    int getBlock(const Node &node) const{
        if(gridLength == -1) return -1;
        int xv = ((node.getX() + CircleR) / gridLength) / 10;
        int yv = ((node.getY() + CircleR) / gridLength) / 10;
        int num = (CircleR * 2 + gridLength - 1) / gridLength; // 一行有多少块
        return xv * num + yv;
    }

    // 判断点是否在圆内
    bool isInCircle(const Node &node) const {
        return node.get_disFromOriSqr() <= CircleR * CircleR;
    }

    // 获取 safeZone 的半径
    double get_SafeZoneR(SINR &Sp, double r) const {
        return ((1 + eps) * R * pow(Sp.getBeta(), 1.0 / Sp.getAlpha()) /
                pow(pow(1 + eps, Sp.getAlpha()) - Sp.get_p() - 1, 1 / Sp.getAlpha()))
               + r;
    }

    // 是否在 SafeZone 内部
    bool isInSafeZoneAndCom(const Node &Listener, const Node &Sender, SINR &SINR_prop, double r) const {
        return
            // 在 SafeZone 大范围之内
                Listener.get_disFromOri() >= get_SafeZoneR(SINR_prop, r) &&
                // 在 Sender 的通信范围内
                (Listener - Sender).get_disFromOri() <= R;
    }

    // R 内的节点数量
    bool isInCommunicationRadius(const Node &Listener, const Node &Sender) const {
        return (Listener - Sender).get_disFromOri() <= R;
    }

    // 所有在三个区域内的点数
    bool isInThreeZone(const Node &Listener, const Node &Sender, SINR &SINR_prop, double r) const {
        // 在 SafeZone 中
        return isInSafeZoneAndCom(Listener, Sender, SINR_prop, r)
               // 或者在 DangerZone 或 GreyZone 中
               || ((Listener.get_disFromOri() <= get_SafeZoneR(SINR_prop, r)));
    }

public:

    /**
     * @param CommunicationRadius 通讯半径
     * @param FieldRadius 场地半径
     * @param n 节点个数
     */
    BaseCircle(int CommunicationRadius, int FieldRadius, int n, int gridLength = -1) :
            CircleR(FieldRadius), n(n), RunRound(0),
            R(CommunicationRadius), gridLength(gridLength) {
        BlockSize = (CircleR * 2 + gridLength - 1) / gridLength;
        BlockSize = BlockSize * BlockSize;
    }

    void generateNodeWithUniform(bool global = false, int moreNode=0) {
        // 生成时判断是否在圆内
        unordered_set<Node, NodeHash, NodeEqu> exist;

        // 定义随机数生成器
        uniform_int_distribution<int> randomGen(-CircleR, CircleR);



        while(nodes.size() != n + moreNode){
            // 清空原有节点向量
            nodes.clear();

            if(global) {
                nodes.emplace_back(0, 0);
                nodes.back().setGrid(getGrid(nodes.back()));
                nodes.back().setBlock(getBlock(nodes.back()));
            }
            // 随机生成点
            int failed = 0;
            // 连续 100 次重复，说明过于稠密
            while (nodes.size() < n + moreNode && failed < 200) {
                Node newNode(randomGen(rand_eng), randomGen(rand_eng));
                // 判断是否在圆内，判断是否在曾经存在
                if (isInCircle(newNode) && !exist.count(newNode) && newNode.getX() != 0 && newNode.getY() != 0) {
                    newNode.setGrid(getGrid(newNode));
                    newNode.setBlock(getBlock(newNode));
                    exist.insert(newNode), nodes.push_back(newNode);
                    failed = 0;
                } else failed++;
            }
        }

        // 检查是否成功生成 n 个点
        // 断言失败说明 n 对于当前的 CircleR 过大
        assert(nodes.size() == n + moreNode);
//        nodes.resize(n);
    }

    /**
     * @param p 每轮 p 的概率 Send， 1-p 的概率接受
     * @param r jammer 的选取半径
     * @return 返回选中的 jammer 的 index
     */
    int selectJammerIndex(int r, double p, Node Eavesdropper = Node(0, 0)) {
        // 定义随机数生成器
        uniform_real_distribution<double> randomGen(0, 1);
        // 定义 SINR 计算类
//        SINR sinrCalculate(R);

        // 算法运行中剩余的点
        int nodeLeft = 0;
        // 获取 r 以内的点
        vector<int> in_r_index;
        for (int i = 0; i < n; i++)
            if ((nodes[i] - Eavesdropper).get_disFromOriSqr() <= r * r)
                in_r_index.emplace_back(i), nodeLeft++;
        // 表示 r 的范围内没有点
        if (nodeLeft == 0) return -1;

        vector<Node> SendNode;

        // 节点初始化
        for (const auto &x:in_r_index) nodes[x].setState(Active);
        // 算法迭代
        while (nodeLeft > 1) {
            // 一轮迭代
            int sendNum = 0;
            SendNode.clear();

            vector<int> sendNodeIndex;

            // 进行标记
            for (const auto &x: in_r_index) {
                if (nodes[x].getState() == Active) {
                    if (randomGen(rand_eng) <= p) {
                        SendNode.emplace_back(nodes[x]);
                        sendNum++;
                    } else nodes[x].setState(Receive);
                }
            }

            // 判断是否收到消息，且改变对应的状态
            for (const auto &x: in_r_index) {
                if (nodes[x].getState() == Receive) {
                    if (sendNum)
                        // && sinrCalculate.canGetSignal(nodes[x], SendNode)
                        nodes[x].setState(Inactive), nodeLeft--;
                }
            }

            // 设置没有收到消息的节点为初始状态
            for (const auto &x: in_r_index) {
                if (nodes[x].getState() == Receive) {
                    nodes[x].setState(Active);
                }
            }

            // 统计当前计算轮数
            RunRound++;
        }
        // 获取剩余的最后一个点
        int resIndex = -1;
        for (const auto &x:in_r_index) {
            if (nodes[x].getState() == Active) {
                if (resIndex != -1) assert(false); // 断言不存在两个合法的点
                else resIndex = x;
            }
        }
        // 断言剩余最后一定有一个点
        assert(resIndex != -1);

//        cerr << "111\n";


        return resIndex;
//        return 0;
    }

//    int getRound() const { return RunRound; }
};

#endif //INC_210825_BASEROUND_H
