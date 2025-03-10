#pragma once
#include "ProblemInstance.h"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>

using namespace std;

class Reader {
private:
  string filePath;
  ifstream input;

public:
  explicit Reader(string option);
  ~Reader();

  ProblemInstance *readInputFile();

private:
  void findDef(string def);
  void CountDepots(ProblemInstance *problemInstance);
  void CountBridges(ProblemInstance *problemInstance);
  void CountAccommodations(ProblemInstance *problemInstance);

  void readWorkTime(ProblemInstance *problemInstance);
  void readDays(ProblemInstance *problemInstance);
  void readDispatchingCost(ProblemInstance *problemInstance);
  void readAccomodations(ProblemInstance *problemInstance);
  void readNodes(ProblemInstance *problemInstance);
  void readBxBDistances(ProblemInstance *problemInstance);
  void readBxADistances(ProblemInstance *problemInstance);
  void readAxBDistances(ProblemInstance *problemInstance);
  void readDxBDistances(ProblemInstance *problemInstance);
  void readBxDDistances(ProblemInstance *problemInstance);
  void readDistances(ProblemInstance *problemInstance, int Mtype, int row,
                     int columns);
};
