#ifndef INC_210825_STATISTICS_H
#define INC_210825_STATISTICS_H
template<class Key, class Value>
class statistics {
    string name;
    map<Key, Value> val;
    map<Key, int> num;

public:
    statistics(const string &name) : name(name) {}

public:

    void clear() {
        val.clear();
        num.clear();
    }

    // 增加一个统计元素
    void add(Key r, Value v) {
        val[r] += v, num[r] += 1;
    }

    // 输出字典
    ofstream &print(ofstream &out) {
        out << R"(  {"name":")" << name << R"(","data": [)" << endl;
        bool flag = false;
        for (auto &x:val) {
            if (flag) out << ",\n";
            flag = true;
            out << "      {" << "\"r\":" << x.first << ",";
            out << "\"val\":" << x.second / (1.0 * num[x.first]) << "}";
        }
        out << "]}";
        return out;
    }
};

template<class Key, class Value>
class statisticsPair {
    string name;
    statistics<Key, Value> less, great;


public:
    statisticsPair(const string &str,
                   string First = "Less",
                   string Second = "Great")
            : name(str), less(name + " " + First), great(name + " " + Second) {}

    /**
     * @param type  0 First  1 Second
     */
    void add(int type, Key r, Value val) {
        if (type == 0) less.add(r, val);
        else if (type == 1) great.add(r, val);
    }

    void clear() {
        less.clear(), great.clear();
    }

    ofstream &print(ofstream &out) {
        less.print(out);
        out << ",\n";
        great.print(out);
        return out;
    }

};

#endif //INC_210825_STATISTICS_H
