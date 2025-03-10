#include "Route.h"
#include "algorithm"

Route::Route() { this->routecost = 0; }

Route::Route(vector<Node *> nodes) {
  for (Node *node : this->rute_nodes) {
      delete node;
  }
   this->rute_nodes.clear();
   this->rute_nodes.shrink_to_fit();

  for (Node *node : nodes) {
    Node *copy = new Node(*node);
    this->rute_nodes.push_back(copy);
  }

  this->setRouteCost();
  this->setTotalRouteCost();
  this->setRouteTime();
}

Route::Route(const Route &r1) {
  
   /*for (Node *node : this->rute_nodes) {
      delete node;
  }
  this->rute_nodes.clear();
  this->rute_nodes.shrink_to_fit();
  */
  
  for (Node *node : r1.rute_nodes) {
    Node *copy = new Node(*node);
    this->rute_nodes.push_back(copy);
  }
  
  routecost = r1.routecost; 

//   this->daytime.clear();
//   this->daytime.shrink_to_fit();
  for (int dayt:r1.daytime)
      daytime.push_back(dayt);

  
//   this->daycost.clear();
//   this->daycost.shrink_to_fit();  
  for (float dayc:r1.daycost)
      daycost.push_back(dayc);
  
    return;
}

Route::~Route() {
  for (Node *m : this->rute_nodes) {
    delete m;
  }

  this->rute_nodes.clear();
  this->rute_nodes.shrink_to_fit();

  this->daytime.clear();
  this->daytime.shrink_to_fit();

  this->daycost.clear();
  this->daycost.shrink_to_fit();
}

void Route::AddNode(Node *newnode) { this->rute_nodes.push_back(newnode); }

void Route::setTotalRouteCost() {
  this->routecost = 0;
  for (float cost : this->daycost) {
    this->routecost += cost;
  }
}

void Route::setTotalRouteCost(float cost) {
    this->routecost = cost; 
}

void Route::setRouteCost() {
  this->daycost.clear();
  this->daycost.shrink_to_fit();
  
  float day_cost = 0;
  for (auto i = this->rute_nodes.begin() + 1; i != this->rute_nodes.end(); i++) {
    if ((*i)->getNodeType() == 0 || (*i)->getNodeType() == 2) {
      day_cost += (*i)->getCost();
      //cout << "pusheando day_cost " << day_cost << endl;
      this->daycost.push_back(day_cost);
      day_cost = 0;
    } else {
      day_cost += (*i)->getCost();
    }
  }
  //this->printRoute();
}

void Route::setRouteCost(int day, float cost) { this->daycost.at(day) = cost; }

void Route::setRouteTime() {
  this->daytime.clear();
  this->daytime.shrink_to_fit();
  
  int day_time = 0;
  for (auto i = this->rute_nodes.begin() + 1; i != this->rute_nodes.end(); i++) {
    if ((*i)->getNodeType() == 0 || (*i)->getNodeType() == 2) {
      this->daytime.push_back(day_time);
      day_time = 0;
    } else {
      day_time += (*i)->getTime();
    }
  }
}

void Route::setRouteTime(int day, int new_time) {
  this->daytime.at(day) = new_time;
}

float Route::getRouteCost() {
    float totalCost=0;
    for (float cost: this->daycost)
        totalCost+=cost;
    return totalCost;
    //return this->routecost; 
}

float Route::getRouteCost(int day) {
    return this->daycost.at(day); 
}

float Route::getSubRouteCost(int first, int last) {
  float cost = 0;
  for (int i = first; i < last; i++) {
    cost += this->rute_nodes.at(i + 1)->getCost();
  }
  return cost;
}
/*
void Route::getRouteTime(vector<int> &routetimes) {
  routetimes.clear();
  for (int i : this->daytime) {
    routetimes.push_back(i);
  }
}
*/
int Route::getRouteTime(int day) {
    return this->daytime.at(day); 
    
}

int Route::getSubRouteTime(int first, int last) {
  int time = 0;
  for (int i = first; i < last; i++) {
    time += this->rute_nodes.at(i + 1)->getTime();
  }
  return time;
}

bool Route::timefeasible(int maxtime) {
  for (int i : this->daytime) {
    if (i > maxtime) {
      return false;
    }
  }
  return true;
}

int Route::getRouteDays() {
  this->setRouteTime();
  return daytime.size();
}

int Route::getNodeDay(int index) {
  int day = 0;
  for (int i = 1; i < index; i++) {
    if (this->rute_nodes.at(i)->getNodeType() == 2 ||
        this->rute_nodes.at(i)->getNodeType() == 0) {
      day++;
    }
  }
  return day;
}

int Route::countnodes() {

  int nodes = 0;
  for (Node *node : this->rute_nodes) {
    if (node->getNodeType() == 1) {
      nodes++;
    }
  }
  return nodes;
}

void Route::printRoute() {
  for (Node *node : this->rute_nodes) {
    cout << node->getId() << "->";
  }
  cout << "END {";
  
  for (int c: daycost)
   cout << " " << c ;    

  cout << " } [";
  for (int t: daytime)
   cout << " " << t ;    

  cout << " ] " << endl;
}

void Route::printAll() {

  cout << endl;

  int daycount = 0;
  this->printRoute();

  this->setRouteCost();
  this->setTotalRouteCost();
  this->setRouteTime();

  for (Node *node : this->rute_nodes) {
    node->printAll();
  }

  cout << endl;
  cout << "Team Cost: " << this->routecost << endl;

  for (int i : this->daytime) {
    cout << "Worktime on day " << daycount << " : " << i << endl;
    daycount++;
  }

  daycount = 0;

  for (float i : this->daycost) {
    cout << "Cost on day " << daycount << " : " << i << endl;
    daycount++;
  }

  cout << endl;
}
