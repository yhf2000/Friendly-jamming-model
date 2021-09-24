#ifndef INC_210825_GLOBALBROADCAST_H
#define INC_210825_GLOBALBROADCAST_H

#include "BaseRound.h"

class GlobalBroadcast : public BaseCircle{

    void broadcast(){

    }

    void run(){
        // 1  p工作(运行算法)  1-p静默

        // 2  选择所有存在信号的单元

        // 3  0-99 循环，所有编号为 0 的一起运行 leader 选举

        // 4 所有的 leader p 的概率发信， 1-p 的概率静默
    }

public:

    GlobalBroadcast(){}

};

#endif //INC_210825_GLOBALBROADCAST_H
