#pragma once
#include "ProblemInstance.h"
#include "Solution.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

class Iterator {
public:
  bool construct;

public:
  explicit Iterator();
  ~Iterator();
  void IterateSolution(Solution *solution, int iterations, int vns_iterations,
                       int disturb_iterations);
};