#include "Node.h"

Node::Node(int id, double x_cord, double y_cord, int bridge_wt, int node_id, int type) : id(id), x_cord(x_cord), y_cord(y_cord), bridge_wt(bridge_wt), node_id(node_id), type(type) {
  this->distance = 0;
  this->cost = 0;
  this->time = 0;
}

Node::Node(const Node &n1) {
  id = n1.id;
  x_cord = n1.x_cord;
  y_cord = n1.y_cord;
  bridge_wt = n1.bridge_wt;
  node_id = n1.node_id;
  type = n1.type;

  distance = n1.distance;
  cost = n1.cost;
  time = n1.time;
}

Node::~Node() = default;

int Node::getId() const { return this->id; }

double Node::getXCoord() const { return this->x_cord; }

double Node::getYCoord() const { return this->y_cord; }

int Node::getBridge_wt() const { return this->bridge_wt; }

int Node::getNodeId() const { return this->node_id; }

int Node::getNodeType() const { return this->type; }

void Node::setDistance(float n_distance) { this->distance = n_distance; }

float Node::getDistance() const { return this->distance; }

void Node::setCost(float n_cost) { this->cost = n_cost; }

float Node::getCost() const { return this->cost; }

int Node::getTime() const { return this->time; }

void Node::setTime(int n_time) { this->time = n_time; }

void Node::printAll() {
  cout << "Node:  " << this->getId();
  cout << "\tNodeID: " << this->getNodeId();
  cout << "\tX: " << fixed << this->getXCoord();
  cout << "\tY: " << fixed << this->getYCoord();
  cout << "\tP: " << this->getBridge_wt();
  cout << "\tType: " << this->getNodeType();
  cout << "\tDistance: " << this->getDistance();
  cout << "\tCost: " << this->getCost();
  cout << "\tTime: " << this->getTime() << endl;
}

// void Node::printAll() {
//   cout << "Node:  " << this->getId() << endl;
//   cout << "\tNodeID: " << this->getNodeId()<< endl;
//   cout << "\tX: " << this->getXCoord()<< endl;
//   cout << "\tY: " << this->getYCoord()<< endl;
//   cout << "\tP: " << this->getBridge_wt()<< endl;
//   cout << "\tType: " << this->getNodeType()<< endl;
//   cout << "\tDistance: " << this->getDistance()<< endl;
//   cout << "\tCost: " << this->getCost()<< endl;
//   cout << "\tTime: " << this->getTime() << endl;
// }

void Node::printBase() {
  cout << "Node:  " << this->getId() << "\tNodeID: " << this->getNodeId()
       << "\tX: " << fixed << this->getXCoord() << "\tY: " << fixed
       << this->getYCoord() << "\tP: " << this->getBridge_wt()
       << "\tType: " << this->getNodeType() << endl;
}

bool sortByWorkTime(const Node *n1, const Node *n2) {
  return n1->getBridge_wt() < n2->getBridge_wt();
}
bool sortByType(const Node *n1, const Node *n2) {
  return n1->getNodeType() < n2->getNodeType();
}
bool sortById(const Node *n1, const Node *n2) {
  return n1->getId() < n2->getId();
}
bool sortByCost(const Node *n1, const Node *n2) {
  return n1->getCost() < n2->getCost();
}
