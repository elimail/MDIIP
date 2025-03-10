#include "ProblemInstance.h"
#include <algorithm>
#include <math.h>

ProblemInstance::ProblemInstance() {}

ProblemInstance::~ProblemInstance() {
  // cout << "Deleting Problem Instance" << endl;

  for (int i = 0; i < this->getNumberOfBridgeNodes(); ++i) {
    delete[] this->distances_bxb[i];
  }
  delete[] distances_bxb;

  this->accommodation_nodes.clear();
  this->accommodation_nodes.shrink_to_fit();

  this->depot_nodes.clear();
  this->depot_nodes.shrink_to_fit();

  this->bridge_nodes.clear();
  this->bridge_nodes.shrink_to_fit();

  for (Accommodation *Accommodation : this->Accommodations) {
    delete Accommodation;
  }
  this->Accommodations.clear();
  this->Accommodations.shrink_to_fit();

  for (Node *node : this->Dnodes) {
    delete node;
  }
  this->Dnodes.clear();
  this->Dnodes.shrink_to_fit();

  for (Node *node : this->Bnodes) {
    delete node;
  }
  this->Bnodes.clear();
  this->Bnodes.shrink_to_fit();

  for (Node *node : this->Anodes) {
    delete node;
  }
  this->Anodes.clear();
  this->Anodes.shrink_to_fit();
}

int ProblemInstance::getNumberOfDepotNodes() {
  return (int)this->Dnodes.size();
}
int ProblemInstance::getNumberOfBridgeNodes() {
  return (int)this->Bnodes.size();
}
int ProblemInstance::getNumberOfAccommodationNodes() {
  return (int)this->Anodes.size();
}

void ProblemInstance::setTotalDepots(int totaldepots) {
  this->n_depots = totaldepots;
}
void ProblemInstance::setTotalBridges(int totalbridges) {
  this->n_bridges = totalbridges;
}
void ProblemInstance::setTotalAccommodations(int totalaccommodations) {
  this->n_accommodations = totalaccommodations;
}

void ProblemInstance::setTotalDistances() {
  this->distances_bxb = new float *[this->n_bridges];
  for (int i = 0; i < this->n_bridges; ++i) {
    this->distances_bxb[i] = new float[this->n_bridges];
  }
  this->distances_bxa = new float *[this->n_bridges];
  for (int i = 0; i < this->n_bridges; ++i) {
    this->distances_bxa[i] = new float[this->n_accommodations];
  }
  this->distances_axb = new float *[this->n_accommodations];
  for (int i = 0; i < this->n_accommodations; ++i) {
    this->distances_axb[i] = new float[this->n_bridges];
  }
  this->distances_dxb = new float *[n_depots];
  for (int i = 0; i < n_depots; ++i) {
    this->distances_dxb[i] = new float[this->n_bridges];
  }
  this->distances_bxd = new float *[this->n_bridges];
  for (int i = 0; i < this->n_bridges; ++i) {
    this->distances_bxd[i] = new float[n_depots];
  }
}

double ProblemInstance::CalculateDistance(double x1, double x2, double y1, double y2) {

  double x = x1 - x2;
  double y = y1 - y2;
  double dist;

  dist = pow(x, 2) + pow(y, 2);
  dist = sqrt(dist);

  return dist;
}

int ProblemInstance::getTotalDepots() { return this->n_depots; }
int ProblemInstance::getTotalBridges() { return this->n_bridges; }
int ProblemInstance::getTotalAccommodations() { return this->n_accommodations; }

void ProblemInstance::setworktime(int i_worktime) {
  this->worktime = i_worktime;
}
void ProblemInstance::setdispatching_cost(float dispatchingc) {
  this->dispatching_cost = dispatchingc;
}
void ProblemInstance::setDays(int t_days) { this->days = t_days; }

int ProblemInstance::getworktime() { return this->worktime; }
float ProblemInstance::getdispatching_cost() { return this->dispatching_cost; }
int ProblemInstance::getDays() { return this->days; }

void ProblemInstance::addDepotNode(Node *node) { this->Dnodes.push_back(node); }
void ProblemInstance::addBridgeNode(Node *node) {
  this->Bnodes.push_back(node);
}
void ProblemInstance::addAccommodationNode(Node *node) {
  this->Anodes.push_back(node);
}

void ProblemInstance::addAccomodation(Accommodation *accommodation) {
  this->Accommodations.push_back(accommodation);
}

void ProblemInstance::setDistance(int matrix, int row, int col,
                                  float distance) {

  switch (matrix) {
  case 0:
    this->distances_bxb[row][col] = distance;
    break;
  case 1:
    this->distances_bxa[row][col] = distance;
    break;
  case 2:
    this->distances_axb[row][col] = distance;
    break;
  case 3:
    this->distances_dxb[row][col] = distance;
    break;
  case 4:
    this->distances_bxd[row][col] = distance;
    break;
  }
}

