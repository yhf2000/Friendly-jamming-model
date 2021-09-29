#ifndef SIMULATION_UTILS_H
#define SIMULATION_UTILS_H

#define R_for(i, R) for(int i = R.getMin(); i <= R.getMax(); i += R.getStep())
#define R_for_d(i, R) for(double i = R.getMin(); i <= R.getMax(); i += R.getStep())

template <class T=int>
class Range {
    T Min, Max, Step;

public:
    T getMin() const {
        return Min;
    }

    T getMax() const {
        return Max;
    }

    T getStep() const {
        return Step;
    }


    Range(T Min, T Max, T Step) :
            Min(Min), Max(Max), Step(Step) {}
};

#endif //SIMULATION_UTILS_H
