#ifndef INC_210825_STATISTICS_H
#define INC_210825_STATISTICS_H

class statistics {
    string name;
    map<int, double> val;
    map<int, int> num;

public:
    statistics(const string &name) : name(name) {}

public:

    void clear() {
        val.clear();
        num.clear();
    }

    // 增加一个统计元素
    void add(int r, double v) {
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

class statisticsHalf {
    string name;
    statistics less, great;


public:
    statisticsHalf(const string &str) : name(str), less(name + " Less"), great(name + " Great") {}

    /**
     * @param type  0 Less  1 Great
     */
    void add(int type, int r, double val) {
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
