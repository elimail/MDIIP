#pragma once
#include "Node.h"
#include "Route.h"
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

class Team {
public:
  vector<Node *> visited_nodes;
  Route *route;
  int worktime;
  int workdays;
  float totalcost;
  int TotalVisited;

public:
  Team();
  Team(const Team &t1);
  ~Team();

  void AddNode(Node *newnode);
  void AddCost(float cost);
  void ResetWorkitme();
  void PassDay();
  void AddVisited();
  void AddWorkTime(int work);
  void RestoreAccommodation();
  void NodetoRoute();
  void printRoute();
  void printAll();
  void printSmall();
  bool isVisited(int id);
};
