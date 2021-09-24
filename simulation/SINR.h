#ifndef INC_210825_SINR_H
#define INC_210825_SINR_H

#include "Const.h"
#include "node.h"
#include <vector>

using namespace std;

// SINR 类
class SINR {
    // a, b, 功率，环境噪音，通信半径，out interference
    double Alpha, Beta, P, N, R, p;
public:

    double get_Alpha() const { return Alpha; }

    double get_Beta() const { return Beta; }

    double get_p() const { return p; }

    SINR(double R, double P = -1, double p = JammerThreshold, double Alpha = 3, double Beta = 1.5, double N = 1) :
            Alpha(Alpha), Beta(Beta), P(P), N(N), R(R), p(p) {
        if (this->P == -1) {
            this->P = pow((1 + eps) * R, Alpha) * Beta * N;
        }
    }

    /**
     *
     * @param Receiver
     * @param Sender
     * @return 当前的信号强度之和
     */
    double SignalStrength(const Node &Receiver, vector<Node> &Sender) const {
        double res = 0;
        for (const auto x:Sender)
            res += P / pow((Receiver - x).get_disFromOri(), Alpha);
        return res;
    }

    bool canGetSignal(const Node &Receiver, vector<Node> &Sender) const {
        return SignalStrength(Receiver, Sender) >= N * (1 + p);
    }

    /**
     *
     * @param Receiver
     * @param Sender
     * @return 返回满足不等式的节点的 index
     */
    int Listen(const Node &Receiver, vector<Node> &Sender) const {
        // 分母
        double all = N + SignalStrength(Receiver, Sender);

        // 满足不等式的数量，与 index 的标号
        int num = 0, pos = -1;
        for (int i = 0; i < Sender.size(); i++) {
            const auto &x = Sender[i];

            // 单个的强度
            double strength = (P / pow((x - Receiver).get_disFromOri(), Alpha));
            if (strength / (all - strength) >= Beta) {
                pos = i;
                num++;
            }
        }
        if (num == 1) return pos;
        else return -1;
    }
};

#endif //INC_210825_SINR_H
