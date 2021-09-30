#ifndef INC_210825_CONST_H
#define INC_210825_CONST_H

#include <random>
#include <thread>

std::default_random_engine rand_eng(time(nullptr));

const double JammerThreshold = 0.1;
const double eps = 1;
const int maxThread = std::thread::hardware_concurrency();

#endif //INC_210825_CONST_H
