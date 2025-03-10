#pragma once
#include <iostream>

using namespace std;

class Node {
private:
  int id;
  double x_cord;
  double y_cord;
  int bridge_wt;
  int node_id;
  int type;
  float distance;
  float cost;
  int time;

public:
  explicit Node(int id, double x_cord, double y_cord, int bridge_wt, int node_id, int type);
  Node(const Node &n1);
  ~Node();

  int getId() const;
  double getXCoord() const;
  double getYCoord() const;
  int getBridge_wt() const;
  int getNodeId() const;
  int getNodeType() const;
  float getCost() const;
  float getDistance() const;
  int getTime() const;

  void setDistance(float distance);
  void setCost(float cost);
  void setTime(int time);

  void printAll();
  void printBase();

  bool operator==(const Node &node) const {
    return this->getId() == node.getId();
  }
  bool operator!=(const Node &node) const { return !operator==(node); }
};

bool sortByWorkTime(const Node *n1, const Node *n2);
bool sortByType(const Node *n1, const Node *n2);
bool sortById(const Node *n1, const Node *n2);
bool sortByCost(const Node *n1, const Node *n2);
