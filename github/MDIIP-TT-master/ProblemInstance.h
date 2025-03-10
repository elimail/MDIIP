#pragma once
#include "Accommodation.h"
#include "Node.h"
#include <iostream>
#include <vector>

using namespace std;

class ProblemInstance {
public:
  vector<int> accommodation_nodes;
  vector<int> bridge_nodes;
  vector<int> depot_nodes;

  float **distances_bxb;
  float **distances_bxa;
  float **distances_axb;
  float **distances_dxb;
  float **distances_bxd;

  int n_depots;
  int n_bridges;
  int n_accommodations;
  int days;
  int worktime;
  float dispatching_cost;

  vector<Node *> Dnodes;
  vector<Node *> Bnodes;
  vector<Node *> Anodes;

  vector<Accommodation *> Accommodations;

public:
  explicit ProblemInstance();
  ~ProblemInstance();

  int getNumberOfDepotNodes();
  int getNumberOfBridgeNodes();
  int getNumberOfAccommodationNodes();

  int getNumberOfAccommodations();

  void setTotalDistances();

  void setDepots();

  void setTotalDepots(int n_depots);
  void setTotalBridges(int n_bridges);
  void setTotalAccommodations(int n_accommodations);

  int getTotalDepots();
  int getTotalBridges();
  int getTotalAccommodations();

  void setworktime(int worktime);
  void setdispatching_cost(float dispatchingcost);
  void setDays(int days);

  int getworktime();
  float getdispatching_cost();
  int getDays();

  void addDepotNode(Node *node);
  void addBridgeNode(Node *node);
  void addAccommodationNode(Node *node);

  void addAccomodation(Accommodation *Accommodation);

  void setBxBDistance();
  void setBxDDistance();
  void setBxADistance();
  void setDistance(int m, int row, int column, float distance);

  double CalculateDistance(double x1, double x2, double y1, double y2);

  float getDistance(Node *initialNode, Node *finalNode); // calculateDistance
  int getTime(Node *initialNode, Node *finalNode); 
  float getCost(Node *initialNode, Node *finalNode, float distance_cost);

  void sortNodes();

  void printAll();
};
