#include "Accommodation.h"

Accommodation::Accommodation(int id, int cost, int nodeID)
    : id(id), cost(cost), nodeID(nodeID) {}

Accommodation::~Accommodation() = default;

int Accommodation::getId() const { return this->id; }

int Accommodation::getCost() const { return this->cost; }

int Accommodation::getNodeID() const { return this->nodeID; }

void Accommodation::printAll() {
  cout << "Accommodation: " << this->getId() << " Cost: " << this->getCost()
       << " NodeID: " << this->getNodeID() << endl;
}