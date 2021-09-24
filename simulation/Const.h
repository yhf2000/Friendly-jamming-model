#ifndef INC_210825_CONST_H
#define INC_210825_CONST_H

#include <random>

std::default_random_engine rand_eng(time(nullptr));

const double JammerThreshold = 0.1;
const double eps = 1;
const int maxThread = 56;

#endif //INC_210825_CONST_H
