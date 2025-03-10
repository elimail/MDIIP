#pragma once
#include "ProblemInstance.h"
#include "Solution.h"
#include <iostream>
#include <random>
#include <vector>

using namespace std;

class Constructor {
public:
  bool construct;

public:
  explicit Constructor();
  ~Constructor();
  bool feasibleSolution(Solution *solution);
};
