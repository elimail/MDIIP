#pragma once
#include "Node.h"
#include "ProblemInstance.h"
#include "Route.h"
#include "Team.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <random>
#include <vector>
#include <set>

using namespace std;

class Solution {
public:
  vector<Team *> Teams;
  vector<Node *> Unvisited_bridges;
  vector<Node *> available_visit;

  ProblemInstance *problemInstance;

  int totalcost;
  int minvisit;
  int maxvisit;
  int speed;
  int distance_cost;

public:
  explicit Solution(ProblemInstance *problemInstance);
  ~Solution();
  Solution(const Solution &s2);
  void reset(const Solution &s2);
  void check();
  void alwaysCheck();
  void printSummary();

  int random_int_number(int min, int max);
  void setTotalCost();
  float getTotalCost();
  bool removeBridge(Node *node);
  void getAvailableBridges(Team *team);
  void selectBridge(Team *team);
  void selectAccommodation(Team *team);
  void selectAccommodation(Route *route);
  void returnToDepot(Team *team);
  void returnToDepot(Route *route);
  bool isAccommodation(Route *route, int index, int size);
  bool isDepot(Route *route, int index, int size);
  void printAll();
  void printSmall();
  void changeRoutes2opt();
  void twoOpt(Route *actual_route, int initial_node, int final_node, int day_count);
  void twoOptSwap(Route *route, int i, int k);
  void changeDistance(Node *i, Node *j);
  void changeRoutesNodeSwap();
  void nodeSwap(Route *route1, int swap1, int day1, Route *route2, int swap2, int day2);
  void changeRoutesRouteSwap(int lenght);
  bool routeSwap(Route *route1, int swap1, int day1, Route *route2, int swap2, int day2, int length);
  void changeRoutesNodeDay();
  bool changeRoutesNodeDayIteration();
  bool changeRoutesDayIteration();
  void changeRoutesNodeNewDay();
  void changeRoutesNewDay();
  bool addToDay(Route *origin_route, int origin_swap, int origin_day, Route *destiny_route, int destiny_swap, int destiny_day);
  bool addToDay(Route *origin_route, int origin_swap, int origin_day, Route *destiny_route, int destiny_swap, int destiny_day, int size);
  bool addToNewDay(Route *origin_route, int swap, int origin_day, Route *destiny_route);
  bool addToNewDay(Route *origin_route, int swap, int origin_day, Route *destiny_route, int size);
  void deleteNode(Route *origin_route, int swap, int origin_day);
  void deleteNode(Route *origin_route, int swap, int origin_day, int size);
  void deleteAccommodation(Route *origin_route, int swap);
  void changeRoutesAccommodation();
  void changeAccommodation(Route *route, int swap, int day);
  void changeSolution();
  void VNSAll(int max_iter);
  void DisturbAll(int max_iter);
  void VNSOneMove();

  void DisturbRoutes2opt();
  void DisturbtwoOpt(Route *actual_route, int initial_node, int final_node, int day_count);
  void DisturbRoutesRouteSwap();
  bool DisturbrouteSwap(Route *route1, int swap1, int day1, Route *route2, int swap2, int day2, int length);
  void DisturbRoutesDayIteration();
  bool DisturbaddToDay(Route *origin_route, int origin_swap, int origin_day, Route *destiny_route, int destiny_swap, int destiny_day, int size);
  void DisturbRoutesNewDay();
  bool DisturbaddToNewDay(Route *origin_route, int swap, int origin_day, Route *destiny_route, int size);
  void DisturbRoutesAccommodation();
  bool DisturbAccommodation(Route *route, int swap, int day);
};
