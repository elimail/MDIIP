#pragma once
#include <iostream>

using namespace std;

class Accommodation {
private:
  int id;
  int cost;
  int nodeID;

public:
  Accommodation(int id, int cost, int nodeID);
  ~Accommodation();

  int getId() const;
  int getCost() const;
  int getNodeID() const;

  void printAll();

  bool operator==(const Accommodation &accommodation) const {
    return this->getId() == accommodation.getId();
  }
  bool operator!=(const Accommodation &accommodation) const {
    return !operator==(accommodation);
  }
  bool operator>(const Accommodation &accommodation) const {
    return this->getCost() > accommodation.getCost();
  }
  bool operator<(const Accommodation &accommodation) const {
    return !operator>(accommodation);
  }
};
