#ifndef SIMULATION_UTILS_H
#define SIMULATION_UTILS_H

#define R_for(i, R) for(int i = R.getMin(); i <= R.getMax(); i += R.getStep())

class Range {
    int Min, Max, Step;

public:
    int getMin() const {
        return Min;
    }

    int getMax() const {
        return Max;
    }

    int getStep() const {
        return Step;
    }


    Range(int Min, int Max, int Step) :
            Min(Min), Max(Max), Step(Step) {}
};

#endif //SIMULATION_UTILS_H
