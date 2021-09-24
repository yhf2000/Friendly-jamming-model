#ifndef INC_210825_NODE_H
#define INC_210825_NODE_H

// 定义状态
enum NodeState {
    Active,     // 未使用状态
    Inactive,   // 静默状态
    Send,       // 发送状态
    Receive     // 接受状态

};

class Node {
    // 点的实际坐标
    int x, y;
    // 节点状态
    NodeState state;
    // 所在网格的编号
    int gridID, blockID;
public:
    // 构造函数
    Node(int x = 0, int y = 0, int gridID = -1) :
            x(x), y(y), state(Active), gridID(gridID) {}

    int getX() const { return x; }

    int getY() const { return y; }

    int get_disFromOriSqr() const { return x * x + y * y; }

    double get_disFromOri() const { return sqrt(get_disFromOriSqr()); }

    void setState(NodeState state_prop) { state = state_prop; }

    void setGrid(int grid) { gridID = grid; }

    void setBlock(int block) { blockID = block; }

    int getGridId() const { return gridID; }

    int getBlockId() const { return blockID; }

    NodeState getState() const { return state; }

    Node operator-(const Node &node) const { return {x - node.x, y - node.y}; }
};

// 定义节点哈希运算
class NodeHash {
public:
    size_t operator()(const Node &node) const {
        return node.getX() * (1 << 20) + node.getY();
    }
};

// 定义节点相等运算
class NodeEqu {
public:
    bool operator()(const Node &node1, const Node &node2) const {
        return node1.getX() == node2.getX() &&
                node1.getY() == node2.getY();
    }
};

#endif //INC_210825_NODE_H