float ProblemInstance::getDistance(Node *initialNode, Node *finalNode) {
    //int row_ID = i->getNodeId();
    //int col_ID = j->getNodeId();
    bool debug = false;
    
    if (debug) cout << " En getDistance -- Calculando distancia entre " << initialNode->getId() << " y " << finalNode->getId() << endl; 
    if (initialNode->getNodeType() == 1 && finalNode->getNodeType() == 1){
      if (debug) cout << "puente a puente" << endl;
      return this->distances_bxb[initialNode->getNodeId()][finalNode->getNodeId()];
    }
    
    if (initialNode->getNodeType() == 0 && finalNode->getNodeType() == 1){
      if (debug) cout << "deposito a puente" << endl;
      return this->distances_dxb[initialNode->getNodeId()][finalNode->getNodeId()];
    }
    
    if (initialNode->getNodeType() == 1 && finalNode->getNodeType() == 0){
      if (debug) cout << "puente a deposito" << endl;
      return this->distances_bxd[initialNode->getNodeId()][finalNode->getNodeId()];
    }
    
    if (initialNode->getNodeType() == 2 && finalNode->getNodeType() == 1){
      if (debug) cout << "hotel a puente" << endl;     
      return this->distances_axb[initialNode->getNodeId()][finalNode->getNodeId()];
    }
    
    if (initialNode->getNodeType() == 1 && finalNode->getNodeType() == 2){
      if (debug) cout << "puente a hotel" << endl;     
      return this->distances_bxa[initialNode->getNodeId()][finalNode->getNodeId()];
    }
    return -1;
}

int  ProblemInstance::getTime(Node *initialNode, Node *finalNode) {
    bool debug = false;
    
    if (debug) cout << " En getTime -- Calculando tiempo entre " << initialNode->getId() << " y " << finalNode->getId() << endl; 
    return roundf(getDistance(initialNode, finalNode)) + finalNode->getBridge_wt(); // Round con 0 decimales
}

float ProblemInstance::getCost(Node *initialNode, Node *finalNode, float distance_cost) {
    bool debug = false;
    float cost;
    
    if (debug) cout << " En getCost -- Calculando costo entre " << initialNode->getId() << " y " << finalNode->getId() << endl; 
    
    if (finalNode->getNodeType() == 2) 
        cost = this->Accommodations.at(finalNode->getNodeId())->getCost();
    
    return cost + distance_cost * (roundf(10*getDistance(initialNode, finalNode))/10) ;
}


void ProblemInstance::sortNodes() {
  sort(Bnodes.begin(), Bnodes.end(), sortById);
  sort(Dnodes.begin(), Dnodes.end(), sortById);
  sort(Anodes.begin(), Anodes.end(), sortById);
}

void ProblemInstance::printAll() {
  int td = this->n_depots;
  int tb = this->n_bridges;
  int ta = this->n_accommodations;
  int wt = this->worktime;
  int dc = this->dispatching_cost;
  int days = this->days;
  cout << endl;
  cout << "**Problem Instance**" << endl;
  cout << endl;

  cout << "Number of Depots: " << td << endl;

  for (int dn : this->depot_nodes) {
    cout << dn << "\t";
  }
  cout << endl;

  cout << "Number of Bridges: " << tb << endl;
  for (int dn : this->bridge_nodes) {
    cout << dn << "\t";
  }
  cout << endl;

  cout << "Number of Accommodations: " << ta << endl;
  for (int dn : this->accommodation_nodes) {
    cout << dn << "\t";
  }

  cout << endl;

  for (Node *n : this->Dnodes) {
    n->printBase();
  }
  cout << endl;

  for (Node *n : this->Bnodes) {
    n->printBase();
  }
  cout << endl;

  for (Node *n : this->Anodes) {
    n->printBase();
  }
  cout << endl;

  for (Accommodation *t : this->Accommodations) {
    t->printAll();
  }
  cout << endl;

  cout << "Worktime: " << wt << endl;
  cout << "Dispatching cost: " << dc << endl;
  cout << "Total Days: " << days << endl;

  cout << "Distance Matrix Bridge x Bridge" << endl;

  for (int i = 0; i < tb; i++) {
    for (int j = 0; j < tb; j++) {
      cout << this->distances_bxb[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;

  cout << "Distance Matrix Bridge x Accommodation" << endl;

  for (int i = 0; i < tb; i++) {
    for (int j = 0; j < ta; j++) {
      cout << this->distances_bxa[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;

  cout << "Distance Matrix Accommodation x Bridge" << endl;

  for (int i = 0; i < ta; i++) {
    for (int j = 0; j < tb; j++) {
      cout << this->distances_axb[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;

  cout << "Distance Matrix Depot x Bridge" << endl;

  for (int i = 0; i < td; i++) {
    for (int j = 0; j < tb; j++) {
      cout << this->distances_dxb[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;

  cout << "Distance Matrix Bridge x Depot" << endl;

  for (int i = 0; i < tb; i++) {
    for (int j = 0; j < td; j++) {
      cout << this->distances_bxd[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;
}
