#pragma once
#include "Node.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

class Route {
public:
  vector<Node *> rute_nodes;
  float routecost;
  vector<int> daytime;
  vector<float> daycost;

public:
  explicit Route();
  Route(vector<Node *> nodes);
  Route(const Route &r1);
  ~Route();

  void AddNode(Node *newnode);
  void setTotalRouteCost();
  void setTotalRouteCost(float routecost);
  void setRouteCost();
  void setRouteCost(int day, float daycost);
  void setRouteTime();
  void setRouteTime(int day, int daytime);
  float getRouteCost();
  float getRouteCost(int day);
  float getSubRouteCost(int first, int last);
  void getRouteTime(vector<int> &routetimes);
  int getRouteTime(int day);
  int getSubRouteTime(int firs, int last);
  int getRouteDays();
  int countnodes();
  int getNodeDay(int index);
  bool timefeasible(int maxtime);
  void printRoute();
  void printAll();
};
