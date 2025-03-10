
#include "Team.h"
#include "algorithm"
#include <iomanip>

Team::Team() {
  this->worktime = 0;
  this->TotalVisited = 0;
  this->workdays = 1;
  this->totalcost = 0;
}

Team::Team(const Team &t1) {

  this->visited_nodes.clear();
  this->visited_nodes.shrink_to_fit();
  for (Node *node : t1.visited_nodes) {
    Node *copy = new Node(*node);
    this->visited_nodes.push_back(copy);
  }

  route = new Route(*t1.route);
  worktime = t1.worktime;
  workdays = t1.worktime;
  totalcost = t1.totalcost;
  TotalVisited = t1.TotalVisited;
}

Team::~Team() = default;

void Team::AddNode(Node *newnode) { this->visited_nodes.push_back(newnode); }
void Team::AddCost(float cost) { this->totalcost = this->totalcost + cost; }
void Team::AddVisited() { this->TotalVisited = this->TotalVisited + 1; }
void Team::PassDay() { this->workdays = this->workdays + 1; }
void Team::AddWorkTime(int work) { this->worktime = this->worktime + work; }
void Team::ResetWorkitme() { this->worktime = 0; }

// Devuelve los valores anteriores en caso de tener que volver al deposito.
void Team::RestoreAccommodation() {
  this->totalcost = this->totalcost - this->visited_nodes.back()->getCost();
  this->workdays = this->workdays - 1;
  this->visited_nodes.pop_back();
}

void Team::NodetoRoute() { this->route = new Route(this->visited_nodes); }

void Team::printRoute() {
  for (Node *node : this->route->rute_nodes) {
    cout << node->getId() << "->";
  }
  cout << "END" << endl;
}

void Team::printAll() {
  this->route->printAll();

  /*
      this->printRoute();

      for (Node *node: this->route->rute_nodes){
          node->printAll();
      }


      cout << endl;
      cout << "Team Cost: " << fixed << setprecision(0) << this->totalcost <<
     endl; cout << "Work Days: " << this->workdays << endl; cout << "Total
     Visited Nodes: " << this->TotalVisited << endl;
  */
}


void Team::printSmall() {
      this->printRoute();
      cout << endl;
      cout << "Team Cost: " << fixed << setprecision(0) << this->totalcost << endl;
      cout << "Work Days: " << this->workdays << endl; cout << "Total Visited Nodes: " << this->TotalVisited << endl;
}


bool Team::isVisited(int id) {
  for (Node *node : this->visited_nodes) {
    if (node->getId() == id)
      return true;
  }
  return false;
}
