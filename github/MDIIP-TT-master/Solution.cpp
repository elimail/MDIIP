#include "Solution.h"
#include "algorithm"

Solution::Solution(ProblemInstance *problemInstance) {
  this->totalcost = 0;
  this->minvisit = 1;
  //TODO: Revisar como manejé esto antes.
  if(problemInstance->getTotalDepots() > 1) this->maxvisit = problemInstance->getTotalBridges() - minvisit;
  else this->maxvisit = problemInstance->getTotalBridges();
  this->speed = 60;
  this->distance_cost = 800;

  for (Node *bn : problemInstance->Bnodes) {
    this->Unvisited_bridges.push_back(bn);
  }

  for (Node *dn : problemInstance->Dnodes) {
    auto *team = new Team();
    team->AddNode(dn);
    team->AddCost(problemInstance->getdispatching_cost());
    this->Teams.push_back(team);
  }

  this->problemInstance = problemInstance;
}

Solution::~Solution() {}

Solution::Solution(const Solution &s2) {

  this->Teams.clear();
  this->Teams.shrink_to_fit();
  for (Team *team : s2.Teams) {
    Team *Tcopy = new Team(*team);
    this->Teams.push_back(Tcopy);
  }

  this->Unvisited_bridges.clear();
  this->Unvisited_bridges.shrink_to_fit();
  for (Node *node : s2.Unvisited_bridges) {
    Node *Ucopy = new Node(*node);
    this->Unvisited_bridges.push_back(Ucopy);
  }

  this->available_visit.clear();
  this->available_visit.shrink_to_fit();
  for (Node *node : s2.available_visit) {
    Node *Acopy = new Node(*node);
    this->available_visit.push_back(Acopy);
  }

  problemInstance = s2.problemInstance;

  totalcost = s2.totalcost;
  minvisit = s2.minvisit;
  maxvisit = s2.maxvisit;
  speed = s2.speed;
  distance_cost = s2.distance_cost;
}

void Solution::reset(const Solution &s2) {

  for (Team *team : this->Teams) {
    delete team;
  }

  this->Teams.clear();
  this->Teams.shrink_to_fit();

  for (Team *team : s2.Teams) {
    Team *Tcopy = new Team(*team);
    this->Teams.push_back(Tcopy);
  }

  for (Node *node : this->Unvisited_bridges) {
    delete node;
  }

  this->Unvisited_bridges.clear();
  this->Unvisited_bridges.shrink_to_fit();

  for (Node *node : s2.Unvisited_bridges) {
    Node *Ucopy = new Node(*node);
    this->Unvisited_bridges.push_back(Ucopy);
  }

  for (Node *node : this->available_visit) {
    delete node;
  }

  this->available_visit.clear();
  this->available_visit.shrink_to_fit();
  for (Node *node : s2.available_visit) {
    Node *Acopy = new Node(*node);
    this->available_visit.push_back(Acopy);
  }

  problemInstance = s2.problemInstance;

  totalcost = s2.totalcost;
  minvisit = s2.minvisit;
  maxvisit = s2.maxvisit;
  speed = s2.speed;
  distance_cost = s2.distance_cost;
}

int Solution::random_int_number(int min, int max) {
  int randomNumber = rand() % (max - min + 1) + min;
  return randomNumber;
}

void Solution::setTotalCost() {
  for (Team *t : this->Teams) {
    totalcost += t->totalcost;
  }
}

float Solution::getTotalCost() {
  float totalcost = 0;
  for (Team *t : this->Teams) {
    t->route->setRouteCost();
    totalcost += t->route->getRouteCost();
    //cout << "totalcost: " << totalcost << endl;
    totalcost += problemInstance->getdispatching_cost();
    //cout << "totalcost: " << totalcost << endl;
  }
  //cout << "totalcost: " << totalcost << endl;
  return totalcost;
}

void Solution::changeDistance(Node *i, Node *j) {
  bool debug=false;
  if (debug) cout << "changeDistance from: " << i->getId() << " to: "  << j->getId() << endl;
  
  int row_type = i->getNodeType();
  int col_type = j->getNodeType();
  int row_ID = i->getNodeId();
  int col_ID = j->getNodeId();

  float distance, time, cost;

  if (row_type == 1 && col_type == 1) {
    distance = this->problemInstance->distances_bxb[row_ID][col_ID];
    time = roundf(distance) + j->getBridge_wt(); // Round con 0 decimales
    cost = this->distance_cost * (roundf(distance * 10) / 10); // Round con 1 decimal
    j->setDistance(distance);
    j->setCost(cost);
    j->setTime(static_cast<int>(time));

  }

  else if (row_type == 0 && col_type == 1) {
    distance = this->problemInstance->distances_dxb[row_ID][col_ID];
    time = roundf(distance) + j->getBridge_wt(); // Round con 0 decimales
    cost = this->distance_cost * (roundf(distance * 10) / 10); // Round con 1 decimal
    j->setDistance(distance);
    j->setCost(cost);
    j->setTime(static_cast<int>(time));
  }

  else if (row_type == 2 && col_type == 1) {
    distance = this->problemInstance->distances_axb[row_ID][col_ID];
    time = roundf(distance) + j->getBridge_wt(); // Round con 0 decimales
    cost = this->distance_cost * (roundf(distance * 10) / 10); // Round con 1 decimal
    j->setDistance(distance);
    j->setCost(cost);
    j->setTime(static_cast<int>(time));
  }

  else if (row_type == 1 && col_type == 0) {
    distance = this->problemInstance->distances_bxd[row_ID][col_ID];
    time = 0;
    cost = this->distance_cost * (roundf(distance * 10) / 10); // Round con 1 decimal
    j->setDistance(distance);
    j->setCost(cost);
    j->setTime(static_cast<int>(time));
  }

  else if (row_type == 1 && col_type == 2) {
    distance = this->problemInstance->distances_bxa[row_ID][col_ID];
    time = 0;
    cost = this->problemInstance->Accommodations.at(col_ID)->getCost() + (this->distance_cost * (roundf(distance * 10) / 10)); // Round con 1 decimal
    j->setDistance(distance);
    j->setCost(cost);
    j->setTime(static_cast<int>(time));
  }
}

// Verifica los puentes que es posible visitar de acuerdo al tiempo de trabajo diponible.
void Solution::getAvailableBridges(Team *team) {
  bool debug = false;

  this->available_visit.clear();
  float node_time;
  float node_distance;
  float node_cost;

  Node *current_node = new Node(*team->visited_nodes.back());

  if(debug) cout << "tipo: " << current_node->getNodeType() << endl;

  switch (current_node->getNodeType()) {
  case 0:
    for (Node *b : this->Unvisited_bridges) {
      node_distance = this->problemInstance->distances_dxb[current_node->getNodeId()][b->getNodeId()];
      node_time = roundf(node_distance); // Round con 0 decimales
      node_time = node_time += b->getBridge_wt();
      if (debug) cout << "node_time: " << node_time << ", team->worktime: " << team->worktime << ", problemInstance->getworktime()" << problemInstance->getworktime() << endl;
      if (node_time + team->worktime <= problemInstance->getworktime()) {
        if (debug) cout << b->getNodeId() << "cumple con la condición" << endl;
        node_cost = this->distance_cost * (roundf(node_distance * 10) / 10); // Round con 1 decimal
        b->setDistance(node_distance);
        b->setCost(node_cost);
        b->setTime(static_cast<int>(node_time));
        available_visit.push_back(b);
      }
    }
    break;
  case 1:
    for (Node *b : this->Unvisited_bridges) {
      node_distance = this->problemInstance->distances_bxb[current_node->getNodeId()][b->getNodeId()];
      node_time = roundf(node_distance); // Round con 0 decimales
      node_time = node_time += b->getBridge_wt();
      if (node_time + team->worktime <= problemInstance->getworktime()) {
        node_cost = this->distance_cost * (roundf(node_distance * 10) / 10); // Round con 1 decimal
        b->setDistance(node_distance);
        b->setCost(node_cost);
        b->setTime(static_cast<int>(node_time));
        available_visit.push_back(b);
      }
    }
    break;
  case 2:
    for (Node *b : this->Unvisited_bridges) {
      node_distance = this->problemInstance->distances_axb[current_node->getNodeId()][b->getNodeId()];
      node_time = roundf(node_distance); // Round con 0 decimales
      node_time = node_time += b->getBridge_wt();
      if (node_time + team->worktime <= problemInstance->getworktime()) {
        node_cost = this->distance_cost * (roundf(node_distance * 10) / 10); // Round con 1 decimal
        b->setDistance(node_distance);
        b->setCost(node_cost);
        b->setTime(static_cast<int>(node_time));
        available_visit.push_back(b);
      }
    }
    break;
  }
  sort(this->available_visit.begin(), this->available_visit.end(), sortByCost);

}
// Selecciona uno de los 3 puentes disponibles [más cercanos] para ser visitado y lo agrega a la ruta.
void Solution::selectBridge(Team *team) {
  int selection = this->random_int_number(0, 2);
  bool debug = false;
  if (debug) cout << "selection: " << selection << endl;
  if (debug) cout << "this->available_visit.size(): " << this->available_visit.size() << endl;
  if (selection < this->available_visit.size()) {
  } else if (selection - 1 < this->available_visit.size()) {
    selection = selection - 1;
  } else {
    selection = selection - 2;
  }

  int selectID = this->available_visit[selection]->getId();
  if (debug) cout << "selectID:" << selectID << endl;
  team->AddNode(this->available_visit[selection]);
  team->AddWorkTime(this->available_visit[selection]->getTime());
  team->AddCost(this->available_visit[selection]->getCost());
  team->AddVisited();
  this->Unvisited_bridges.erase(std::remove_if(Unvisited_bridges.begin(), Unvisited_bridges.end(), [&selectID](Node *node) { return (node->getId() == selectID); }));

}

// Selecciona uno de los 2 Alojamientos con menor costo, lo agrega a la ruta actualiza los valores del equipo de trabajo.
void Solution::selectAccommodation(Team *team) {
  bool debug = false;
  this->available_visit.clear();
  float node_distance;
  float node_cost;

  int current_nodeID = team->visited_nodes.back()->getNodeId();
  
  if (team->visited_nodes.back()->getNodeType() == 0){
      cout << "El nodo anterior es un deposito " << endl;
      getchar(); getchar();
  }
  
  //if (debug) team->route->printRoute();

  for (Node *a : this->problemInstance->Anodes) {
    Node *accomodation_node = new Node(*a);
    node_distance = this->problemInstance->distances_bxa[current_nodeID][accomodation_node->getNodeId()];
    if (debug) cout << "From: " << team->visited_nodes.back()->getId() << " to: " << accomodation_node->getId() << " node_distance: " << node_distance << endl;
    node_cost = this->problemInstance->Accommodations.at(accomodation_node->getNodeId())->getCost() + (this->distance_cost * (roundf(node_distance * 10) / 10)); // Round con 1 decimal
    accomodation_node->setDistance(node_distance);
    accomodation_node->setCost(node_cost);
    available_visit.push_back(accomodation_node);
  }
  sort(this->available_visit.begin(), this->available_visit.end(), sortByCost);
  int selection = this->random_int_number(0, 1);
  team->AddNode(this->available_visit[selection]);
  team->AddCost(this->available_visit[selection]->getCost());
  team->PassDay();
  team->ResetWorkitme();
}

void Solution::selectAccommodation(Route *route) {
  this->available_visit.clear();
  float node_distance;
  float node_cost;

  int current_nodeID = route->rute_nodes.back()->getNodeId();

  for (Node *a : this->problemInstance->Anodes) {
    Node *accomodation_node = new Node(*a);
    node_distance = this->problemInstance->distances_bxa[current_nodeID][accomodation_node->getNodeId()];
    node_cost = this->problemInstance->Accommodations.at(accomodation_node->getNodeId())->getCost() + (this->distance_cost * (roundf(node_distance * 10) / 10)); // Round con 1 decimal
    accomodation_node->setDistance(node_distance);
    accomodation_node->setCost(node_cost);
    available_visit.push_back(accomodation_node);
  }

  sort(this->available_visit.begin(), this->available_visit.end(), sortByCost);
  Node *selected_accommodation = new Node(*this->available_visit[0]);

  route->AddNode(selected_accommodation);
  route->daycost.back() += this->available_visit[0]->getCost();
  route->daycost.push_back(0);
  route->daytime.push_back(0);

  for (Node *n : this->available_visit) {
    delete n;
  }
  this->available_visit.clear();
  this->available_visit.shrink_to_fit();
}

void Solution::changeAccommodation(Route *route, int swap, int day) {

  vector<Node *> toNode;

  this->available_visit.clear();
  toNode.clear();

  Node *previous_Node = new Node(*route->rute_nodes.at(swap - 1));
  Node *next_Node = new Node(*route->rute_nodes.at(swap + 1));
  Node *Accommodation_Node = new Node(*route->rute_nodes.at(swap));

  float toAccommodation_distance;
  float Accommodation_cost;
  int daytime = route->getRouteTime(day) - next_Node->getTime();
  float min_cost = Accommodation_Node->getCost() + next_Node->getCost();
  int min_i;
  bool change = false;
  float previous_daycost = route->getRouteCost(day - 1) - Accommodation_Node->getCost();
  float actual_daycost = route->getRouteCost(day) - next_Node->getCost();

  int previus_nodeID = previous_Node->getNodeId();
  int next_nodeID = next_Node->getNodeId();

  for (Node *a : this->problemInstance->Anodes) {
    Node *new_Accommodation = new Node(*a);
    Node *new_Next = new Node(*next_Node);
    toAccommodation_distance = this->problemInstance->distances_bxa[previus_nodeID][new_Accommodation->getNodeId()];
    Accommodation_cost = this->problemInstance->Accommodations.at(new_Accommodation->getNodeId())->getCost() + (this->distance_cost * (roundf(toAccommodation_distance * 10) / 10)); // Round con 1 decimal
    new_Accommodation->setDistance(toAccommodation_distance);
    new_Accommodation->setCost(Accommodation_cost);
    available_visit.push_back(new_Accommodation);
    changeDistance(new_Accommodation, new_Next);
    toNode.push_back(new_Next);
  }

  for (int i = 0; i < toNode.size(); i++) {
    if (daytime + toNode.at(i)->getTime() <= this->problemInstance->getworktime()) {
      if (available_visit.at(i)->getCost() + toNode.at(i)->getCost() < min_cost) {
        min_cost = available_visit.at(i)->getCost() + toNode.at(i)->getCost();
        min_i = i;
        change = true;
      }
    }
  }

  if (change) {
    Node *New_Accommodation = new Node(*available_visit.at(min_i));
    Node *New_toNode = new Node(*toNode.at(min_i));
    route->rute_nodes.at(swap) = New_Accommodation;
    route->rute_nodes.at(swap + 1) = New_toNode;
    route->setRouteTime(day, daytime + New_toNode->getTime());
    route->setRouteCost(day - 1, previous_daycost + New_Accommodation->getCost());
    route->setRouteCost(day, actual_daycost + New_toNode->getCost());
  }

  for (Node *a : this->available_visit) {
    delete a;
  }

  for (Node *n : toNode) {
    delete n;
  }

  delete previous_Node;
  delete next_Node;
  delete Accommodation_Node;

  this->available_visit.clear();
  this->available_visit.shrink_to_fit();
  toNode.clear();
  toNode.shrink_to_fit();
}

// Termina la ruta del equipo agregando el deposito y el costo asociado de la vuelta.
void Solution::returnToDepot(Team *team) {
  float node_distance;
  float node_cost;

  Node *depot_node = new Node(*team->visited_nodes.front());

  int depotID = depot_node->getNodeId();
  int current_nodeID = team->visited_nodes.back()->getNodeId();

  node_distance = this->problemInstance->distances_bxd[current_nodeID][depotID];
  node_cost = this->distance_cost * (roundf(node_distance * 10) / 10);

  depot_node->setCost(node_cost);
  depot_node->setDistance(node_distance);

  team->AddNode(depot_node);
  team->AddCost(node_cost);
}

void Solution::returnToDepot(Route *route) {
  float node_distance;
  float node_cost;

  Node *depot_node = new Node(*route->rute_nodes.front());

  int depotID = depot_node->getNodeId();
  int current_nodeID = route->rute_nodes.back()->getNodeId();

  node_distance = this->problemInstance->distances_bxd[current_nodeID][depotID];
  node_cost = this->distance_cost * (roundf(node_distance * 10) / 10);

  depot_node->setDistance(node_distance);
  depot_node->setCost(node_cost);
  route->AddNode(depot_node);

  route->daycost.back() += node_cost;
}

void Solution::deleteNode(Route *origin_route, int swap, int origin_day) {

  int origin_daytime;
  float origin_daycost;

  origin_daytime = origin_route->getRouteTime(origin_day);
  origin_daycost = origin_route->getRouteCost(origin_day);

  origin_daytime = origin_daytime - origin_route->rute_nodes.at(swap)->getTime() - origin_route->rute_nodes.at(swap + 1)->getTime();
  origin_daycost = origin_daycost - origin_route->rute_nodes.at(swap)->getCost() - origin_route->rute_nodes.at(swap + 1)->getCost();

  changeDistance(origin_route->rute_nodes.at(swap - 1), origin_route->rute_nodes.at(swap + 1));

  origin_daycost += origin_route->rute_nodes.at(swap + 1)->getCost();
  origin_daytime += origin_route->rute_nodes.at(swap + 1)->getTime();

  origin_route->setRouteTime(origin_day, origin_daytime);
  origin_route->setRouteCost(origin_day, origin_daycost);

  origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap);
}

void Solution::deleteNode(Route *origin_route, int swap, int origin_day, int size) {

  int origin_daytime;
  float origin_daycost;

  origin_daytime = origin_route->getRouteTime(origin_day);
  origin_daycost = origin_route->getRouteCost(origin_day);

  origin_daytime = origin_daytime - origin_route->rute_nodes.at(swap)->getTime() - origin_route->rute_nodes.at(swap + 1)->getTime();
  origin_daycost = origin_daycost - origin_route->rute_nodes.at(swap)->getCost() - origin_route->rute_nodes.at(swap + 1)->getCost();

  changeDistance(origin_route->rute_nodes.at(swap - 1), origin_route->rute_nodes.at(swap + 1));

  origin_daycost += origin_route->rute_nodes.at(swap + 1)->getCost();
  origin_daytime += origin_route->rute_nodes.at(swap + 1)->getTime();

  origin_route->setRouteTime(origin_day, origin_daytime);
  origin_route->setRouteCost(origin_day, origin_daycost);

  origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap);
}

void Solution::deleteAccommodation(Route *route, int index) {
  bool debug = false;
  if (debug) route->printRoute();
  
  if ((index + 1)  > route->rute_nodes.size()){
        cout << "Aca hay un error con el tamaño de la ruta que se envía a deleteAccommodation" << endl;
        cout << "index: " << index << endl;
        route->printRoute();
  }
      
  if (index >= route->rute_nodes.size()-1){
      cout << "ERROR con el tamaño en deleteAccomondation, index: " << index << " route->rute_nodes. size(): " << route->rute_nodes. size() << endl;
      getchar();
  }
  
  if (debug) route->printRoute();
  
  if (debug) cout << "El nodo :" << route->rute_nodes.at(index)->getId() << " es de tipo: " << route->rute_nodes.at(index)->getNodeType() << endl;
  if (route->rute_nodes.at(index)->getNodeType() == 0) {
    index++;
  }

  if (debug) cout << " Before  --- Borrando el nodo: "  << route->rute_nodes.at(index)->getId() << " de la posición: "  << index << endl;
  if (debug) route->printRoute();
  
  changeDistance(route->rute_nodes.at(index - 1), route->rute_nodes.at(index + 1));
  
  route->rute_nodes.erase(route->rute_nodes.begin() + index);
  
  //route->rute_nodes.shrink_to_fit();
  
  route->setRouteCost();
  route->setTotalRouteCost();
  route->setRouteTime();

  
  if (debug) cout << " After ---- Borrado el nodo. "<< endl;
  if (debug) route->printRoute();

}

bool Solution::isAccommodation(Route *route, int index, int size) {

  for (int i = 0; i < size; i++) {
    if (route->rute_nodes.at(index + i)->getNodeType() == 2) {
      return true;
    } else if (route->rute_nodes.at(index + i)->getNodeType() == 0) {
      return false;
    }
  }
  return false;
}

bool Solution::isDepot(Route *route, int index, int size) {

  for (int i = 0; i < size; i++) {
    if (route->rute_nodes.at(index + i)->getNodeType() == 0) {
      return true;
    }
  }
  return false;
}

void Solution::printAll() {
  for (Team *t : this->Teams) {
    t->printAll();
  }
  cout << endl;
  cout << "Total cost: " << getTotalCost() << endl;
}


void Solution::printSmall() {
  for (Team *t : this->Teams) {
    t->printSmall();
  }
  cout << endl;
  cout << "Total cost: " << getTotalCost() << endl;
}

void Solution::VNSAll(int max_iter) {
  bool debug = false;
  if (debug) cout << "================================================= VNS ===========================================" << endl; 
  int move_selection;
  int iter = 0;
  int improve = 0;
  float new_sol;
  float best = this->getTotalCost();
  int nodes;

  while (improve < max_iter) {
    move_selection = this->random_int_number(0, 4);

    if(debug) cout << "Movimiento (VNSAll):  " << move_selection << endl;
    switch (move_selection) {

    case 0:
      this->changeRoutes2opt();
      break;

    case 1:
      this->changeRoutesRouteSwap(5);
      break;

    case 2:
      this->changeRoutesNodeDay();
      break;

    case 3:
      this->changeRoutesNewDay();
      break;

    case 4:
      this->changeRoutesAccommodation();
      break;
    }
    this->check();
    iter++;

    new_sol = this->getTotalCost();
    if (new_sol < best) {
      best = new_sol;
      improve = 0;
    }

    else {
      improve++;
    }
  }
}

void Solution::DisturbAll(int max_iter) {
  bool debug = false;
  int move_selection;
  int iter = 0;
  int improve = 0;
  int nodes = 0;
  float new_sol;
  float best = this->getTotalCost();

  while (iter < max_iter) {
    move_selection = this->random_int_number(0, 4);
    
  if (debug) cout << "Movimiento (DisturbAll): " << move_selection << endl;

  switch (move_selection) {

    case 0:
      this->DisturbRoutes2opt();
      break;

    case 1:
      this->DisturbRoutesRouteSwap();
      break;

    case 2:
      this->DisturbRoutesDayIteration();
      break;

    case 3:

      this->DisturbRoutesNewDay();
      break;

    case 4:

      this->DisturbRoutesAccommodation();
      break;
    }
    
    this->check();
    
    iter++;
  }
}

void Solution::changeRoutes2opt() {

  int day_count = 0;
  int initial_node, final_node;
  int count = 0;
  bool flag = true;

  for (Team *t : this->Teams) {
    for (Node *n : t->route->rute_nodes) {

      if (n->getNodeType() == 2 || n->getNodeType() == 0) {
        if (flag) {
          initial_node = count;
          flag = false;
        } else {
          final_node = count + 1;
          twoOpt(t->route, initial_node, final_node, day_count);
          day_count++;
          initial_node = count;
        }
      }
      count++;
    }
    t->route->setTotalRouteCost();

    day_count = 0;
    count = 0;
    flag = true;
  }
}

void Solution::twoOpt(Route *actual_route, int initial_node, int final_node,
                      int day_count) {
  bool debug = false;
  vector<Node *> route(actual_route->rute_nodes.begin() + initial_node, actual_route->rute_nodes.begin() + final_node);

  Route *new_route = new Route(route);

  int node_count;
  int size = new_route->rute_nodes.size();

  if (size < 4) { // Si la ruta tiene es muy pequeña no se hacen cambios si tiene largo 3 o menor
    delete new_route;
    return;
  }

  for (int i = 1; i < size - 2; i++) { //Se recorre desde la posición porque la posición 1 tiene el alojamiento o el depósito y al final se hace la misma exclusión
    for (int k = i + 1; k < size - 1; k++) {
    
      twoOptSwap(new_route, i, k);

      if (new_route->timefeasible(this->problemInstance->getworktime()) &&
          new_route->getRouteCost() < actual_route->getRouteCost(day_count)) {

        node_count = 0;
        for (int it = initial_node; it < final_node; it++) {
          delete actual_route->rute_nodes.at(it);
          actual_route->rute_nodes.at(it) = new_route->rute_nodes.at(node_count);
          if(debug) actual_route->rute_nodes.at(it)->printAll();
          node_count++;
        }
        if(debug) cout << "Se acabó la copia " << endl;
        
        actual_route->setRouteCost(day_count, new_route->getRouteCost());
        actual_route->setRouteTime(day_count, new_route->getRouteTime(0));
        
        return;
      } 
    }
  }
  delete new_route;
}

void Solution::twoOptSwap(Route *route, int i, int k) {
  bool debug = false;
  if (debug) cout << "i: " << i << ", k: " << k << endl;
  vector<Node *> new_route;

  float new_cost = 0;
  float new_time = 0;

  Node *new_node = new Node(*route->rute_nodes.at(0)); //Es una sub-ruta así que toma su primer nodo.
  new_route.push_back(new_node);
  // new_route.back()->setCost(0);

  for (int c = 1; c <= i - 1; ++c) {
    Node *new_node = new Node(*route->rute_nodes.at(c));
    new_route.push_back(new_node);
    new_cost += new_route.back()->getCost();
    if (debug) cout << "new_route.back()->getCost(): " << new_route.back()->getCost() << endl;
    new_time += new_route.back()->getTime();
  }

  int dec = 0;
  for (int c = i; c <= k; ++c) {
    Node *new_node = new Node(*route->rute_nodes.at(k - dec));
    this->changeDistance(new_route.back(), new_node); //Calcula la nueva distancia/nuevo costo y nuevo tiempo.
    new_route.push_back(new_node);
    new_cost += new_route.back()->getCost();
    if (debug) cout << "new_route.back()->getCost(): " << new_route.back()->getCost() << endl;
    new_time += new_route.back()->getTime();
    dec++;
  }

  for (int c = k + 1; c < route->rute_nodes.size(); ++c) {
    Node *new_node = new Node(*route->rute_nodes.at(c));
    this->changeDistance(new_route.back(), new_node);
    if (debug) new_route.back()->printAll();
    if (debug) new_node->printAll();

    new_route.push_back(new_node);
    new_cost += new_route.back()->getCost();
    
    if (debug) cout << "*new_route.back()->getCost(): " << new_route.back()->getCost() << endl;
    
    new_time += new_route.back()->getTime();
  }

  for (Node *m : route->rute_nodes) {
    delete m;
  }
  route->rute_nodes.clear();
  route->rute_nodes.shrink_to_fit();
  
  route->rute_nodes = (new_route);

  route->setTotalRouteCost(new_cost);
  route->setRouteCost(0, new_cost);
  route->setRouteTime(0, new_time);
}

void Solution::changeRoutesNodeSwap() {

  int day_node1 = 0;
  int day_node2 = 0;
  int swap1 = 0;
  int swap2 = 0;

  for (Team *t1 : this->Teams) {
    for (Team *t2 : this->Teams) {

      for (Node *n : t1->route->rute_nodes) {
        if (n->getNodeType() == 2) {
          day_node1++;
          swap1++;
          continue;
        } else if (n->getNodeType() == 0) {
          swap1++;
          continue;
        }

        for (Node *m : t2->route->rute_nodes) {
          if (m->getNodeType() == 2) {
            day_node2++;
            swap2++;
            continue;
          } else if (m->getNodeType() == 0) {
            swap2++;
            continue;
          }
          if (day_node1 == day_node2 &&
              t1->route->rute_nodes.front()->getId() ==
                  t2->route->rute_nodes.front()->getId()) {
            swap2++;
            continue;
          }

          nodeSwap(t1->route, swap1, day_node1, t2->route, swap2, day_node2);
          swap2++;
        }
        swap1++;
        swap2 = 0;
        day_node2 = 0;
      }
      swap1 = 0;
      day_node1 = 0;
    }
    t1->route->setTotalRouteCost();
  }
}

void Solution::nodeSwap(Route *route1, int swap1, int day1, Route *route2,
                        int swap2, int day2) {

  float daycost1 = route1->getRouteCost(day1);
  float daycost2 = route2->getRouteCost(day2);
  int daytime1 = route1->getRouteTime(day1);
  int daytime2 = route2->getRouteTime(day2);

  shared_ptr<Node> previus_node1(new Node(*route1->rute_nodes.at(swap1 - 1)));
  shared_ptr<Node> actual_node1(new Node(*route1->rute_nodes.at(swap1)));
  shared_ptr<Node> next_node1(new Node(*route1->rute_nodes.at(swap1 + 1)));

  // Node *previus_node1 = new Node(*route1->rute_nodes.at(swap1-1));
  // Node *actual_node1 = new Node(*route1->rute_nodes.at(swap1));
  // Node *next_node1 = new Node(*route1->rute_nodes.at(swap1+1));

  shared_ptr<Node> previus_node2(new Node(*route2->rute_nodes.at(swap2 - 1)));
  shared_ptr<Node> actual_node2(new Node(*route2->rute_nodes.at(swap2)));
  shared_ptr<Node> next_node2(new Node(*route2->rute_nodes.at(swap2 + 1)));

  // Node *previus_node2 = new Node(*route2->rute_nodes.at(swap2-1));
  // Node *actual_node2 = new Node(*route2->rute_nodes.at(swap2));
  // Node *next_node2 = new Node(*route2->rute_nodes.at(swap2+1));

  daycost1 = daycost1 - actual_node1->getCost() - next_node1->getCost();
  daycost2 = daycost2 - actual_node2->getCost() - next_node2->getCost();

  daytime1 = daytime1 - actual_node1->getTime() - next_node1->getTime();
  daytime2 = daytime2 - actual_node2->getTime() - next_node2->getTime();

  changeDistance(previus_node1.get(), actual_node2.get());
  changeDistance(previus_node2.get(), actual_node1.get());

  changeDistance(actual_node1.get(), next_node2.get());
  changeDistance(actual_node2.get(), next_node1.get());

  daycost1 = daycost1 + actual_node2->getCost() + next_node1->getCost();
  daycost2 = daycost2 + actual_node1->getCost() + next_node2->getCost();

  daytime1 = daytime1 + actual_node2->getTime() + next_node1->getTime();
  daytime2 = daytime2 + actual_node1->getTime() + next_node2->getTime();

  if (daytime1 < this->problemInstance->getworktime() &&
      daytime2 < this->problemInstance->getworktime()) {
    if (daycost1 + daycost2 <
        route1->getRouteCost(day1) + route2->getRouteCost(day2)) {

      if (route1->rute_nodes.at(0) == route2->rute_nodes.at(0)) {

        swap(route1->rute_nodes.at(swap1), route1->rute_nodes.at(swap2));
        swap(route2->rute_nodes.at(swap1), route2->rute_nodes.at(swap2));

      } else {
        swap(route1->rute_nodes.at(swap1), route2->rute_nodes.at(swap2));
      }

      changeDistance(route1->rute_nodes.at(swap1 - 1),
                     route1->rute_nodes.at(swap1));
      changeDistance(route1->rute_nodes.at(swap1),
                     route1->rute_nodes.at(swap1 + 1));

      changeDistance(route2->rute_nodes.at(swap2 - 1),
                     route2->rute_nodes.at(swap2));
      changeDistance(route2->rute_nodes.at(swap2),
                     route2->rute_nodes.at(swap2 + 1));

      route1->setRouteTime(day1, daytime1);
      route2->setRouteTime(day2, daytime2);

      route1->setRouteCost(day1, daycost1);
      route2->setRouteCost(day2, daycost2);
    }
  }
}

void Solution::changeRoutesRouteSwap(int lenght) {

  int sub_route = 0;
  int max_sub = lenght;

  bool best_solution = true;

  int origin_day = 0;
  int origin_swap = 0;
  int destiny_day = 0;
  int destiny_swap = 0;

  bool change_day = false;

  while (sub_route < max_sub) {
    origin_day = 0;
    origin_swap = 0;
    destiny_day = 0;
    destiny_swap = 0;
    change_day = false;
    for (Team *t1 : this->Teams) {
      Route *destiny = t1->route;
      for (Node *n : destiny->rute_nodes) {
        if (n->getNodeType() == 0) {
          destiny_swap++;
          continue;

        } else if (isAccommodation(destiny, destiny_swap, sub_route + 1)) {
          if (n->getNodeType() == 2) {
            destiny_day++;
          }
          destiny_swap++;
          continue;
        }

        else if (isDepot(destiny, destiny_swap, sub_route + 1)) {
          continue;
        }

        for (Team *t2 : this->Teams) {
          Route *origin = t2->route;
          for (Node *m : origin->rute_nodes) {
            if (isAccommodation(origin, origin_swap, sub_route + 1)) {
              if (m->getNodeType() == 2) {
                origin_day++;
              }
              origin_swap++;
              continue;
            }

            else if (m->getNodeType() == 0) {
              origin_swap++;
              continue;
            }

            else if (isDepot(origin, origin_swap, sub_route + 1)) {
              continue;
            }
            if (origin->rute_nodes.at(0)->getId() ==
                    destiny->rute_nodes.at(0)->getId() &&
                origin_day == destiny_day) {
              origin_swap++;
              continue;
            }

            best_solution = routeSwap(origin, origin_swap, origin_day, destiny,
                                      destiny_swap, destiny_day, sub_route);

            if (best_solution) {
              return;
            }
            origin_swap++;
          }
          origin_day = 0;
          origin_swap = 0;
        }
        destiny_swap++;
      }
      destiny_day = 0;
      destiny_swap = 0;
    }
    sub_route++;
  }
}

bool Solution::routeSwap(Route *route1, int swap1, int day1, Route *route2,
                         int swap2, int day2, int length) {

  float daycost1 = route1->getRouteCost(day1);
  float daycost2 = route2->getRouteCost(day2);
  int daytime1 = route1->getRouteTime(day1);
  int daytime2 = route2->getRouteTime(day2);

  float subroutecost1 = route1->getSubRouteCost(swap1, swap1 + length);
  int subroutetime1 = route1->getSubRouteTime(swap1, swap1 + length);

  float subroutecost2 = route2->getSubRouteCost(swap2, swap2 + length);
  int subroutetime2 = route2->getSubRouteTime(swap2, swap2 + length);

  // Node *previus_node1 = new Node(*route1->rute_nodes.at(swap1-1));
  // Node *first_node1 = new Node(*route1->rute_nodes.at(swap1));
  // Node *last_node1 = new Node(*route1->rute_nodes.at(swap1+length));
  // Node *next_node1 = new Node(*route1->rute_nodes.at(swap1+length+1));

  shared_ptr<Node> previus_node1(new Node(*route1->rute_nodes.at(swap1 - 1)));
  shared_ptr<Node> first_node1(new Node(*route1->rute_nodes.at(swap1)));
  shared_ptr<Node> last_node1(new Node(*route1->rute_nodes.at(swap1 + length)));
  shared_ptr<Node> next_node1(
      new Node(*route1->rute_nodes.at(swap1 + length + 1)));

  // Node *previus_node2 = new Node(*route2->rute_nodes.at(swap2-1));
  // Node *first_node2 = new Node(*route2->rute_nodes.at(swap2));
  // Node *last_node2 = new Node(*route2->rute_nodes.at(swap2+length));
  // Node *next_node2 = new Node(*route2->rute_nodes.at(swap2+length+1));

  shared_ptr<Node> previus_node2(new Node(*route2->rute_nodes.at(swap2 - 1)));
  shared_ptr<Node> first_node2(new Node(*route2->rute_nodes.at(swap2)));
  shared_ptr<Node> last_node2(new Node(*route2->rute_nodes.at(swap2 + length)));
  shared_ptr<Node> next_node2(
      new Node(*route2->rute_nodes.at(swap2 + length + 1)));

  daycost1 =
      daycost1 - first_node1->getCost() - next_node1->getCost() - subroutecost1;
  daycost2 =
      daycost2 - first_node2->getCost() - next_node2->getCost() - subroutecost2;

  daytime1 =
      daytime1 - first_node1->getTime() - next_node1->getTime() - subroutetime1;
  daytime2 =
      daytime2 - first_node2->getTime() - next_node2->getTime() - subroutetime2;

  changeDistance(previus_node1.get(), first_node2.get());
  changeDistance(previus_node2.get(), first_node1.get());

  changeDistance(last_node1.get(), next_node2.get());
  changeDistance(last_node2.get(), next_node1.get());

  daycost1 =
      daycost1 + first_node2->getCost() + next_node1->getCost() + subroutecost2;
  daycost2 =
      daycost2 + first_node1->getCost() + next_node2->getCost() + subroutecost1;

  daytime1 =
      daytime1 + first_node2->getTime() + next_node1->getTime() + subroutetime2;
  daytime2 =
      daytime2 + first_node1->getTime() + next_node2->getTime() + subroutetime1;

  if (daytime1 < this->problemInstance->getworktime() &&
      daytime2 < this->problemInstance->getworktime()) {
    if (daycost1 + daycost2 <
        route1->getRouteCost(day1) + route2->getRouteCost(day2)) {

      if (route1->rute_nodes.at(0) == route2->rute_nodes.at(0)) {

        swap_ranges(route1->rute_nodes.begin() + swap1,
                    route1->rute_nodes.begin() + swap1 + length + 1,
                    route1->rute_nodes.begin() + swap2);

        // swap_ranges(route2->rute_nodes.begin()+swap1,route2->rute_nodes.begin()+swap1+length+1,route2->rute_nodes.begin()+swap2);

      } else {

        swap_ranges(route1->rute_nodes.begin() + swap1,
                    route1->rute_nodes.begin() + swap1 + length + 1,
                    route2->rute_nodes.begin() + swap2);
      }

      changeDistance(route1->rute_nodes.at(swap1 - 1),
                     route1->rute_nodes.at(swap1));
      changeDistance(route1->rute_nodes.at(swap1 + length),
                     route1->rute_nodes.at(swap1 + length + 1));

      changeDistance(route2->rute_nodes.at(swap2 - 1),
                     route2->rute_nodes.at(swap2));
      changeDistance(route2->rute_nodes.at(swap2 + length),
                     route2->rute_nodes.at(swap2 + length + 1));

      route1->setRouteTime(day1, daytime1);
      route2->setRouteTime(day2, daytime2);

      route1->setRouteCost(day1, daycost1);
      route2->setRouteCost(day2, daycost2);

      route1->setTotalRouteCost();
      route2->setTotalRouteCost();

      return true;
    }
    return false;
  }
  return false;
}

void Solution::changeRoutesNodeDay() {
  bool debug = false;
  int cont = 0;
  bool best_solution = false;
  int nodes;

  while (!best_solution) {
    best_solution = changeRoutesDayIteration();
    if (debug) cout << "Saliendo de changeRoutesDayIteration() " << endl;
    this->check();
    cont++;
  }
  
}

bool Solution::changeRoutesDayIteration() {
  bool debug = false;
  if (debug) cout << "Entrando a : changeRoutesDayIteration() " << endl;
  
  int sub_route = 0;
  int max_sub = 3;

  bool best_solution = true;

  int origin_day = 0;
  int origin_swap = 0;
  int destiny_day = 0;
  int destiny_swap = 0;

  bool change_day = false;

  while (sub_route < max_sub) {
    origin_day = 0;
    origin_swap = 0;
    destiny_day = 0;
    destiny_swap = 0;
    change_day = false;
    for (Team *t1 : this->Teams) {
      Route *destiny = t1->route;
      for (Node *n : destiny->rute_nodes) {

        if (n->getNodeType() == 0 && destiny_day == 0) {
          destiny_swap++;
          continue;
        }
        if (n->getNodeType() == 2) {
          change_day = true;
        }

        for (Team *t2 : this->Teams) {          
          Route *origin = t2->route;
          
          if (origin->rute_nodes.size() - (sub_route + 1) < 3) //La ruta origen no puede quedar vacía. Toma en consideración el valor de sub_route, en caso de que se pueda sacar al menos un puente.
              continue;

          for (Node *m : origin->rute_nodes) {
            if (isAccommodation(origin, origin_swap, sub_route + 1)) {
              if (m->getNodeType() == 2) {
                origin_day++;
              }
              origin_swap++;
              continue;
            }

            else if (m->getNodeType() == 0) {
              origin_swap++;
              continue;
            }

            else if (isDepot(origin, origin_swap, sub_route + 1)) {
              continue;
            }
            if (origin->rute_nodes.at(0)->getId() == destiny->rute_nodes.at(0)->getId() && origin_day == destiny_day) {
              origin_swap++;
              continue;
            }
            best_solution = addToDay(origin, origin_swap, origin_day, destiny, destiny_swap, destiny_day, sub_route);
            if (debug) cout << "after addToDay" << endl;
            this->check();
            origin_swap++;
            if (best_solution) {
              return false;
            }
          }
          origin_day = 0;
          origin_swap = 0;
        }
        destiny_swap++;
        if (change_day) {
          destiny_day++;
          change_day = false;
        }
      }
      destiny_day = 0;
      destiny_swap = 0;
    }
    sub_route++;
  }
  return true;
}

bool Solution::addToDay(Route *origin_route, int origin_swap, int origin_day, Route *destiny_route, int destiny_swap, int destiny_day, int sub_route) {
  
  bool debug = false;

  if (debug) cout << "Entrando a addToDay " << endl;
  if (debug) cout << "origin_swap: " << origin_swap << ", origin_day: " << origin_day << ", destiny_swap: " << destiny_swap << ", destiny_day: " << destiny_day << ", sub_route: "  << sub_route << endl;

  if (debug) origin_route->printRoute();
  if (debug) destiny_route->printRoute();
  
  float origin_daycost = origin_route->getRouteCost(origin_day);
  int origin_daytime = origin_route->getRouteTime(origin_day);

  float destiny_daycost = destiny_route->getRouteCost(destiny_day);
  int destiny_daytime = destiny_route->getRouteTime(destiny_day);
  
  if (debug) cout << "origin_daycost: " << origin_daycost << ",  origin_daytime: " << origin_daytime << endl;
  if (debug) cout << "destiny_daycost: " << destiny_daycost << ",  destiny_daytime: " << destiny_daytime << endl;
  
  Route *temp_route = new Route(*origin_route);
  
  bool delete_acomodation = true;
  bool flag_temp = false;
  int new_origin_swap = origin_swap;

  Node *previus_destiny = new Node(*destiny_route->rute_nodes.at(destiny_swap - 1));
  Node *next_destiny = new Node(*destiny_route->rute_nodes.at(destiny_swap));

  Node *previus_origin = new Node(*origin_route->rute_nodes.at(origin_swap - 1));
  Node *first_origin = new Node(*origin_route->rute_nodes.at(origin_swap));
  Node *last_origin = new Node(*origin_route->rute_nodes.at(origin_swap + sub_route));
  Node *next_origin = new Node(*origin_route->rute_nodes.at(origin_swap + sub_route + 1));

  float origin_subroutecost = origin_route->getSubRouteCost(origin_swap, origin_swap + sub_route);
  int origin_subroutetime =  origin_route->getSubRouteTime(origin_swap, origin_swap + sub_route);

  origin_daycost = origin_daycost - first_origin->getCost() - next_origin->getCost() - origin_subroutecost;
  origin_daytime = origin_daytime - first_origin->getTime() - next_origin->getTime() - origin_subroutetime;

  destiny_daycost -= next_destiny->getCost();
  destiny_daytime -= next_destiny->getTime();

  if (previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1) {
    changeDistance(previus_origin, next_origin);
    delete_acomodation = false;
    origin_daycost += next_origin->getCost();
    origin_daytime += next_origin->getTime();
  } else {
    delete_acomodation = true;
  }

  changeDistance(previus_destiny, first_origin);
  changeDistance(last_origin, next_destiny);

  destiny_daycost = destiny_daycost + first_origin->getCost() + next_destiny->getCost() + origin_subroutecost;
  destiny_daytime = destiny_daytime + first_origin->getTime() + next_destiny->getTime() + origin_subroutetime;

  if (destiny_daytime <= this->problemInstance->getworktime() && origin_daytime <= this->problemInstance->getworktime()) {
    if (debug) cout << "feasible " << endl;

    if (delete_acomodation) {
      if (debug) cout << "delete_acomodation " << endl;
      
      if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId()) {
        if(debug) cout << "Same routes! " << endl;

        changeDistance(temp_route->rute_nodes.at(destiny_swap - 1), temp_route->rute_nodes.at(origin_swap));
        changeDistance(temp_route->rute_nodes.at(origin_swap + sub_route), temp_route->rute_nodes.at(destiny_swap));
        
        if (debug) cout << "Antes del insert! " << endl;
        if (debug) cout << "new_origin_swap: " << new_origin_swap << endl;
        if (debug) temp_route->rute_nodes.at(destiny_swap)->printAll();
        if (debug) temp_route->rute_nodes.at(new_origin_swap)->printAll();
        if (debug) temp_route->rute_nodes.at(new_origin_swap + sub_route + 1)->printAll();
                
        if (destiny_swap < origin_swap){ //Se van moviendo al insertar en destiny  
            for(int pos=0, displacement=0; pos<=sub_route; pos++){ // TODO: Revisar que se usa correctamente
                Node *node_temp = new Node(*temp_route->rute_nodes.at(new_origin_swap + pos + displacement));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                temp_route->rute_nodes.insert(temp_route->rute_nodes.begin() + destiny_swap+pos, node_temp);
                displacement++;
                if(debug) temp_route->printRoute();
            }
        }
        else{
            for(int pos=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*temp_route->rute_nodes.at(new_origin_swap + pos));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                temp_route->rute_nodes.insert(temp_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
            }
        }
        
        
//         temp_route->rute_nodes.insert(temp_route->rute_nodes.begin() + destiny_swap, temp_route->rute_nodes.begin() + new_origin_swap, temp_route->rute_nodes.begin() + new_origin_swap + sub_route + 1); // Inserción

        if (debug) cout << "posterior a la inserción" << endl;
        if (debug) temp_route->printRoute();
        
        
        if (destiny_swap < origin_swap) { // al insertar antes en la misma ruta se desplazan los nodos a borrar a la derecha
          new_origin_swap += sub_route + 1;
        }        
        
        if (debug) cout << "Antes del erase! " << endl;
        if (debug) cout << "new_origin_swap: " << new_origin_swap << endl;
        if (debug) temp_route->rute_nodes.at(new_origin_swap)->printAll();
        if (debug) temp_route->rute_nodes.at(new_origin_swap + sub_route + 1)->printAll();

        temp_route->rute_nodes.erase(temp_route->rute_nodes.begin() + new_origin_swap, temp_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);
        
        if (debug) cout << "posterior al borrado" << endl;
        if (debug) temp_route->printRoute();
        
        this->deleteAccommodation(temp_route, new_origin_swap - 1);
        
        if (debug) cout << "posterior al borrado del hotel" << endl;
        if (debug )temp_route->printRoute();


        for (int new_time : temp_route->daytime) {
          if (debug) cout << "new_time: " << new_time  << ", this->problemInstance->getworktime(): " << this->problemInstance->getworktime() << endl;
          if (new_time > this->problemInstance->getworktime()) {
            if (debug) cout << "new_time > this->problemInstance->getworktime()!" << endl;
            
            delete temp_route;
            delete previus_destiny;
            delete next_destiny;
            delete previus_origin;
            delete first_origin;
            delete last_origin;
            delete next_origin;
            return false;
          }
        }
        
        
        origin_route->setTotalRouteCost();
        if (temp_route->getRouteCost() < origin_route->getRouteCost()) {
          
          new_origin_swap = origin_swap;
          changeDistance(origin_route->rute_nodes.at(destiny_swap - 1), origin_route->rute_nodes.at(origin_swap));
          changeDistance(origin_route->rute_nodes.at(origin_swap + sub_route), origin_route->rute_nodes.at(destiny_swap));

          
//           origin_route->rute_nodes.insert( origin_route->rute_nodes.begin() + destiny_swap, origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);
          
        if (destiny_swap < origin_swap){ //Se van moviendo al insertar en destiny  
            for(int pos=0, displacement=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos + displacement));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                origin_route->rute_nodes.insert(origin_route->rute_nodes.begin() + destiny_swap+pos, node_temp);
                displacement++;
                if(debug) origin_route->printRoute();
            }
        }
        else{
            for(int pos=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                origin_route->rute_nodes.insert(origin_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
            }
        }
          
          if (destiny_swap < origin_swap) {
              new_origin_swap += sub_route + 1;
          }
          
          origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);

          this->deleteAccommodation(origin_route, new_origin_swap - 1);
          if (debug) origin_route->printRoute();

          delete temp_route;
          delete previus_destiny;
          delete next_destiny;
          delete previus_origin;
          delete first_origin;
          delete last_origin;
          delete next_origin;

          return true;
        } else {
          if (debug) cout << "else again! " << endl;
          if (debug) origin_route->printRoute();
          if (debug) destiny_route->printRoute();
          delete temp_route;
          delete previus_destiny;
          delete next_destiny;
          delete previus_origin;
          delete first_origin;
          delete last_origin;
          delete next_origin;

          return false;
        }
      }
      else {
        if (debug) this->printSmall();
        if (debug) cout << "no son las mismas rutas. " << endl;
        
        if (debug) temp_route->printRoute();
        temp_route->rute_nodes.erase(temp_route->rute_nodes.begin() + origin_swap, temp_route->rute_nodes.begin() + origin_swap + sub_route + 1);
        if (debug) temp_route->printRoute();
        this->deleteAccommodation(temp_route, origin_swap - 1);

        for (int new_time : temp_route->daytime) {
          if (new_time > this->problemInstance->getworktime()) {
            delete temp_route;
            delete previus_destiny;
            delete next_destiny;
            delete previus_origin;
            delete first_origin;
            delete last_origin;
            delete next_origin;

            return false;
          }
        }

        if (debug) cout << "here again" << endl;
        origin_route->setTotalRouteCost();

        if (temp_route->getRouteCost() + destiny_daycost < origin_route->getRouteCost() + destiny_route->getRouteCost(destiny_day)) {

          changeDistance(destiny_route->rute_nodes.at(destiny_swap - 1), origin_route->rute_nodes.at(origin_swap));
          changeDistance(origin_route->rute_nodes.at(origin_swap + sub_route), destiny_route->rute_nodes.at(destiny_swap));

          destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap, origin_route->rute_nodes.begin() + origin_swap,          origin_route->rute_nodes.begin() + origin_swap + sub_route + 1);
          origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + origin_swap, origin_route->rute_nodes.begin() + origin_swap + sub_route + 1);

          destiny_route->setRouteTime(destiny_day, destiny_daytime);
          destiny_route->setRouteCost(destiny_day, destiny_daycost);

          this->deleteAccommodation(origin_route, origin_swap - 1);

          delete temp_route;
          delete previus_destiny;
          delete next_destiny;
          delete previus_origin;
          delete first_origin;
          delete last_origin;
          delete next_origin;

          return true;
        } else {
          delete temp_route;
          delete previus_destiny;
          delete next_destiny;
          delete previus_origin;
          delete first_origin;
          delete last_origin;
          delete next_origin;

          return false;
        }
      }
    } else {
      if (debug) cout << " no delete accomodation " << endl;
      if (origin_daycost + destiny_daycost < origin_route->getRouteCost(origin_day) + destiny_route->getRouteCost(destiny_day)) {

        changeDistance(destiny_route->rute_nodes.at(destiny_swap - 1), origin_route->rute_nodes.at(origin_swap));
        changeDistance(origin_route->rute_nodes.at(origin_swap + sub_route), destiny_route->rute_nodes.at(destiny_swap));

        changeDistance(origin_route->rute_nodes.at(origin_swap - 1), origin_route->rute_nodes.at(origin_swap + sub_route + 1));

        //destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap, origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);
        
        if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId() && destiny_swap < origin_swap){ //Se van moviendo al insertar en destiny  
            for(int pos=0, displacement=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos + displacement));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap+pos, node_temp);
                displacement++;
                if(debug) destiny_route->printRoute();
            }
        }
        else{
            for(int pos=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
            }
        }

        if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId() && destiny_swap < origin_swap) {
          new_origin_swap += sub_route + 1;
        }

        if(debug) origin_route->printRoute();
        if(debug) destiny_route->printRoute();
        
        origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);
        
        if(debug) origin_route->printRoute();
        if(debug) destiny_route->printRoute();

        destiny_route->setRouteTime(destiny_day, destiny_daytime);
        destiny_route->setRouteCost(destiny_day, destiny_daycost);

        origin_route->setRouteTime(origin_day, origin_daytime);
        origin_route->setRouteCost(origin_day, origin_daycost);

        origin_route->setRouteCost();
        origin_route->setTotalRouteCost();
        origin_route->setRouteTime();

        destiny_route->setRouteCost();
        destiny_route->setTotalRouteCost();
        destiny_route->setRouteTime();

        delete temp_route;
        delete previus_destiny;
        delete next_destiny;
        delete previus_origin;
        delete first_origin;
        delete last_origin;
        delete next_origin;

        return true;
      } else {
        delete temp_route;
        delete previus_destiny;
        delete next_destiny;
        delete previus_origin;
        delete first_origin;
        delete last_origin;
        delete next_origin;

        return false;
      }
    }
  }
  delete temp_route;
  delete previus_destiny;
  delete next_destiny;
  delete previus_origin;
  delete first_origin;
  delete last_origin;
  delete next_origin;

  return false;
}
/*
void Solution::changeRoutesNodeNewDay() {

  int origin_day = 0;
  int origin_swap = 0;
  bool change = false;

  for (Team *t1 : this->Teams) {
    Route *destiny = t1->route;

    for (Team *t2 : this->Teams) {
      Route *origin = t2->route;

      for (Node *n : origin->rute_nodes) {

        if (n->getNodeType() == 2) {
          origin_day++;
          origin_swap++;
          continue;
        } else if (n->getNodeType() == 0) {
          origin_swap++;
          continue;
        }

        change = addToNewDay(origin, origin_swap, origin_day, destiny);
        origin_swap++;

        if (change) {
          return;
        }
      }
      origin_swap = 0;
      origin_day = 0;
    }
  }
}*/
/*
bool Solution::addToNewDay(Route *origin_route, int swap, int origin_day, Route *destiny_route) {

  int lastworktime, daysworked;
  float daycost;

  float origin_daycost, destiny_daycost, new_daycost;
  int origin_daytime, destiny_daytime, new_daytime;

  daysworked = destiny_route->getRouteDays();

  if (daysworked < this->problemInstance->getDays() - 1) {

    // shared_ptr<Route> Aux(new Route(*destiny_route));
    Route *Aux = new Route(*destiny_route);

    // shared_ptr<Node> previus_origin(new
    // Node(*origin_route->rute_nodes.at(swap - 1))); shared_ptr<Node>
    // actual_node(new Node(*origin_route->rute_nodes.at(swap)));
    // shared_ptr<Node> next_origin(new Node(*origin_route->rute_nodes.at(swap +
    // 1)));

    Node *previus_origin = new Node(*origin_route->rute_nodes.at(swap - 1));
    Node *actual_node = new Node(*origin_route->rute_nodes.at(swap));
    Node *next_origin = new Node(*origin_route->rute_nodes.at(swap + 1));

    origin_daycost = origin_route->getRouteCost(origin_day);
    origin_daytime = origin_route->getRouteTime(origin_day);

    if (previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1) {

      origin_daycost =
          origin_daycost - actual_node->getCost() - next_origin->getCost();
      origin_daytime =
          origin_daytime - actual_node->getTime() - next_origin->getTime();

      changeDistance(previus_origin, next_origin);

      origin_daycost += next_origin->getCost();
      origin_daytime += next_origin->getTime();

    }

    else {

      delete Aux;

      delete previus_origin;
      delete actual_node;
      delete next_origin;
      return false;
    }

    destiny_daycost = destiny_route->getRouteTime(daysworked - 1);
    destiny_daytime = destiny_route->getRouteTime(daysworked - 1);

    destiny_daycost -= destiny_route->rute_nodes.back()->getCost();
    destiny_daytime -= destiny_route->rute_nodes.back()->getTime();

    Node *remove = Aux->rute_nodes.back();
    Aux->rute_nodes.pop_back();
    delete remove;

    this->selectAccommodation(Aux);

    destiny_daycost += Aux->rute_nodes.back()->getCost();
    destiny_daytime += Aux->rute_nodes.back()->getTime();

    changeDistance(Aux->rute_nodes.back(), actual_node);
    Aux->AddNode(actual_node);

    returnToDepot(Aux);

    new_daycost = actual_node->getCost() + Aux->rute_nodes.back()->getCost();
    new_daytime = actual_node->getTime() + Aux->rute_nodes.back()->getTime();

    delete Aux;

    delete previus_origin;
    delete next_origin;

    if (origin_daycost + destiny_daycost + new_daycost <
        origin_route->getRouteCost(origin_day) +
            destiny_route->getRouteTime(daysworked - 1)) {
      destiny_route->rute_nodes.pop_back();
      this->selectAccommodation(destiny_route);

      changeDistance(destiny_route->rute_nodes.back(),
                     origin_route->rute_nodes.at(swap));

      destiny_route->AddNode(origin_route->rute_nodes.at(swap));
      returnToDepot(destiny_route);

      this->deleteNode(origin_route, swap, origin_day);

      destiny_route->setRouteCost();
      destiny_route->setRouteTime();
      destiny_route->setTotalRouteCost();

      origin_route->setRouteCost();
      origin_route->setTotalRouteCost();
      origin_route->setRouteTime();

      // cout << "Ruta destiny modificada" << endl;
      // destiny_route->printRoute();
      // cout << endl;

      // cout << "Ruta origin modificada" << endl;
      // origin_route->printRoute();
      // cout << endl;

      return true;

    } else {
      return false;
    }

  } else {
    return false;
  }
}
*/

void Solution::changeRoutesNewDay() {

  int sub_route = 0;
  int max_sub = 3;

  int origin_day = 0;
  int origin_swap = 0;
  bool change = false;

  while (sub_route < max_sub) {

    for (Team *t1 : this->Teams) {
      Route *destiny = t1->route;

      for (Team *t2 : this->Teams) {
        Route *origin = t2->route;
        
        if (origin->rute_nodes.size() - (sub_route + 1) < 3) //La ruta origen no puede quedar vacía. Toma en consideración el valor de sub_route, en caso de que se pueda sacar al menos un puente.
            continue;

        for (Node *n : origin->rute_nodes) {

          if (isAccommodation(origin, origin_swap, sub_route + 1)) {
            if (n->getNodeType() == 2) {
              origin_day++;
            }
            origin_swap++;
            continue;
          }

          else if (n->getNodeType() == 0) {
            origin_swap++;
            continue;
          }

          else if (isDepot(origin, origin_swap, sub_route + 1)) {
            continue;
          }

          change = addToNewDay(origin, origin_swap, origin_day, destiny, sub_route);
          this->check();
          origin_swap++;

          if (change) {
            return;
          }
        }
        origin_swap = 0;
        origin_day = 0;
      }
    }
    sub_route++;
  }
}

bool Solution::addToNewDay(Route *origin_route, int swap, int origin_day, Route *destiny_route, int sub_route) {
  bool debug = false;
  if (debug) cout << "Entrando a addToNewDay " << endl;
  if (debug) cout << "swap: " << swap << ", origin_day: " << origin_day << ", sub_route: " << sub_route << endl;
  if (debug) origin_route->printRoute();
  if (debug) destiny_route->printRoute();
  
  int lastworktime, daysworked;
  float daycost;

  float origin_daycost, destiny_daycost, new_daycost;
  int origin_daytime, destiny_daytime, new_daytime;
  int new_swap = swap;

  daysworked = destiny_route->getRouteDays();

  if (daysworked < this->problemInstance->getDays()) {
    if (debug) cout << "Menos días que el máximo" << endl;

    Route *Aux = new Route(*destiny_route);
    bool delete_acomodation = false;

    Node *previus_origin = new Node(*origin_route->rute_nodes.at(swap - 1));
    Node *first_origin = new Node(*origin_route->rute_nodes.at(swap));
    Node *last_origin = new Node(*origin_route->rute_nodes.at(swap + sub_route));
    Node *next_origin = new Node(*origin_route->rute_nodes.at(swap + sub_route + 1));

    if (debug) previus_origin->printAll();
    if (debug) first_origin->printAll();
    if (debug) last_origin->printAll();
    if (debug) next_origin->printAll();
    
    float origin_subroutecost = origin_route->getSubRouteCost(swap, swap + sub_route);
    int origin_subroutetime = origin_route->getSubRouteTime(swap, swap + sub_route);

    origin_daycost = origin_route->getRouteCost(origin_day);
    origin_daytime = origin_route->getRouteTime(origin_day);

    origin_daycost = origin_daycost - first_origin->getCost() - next_origin->getCost() - origin_subroutecost;
    origin_daytime = origin_daytime - first_origin->getTime() - next_origin->getTime() - origin_subroutetime;

    destiny_daycost = destiny_route->getRouteTime(daysworked - 1);
    destiny_daytime = destiny_route->getRouteTime(daysworked - 1);

    destiny_daycost -= destiny_route->rute_nodes.back()->getCost();
    destiny_daytime -= destiny_route->rute_nodes.back()->getTime();

    if (debug) Aux->printRoute();
    
    Node *remove = Aux->rute_nodes.back();
    Aux->rute_nodes.pop_back();
    delete remove;

    this->selectAccommodation(Aux);
    
    if (previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1) {
      if (debug) cout  << "previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1 " << endl;
      if (origin_route == destiny_route)
        next_origin = Aux->rute_nodes.at(swap + sub_route + 1);
      if (debug) next_origin->printAll();
      changeDistance(previus_origin, next_origin);
      delete_acomodation = false;

      origin_daycost += next_origin->getCost();
      origin_daytime += next_origin->getTime();
      
      if (origin_route == destiny_route)
        next_origin = nullptr; // Este puede estar asociado a Aux.
    }
    else {
      if (debug) cout  << "delete_acomodation = true" << endl;
      delete_acomodation = true;
    }

    destiny_daycost += Aux->rute_nodes.back()->getCost();
    destiny_daytime += Aux->rute_nodes.back()->getTime();

    changeDistance(Aux->rute_nodes.back(), first_origin);

    Aux->AddNode(first_origin);

    for (int i = 1; i < sub_route + 1; i++) {
      Aux->AddNode(new Node(*origin_route->rute_nodes.at(swap + i)));
    }

    returnToDepot(Aux);

    new_daycost = first_origin->getCost() + Aux->rute_nodes.back()->getCost() + origin_subroutecost;
    new_daytime = first_origin->getTime() + Aux->rute_nodes.back()->getTime() + origin_subroutetime;
    


    if (delete_acomodation) {
      if (debug) cout << "delete_acomodation" << endl;
      if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId()) {
          if (debug) cout << "las rutas son las mismas" << endl;

          Aux->rute_nodes.erase(Aux->rute_nodes.begin() + swap, Aux->rute_nodes.begin() + swap + sub_route + 1);
          this->deleteAccommodation(Aux, swap - 1);

          for (int new_time : Aux->daytime) {
            if (new_time > this->problemInstance->getworktime()) {
              delete Aux;
              delete previus_origin;
              delete next_origin;
              delete last_origin;
              return false;
            }
          }
          origin_route->setTotalRouteCost();

          if (Aux->getRouteCost() < origin_route->getRouteCost()) {
          

          delete Aux;
          delete previus_origin;
          delete next_origin;
          delete last_origin;

          destiny_route->rute_nodes.pop_back();
          this->selectAccommodation(destiny_route);

          changeDistance(destiny_route->rute_nodes.back(), origin_route->rute_nodes.at(swap));

          destiny_route->AddNode(origin_route->rute_nodes.at(swap));

          for (int i = 1; i < sub_route + 1; i++) {
            destiny_route->AddNode(origin_route->rute_nodes.at(swap + i));
          }
          returnToDepot(destiny_route);

          origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap, origin_route->rute_nodes.begin() + swap + sub_route + 1);
          this->deleteAccommodation(origin_route, swap - 1);

          destiny_route->setRouteCost();
          destiny_route->setRouteTime();
          destiny_route->setTotalRouteCost();

          origin_route->setRouteCost();
          origin_route->setTotalRouteCost();
          origin_route->setRouteTime();
          return true;

        }

        else {
          delete Aux;
          delete previus_origin;
          delete next_origin;
          delete last_origin;

          return false;
        }
      }

      else {
          
        //TODO: Checar si hay algún caso en que el tiempo pueda cambiar acá.
        if (debug) cout << "REVISAR ----------------------------------------------> new_daytime: " << new_daytime << endl;
        if(new_daytime > this->problemInstance->getworktime()){
            delete Aux;
            delete previus_origin;
            delete next_origin;
            delete last_origin;
            return false;
        }   
          
        Route *origin_Aux = new Route(*origin_route);
        origin_Aux->rute_nodes.erase(origin_Aux->rute_nodes.begin() + swap, origin_Aux->rute_nodes.begin() + swap + sub_route + 1);
        this->deleteAccommodation(origin_Aux, swap - 1);

        for (int new_time : origin_Aux->daytime) {
          if (new_time > this->problemInstance->getworktime()) {
            delete Aux;
            delete origin_Aux;
            delete previus_origin;
            delete next_origin;
            delete last_origin;
            return false;
          }
        }

        origin_route->setTotalRouteCost();

        if (origin_Aux->getRouteCost() + destiny_daycost + new_daycost < origin_route->getRouteCost() + destiny_route->getRouteCost(daysworked - 1)) {
         //mejor.
          destiny_route->rute_nodes.pop_back();
          this->selectAccommodation(destiny_route);

          changeDistance(destiny_route->rute_nodes.back(), origin_route->rute_nodes.at(swap));

          destiny_route->AddNode(origin_route->rute_nodes.at(swap));

          for (int i = 1; i < sub_route + 1; i++) {
            destiny_route->AddNode(origin_route->rute_nodes.at(swap + i));
          }
          returnToDepot(destiny_route);

          origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap, origin_route->rute_nodes.begin() + swap + sub_route + 1);
          this->deleteAccommodation(origin_route, swap - 1);

          delete Aux;
          delete origin_Aux;
          delete previus_origin;
          delete next_origin;
          delete last_origin;

          destiny_route->setRouteCost();
          destiny_route->setRouteTime();
          destiny_route->setTotalRouteCost();

          origin_route->setRouteCost();
          origin_route->setTotalRouteCost();
          origin_route->setRouteTime();

          return true;

        }

        else {
          delete Aux;
          delete origin_Aux;
          delete previus_origin;
          delete next_origin;
          delete last_origin;
          return false;
        }
      }

    }
    else {
        if (debug) cout << "Else  -- delete_acomodation" << endl;
        
      delete Aux;
      delete previus_origin;
      delete next_origin;
      delete last_origin;
      
      if(new_daytime > this->problemInstance->getworktime()){ //TODO: Checar si hay algún caso en que el tiempo pueda cambiar acá.
        return false;
      }
      
      if (origin_daycost + destiny_daycost + new_daycost < origin_route->getRouteCost(origin_day) + destiny_route->getRouteCost(daysworked - 1)) {
          
        if (debug) cout << "Better" << endl;
        destiny_route->rute_nodes.pop_back();
        this->selectAccommodation(destiny_route);

        changeDistance(destiny_route->rute_nodes.back(), origin_route->rute_nodes.at(swap));

        destiny_route->AddNode(origin_route->rute_nodes.at(swap));

        for (int i = 1; i < sub_route + 1; i++) {
          destiny_route->AddNode(origin_route->rute_nodes.at(swap + i));
        }

        returnToDepot(destiny_route);

        origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap, origin_route->rute_nodes.begin() + swap + sub_route + 1);
        
        changeDistance(origin_route->rute_nodes.at(swap-1), origin_route->rute_nodes.at(swap));

        destiny_route->setRouteCost();
        destiny_route->setRouteTime();
        destiny_route->setTotalRouteCost();

        origin_route->setRouteCost();
        origin_route->setTotalRouteCost();
        origin_route->setRouteTime();

        return true;

      } else {
        if (debug) cout << "else" << endl;
        return false;
      }
    }

  } else {
    if (debug) cout << "maximo de días? " << endl;
    return false;
  }
}

void Solution::changeRoutesAccommodation() {

  int swap = 0;
  int day = 0;

  for (Team *t : this->Teams) {
    for (Node *n : t->route->rute_nodes) {
      if (n->getNodeType() == 2) {
        day++;
        changeAccommodation(t->route, swap, day);
      }
      swap++;
    }
    swap = 0;
    day = 0;
  }
}

void Solution::DisturbRoutes2opt() {

  int day_count = 0;
  int initial_node, final_node;
  int count = 0;
  bool flag = true;

  int selected_team =
      random_int_number(0, this->problemInstance->getTotalDepots() - 1);

  Route *t = this->Teams.at(selected_team)->route;

  int selected_day = random_int_number(0, t->getRouteDays() - 1);

  for (Node *n : t->rute_nodes) {

    if (n->getNodeType() == 2 || n->getNodeType() == 0) {
      if (flag) {
        initial_node = count;
        flag = false;
      } else {
        final_node = count;
        if (day_count == selected_day) {
          DisturbtwoOpt(t, initial_node, final_node, day_count);
          break;
        }
        day_count++;
        initial_node = count;
      }
    }
    count++;
  }

  t->setTotalRouteCost();
}

void Solution::DisturbtwoOpt(Route *actual_route, int initial_node,
                             int final_node, int day_count) {
  vector<Node *> route(actual_route->rute_nodes.begin() + initial_node,
                       actual_route->rute_nodes.begin() + final_node + 1);
  bool debug = false;
  Route *new_route = new Route(route);

  int node_count;
  int size = new_route->rute_nodes.size();

  if (size < 4) {
    delete new_route;
    return;
  }

  int i = random_int_number(1, size - 3);
  int k = random_int_number(i + 1, size - 2);

  twoOptSwap(new_route, i, k);

  if (new_route->timefeasible(this->problemInstance->getworktime())) {

    node_count = 0;
    for (int it = initial_node; it <= final_node; it++) {

      delete actual_route->rute_nodes.at(it);
      actual_route->rute_nodes.at(it) = new_route->rute_nodes.at(node_count);
      if (debug) actual_route->rute_nodes.at(it)->printAll();
      node_count++;
    }
    if (debug) cout << "Copia finalizada" << endl;

    actual_route->setRouteCost(day_count, new_route->getRouteCost());
    actual_route->setRouteTime(day_count, new_route->getRouteTime(0));
    return;
  }

  delete new_route;
}

void Solution::DisturbRoutesRouteSwap() {

  int sub_route = random_int_number(0, 2);
  int selected_destiny_team =
      random_int_number(0, this->problemInstance->getTotalDepots() - 1);
  int selected_origin_team =
      random_int_number(0, this->problemInstance->getTotalDepots() - 1);

  Route *destiny = this->Teams.at(selected_destiny_team)->route;
  Route *origin = this->Teams.at(selected_origin_team)->route;

  int destiny_swap = random_int_number(1, destiny->rute_nodes.size() - 2);
  int origin_swap = random_int_number(1, origin->rute_nodes.size() - 2);

  if (destiny->rute_nodes.at(destiny_swap)->getNodeType() == 2) {
    destiny_swap++;
  }

  if (origin->rute_nodes.at(origin_swap)->getNodeType() == 2) {
    origin_swap++;
  }

  for (int i = 1; i < sub_route + 1; i++) {
    if (destiny->rute_nodes.at(destiny_swap + i)->getNodeType() == 2 ||
        destiny->rute_nodes.at(destiny_swap + i)->getNodeType() == 0) {
      if (i - 1 < sub_route) {
        sub_route = i - 1;
        break;
      }
    }
    if (origin->rute_nodes.at(origin_swap + i)->getNodeType() == 2 ||
        origin->rute_nodes.at(origin_swap + i)->getNodeType() == 0) {
      if (i - 1 < sub_route) {
        sub_route = i - 1;
        break;
      }
    }
  }

  int destiny_day = destiny->getNodeDay(destiny_swap);
  int origin_day = origin->getNodeDay(origin_swap);

  float best_solution =
      DisturbrouteSwap(origin, origin_swap, origin_day, destiny, destiny_swap,
                       destiny_day, sub_route);
}

bool Solution::DisturbrouteSwap(Route *route1, int swap1, int day1,
                                Route *route2, int swap2, int day2,
                                int length) {
  bool debug = false;
  if (debug) cout << " ============================== DisturbrouteSwap ===============================" << endl;
  if (debug && route1 == route2) cout << "Las rutas son la misma!" << endl;
  if (debug) cout << "Swap1: "  << swap1 << ", day1 : "  << day1 << endl;
  if (debug) cout << "Swap2: "  << swap2 << ", day2 : "  << day2 << endl;
  if (debug) cout << "length: "  << length << endl;
  
  if (route1 == route2 && day1 == day2)
      return false;
  
  float daycost1 = route1->getRouteCost(day1);
  float daycost2 = route2->getRouteCost(day2);
  int daytime1 = route1->getRouteTime(day1);
  int daytime2 = route2->getRouteTime(day2);

  float subroutecost1 = route1->getSubRouteCost(swap1, swap1 + length);
  int subroutetime1 = route1->getSubRouteTime(swap1, swap1 + length);

  float subroutecost2 = route2->getSubRouteCost(swap2, swap2 + length);
  int subroutetime2 = route2->getSubRouteTime(swap2, swap2 + length);

  // Node *previus_node1 = new Node(*route1->rute_nodes.at(swap1-1));
  // Node *first_node1 = new Node(*route1->rute_nodes.at(swap1));
  // Node *last_node1 = new Node(*route1->rute_nodes.at(swap1+length));
  // Node *next_node1 = new Node(*route1->rute_nodes.at(swap1+length+1));

  shared_ptr<Node> previus_node1(new Node(*route1->rute_nodes.at(swap1 - 1)));
  shared_ptr<Node> first_node1(new Node(*route1->rute_nodes.at(swap1)));
  shared_ptr<Node> last_node1(new Node(*route1->rute_nodes.at(swap1 + length)));
  shared_ptr<Node> next_node1(
      new Node(*route1->rute_nodes.at(swap1 + length + 1)));

  // Node *previus_node2 = new Node(*route2->rute_nodes.at(swap2-1));
  // Node *first_node2 = new Node(*route2->rute_nodes.at(swap2));
  // Node *last_node2 = new Node(*route2->rute_nodes.at(swap2+length));
  // Node *next_node2 = new Node(*route2->rute_nodes.at(swap2+length+1));

  shared_ptr<Node> previus_node2(new Node(*route2->rute_nodes.at(swap2 - 1)));
  shared_ptr<Node> first_node2(new Node(*route2->rute_nodes.at(swap2)));
  shared_ptr<Node> last_node2(new Node(*route2->rute_nodes.at(swap2 + length)));
  shared_ptr<Node> next_node2(
      new Node(*route2->rute_nodes.at(swap2 + length + 1)));

  daycost1 =
      daycost1 - first_node1->getCost() - next_node1->getCost() - subroutecost1;
  daycost2 =
      daycost2 - first_node2->getCost() - next_node2->getCost() - subroutecost2;

  daytime1 =
      daytime1 - first_node1->getTime() - next_node1->getTime() - subroutetime1;
  daytime2 =
      daytime2 - first_node2->getTime() - next_node2->getTime() - subroutetime2;

  changeDistance(previus_node1.get(), first_node2.get());
  changeDistance(previus_node2.get(), first_node1.get());

  changeDistance(last_node1.get(), next_node2.get());
  changeDistance(last_node2.get(), next_node1.get());

  daycost1 =
      daycost1 + first_node2->getCost() + next_node1->getCost() + subroutecost2;
  daycost2 =
      daycost2 + first_node1->getCost() + next_node2->getCost() + subroutecost1;

  daytime1 =
      daytime1 + first_node2->getTime() + next_node1->getTime() + subroutetime2;
  daytime2 =
      daytime2 + first_node1->getTime() + next_node2->getTime() + subroutetime1;

  if (daytime1 < this->problemInstance->getworktime() &&
      daytime2 < this->problemInstance->getworktime()) {
      
     //Factible
    if (debug) cout << "Feasible" << endl;

    if (route1->rute_nodes.at(0) == route2->rute_nodes.at(0)) { // ¿tienen el mismo nodo?
    if (debug) cout << "Antes del swap_ranges " << endl;
    if (debug) route1->printRoute();
    if (debug) route2->printRoute();
      
      swap_ranges(route1->rute_nodes.begin() + swap1,
                  route1->rute_nodes.begin() + swap1 + length + 1,
                  route1->rute_nodes.begin() + swap2);

    if (debug) cout << "Despues del swap_ranges " << endl;
    if (debug) route1->printRoute();
    if (debug) route2->printRoute();

      // swap_ranges(route2->rute_nodes.begin()+swap1,route2->rute_nodes.begin()+swap1+length+1,route2->rute_nodes.begin()+swap2);

    } else {

      swap_ranges(route1->rute_nodes.begin() + swap1,
                  route1->rute_nodes.begin() + swap1 + length + 1,
                  route2->rute_nodes.begin() + swap2);
    }

    changeDistance(route1->rute_nodes.at(swap1 - 1),
                   route1->rute_nodes.at(swap1));
    changeDistance(route1->rute_nodes.at(swap1 + length),
                   route1->rute_nodes.at(swap1 + length + 1));
    
    if(length==1 && abs(swap1-swap2) == length && day1==day2 && route1 == route2) { //Esto no hace falta, probablemente solo evitemos el cambio en las condiciones especificadas de abs(swap1-swap2) respecto a lenght.
        cout <<  "-------------------------------------------------->here!"  << endl;//nothing
    }
    else{
        changeDistance(route2->rute_nodes.at(swap2 - 1),
                   route2->rute_nodes.at(swap2));
        changeDistance(route2->rute_nodes.at(swap2 + length),
                   route2->rute_nodes.at(swap2 + length + 1));
    }
    route1->setRouteTime(day1, daytime1);
    route2->setRouteTime(day2, daytime2);

    route1->setRouteCost(day1, daycost1);
    route2->setRouteCost(day2, daycost2);

    route1->setTotalRouteCost();
    route2->setTotalRouteCost();
    
    if (debug) route1->printRoute();
    if (debug) route2->printRoute();

    return true;
  }
  return false;
}

void Solution::DisturbRoutesDayIteration() {

  int sub_route, selected_destiny_team, selected_origin_team;

  Route *destiny;
  Route *origin;

    do{
      sub_route = random_int_number(0, 2);
      selected_destiny_team = random_int_number(0, this->problemInstance->getTotalDepots() - 1);
      selected_origin_team = random_int_number(0, this->problemInstance->getTotalDepots() - 1);
      destiny = this->Teams.at(selected_destiny_team)->route;
      origin = this->Teams.at(selected_origin_team)->route;
  }while((origin->rute_nodes.size() - (sub_route + 1)) < 3); //TODO: Esto se puede quedar pegado cuando hay pocas opciones.   

  
  int destiny_swap = random_int_number(1, destiny->rute_nodes.size() - 1);
  int origin_swap = random_int_number(1, origin->rute_nodes.size() - 2);

  if (origin->rute_nodes.at(origin_swap)->getNodeType() == 2) {
    origin_swap++;
//     if (origin_swap == destiny_swap){
//         cout << "pase por acá" << endl;
//         getchar();
//     }        
  }

  for (int i = 1; i < sub_route + 1; i++) {
    if (origin->rute_nodes.at(origin_swap + i)->getNodeType() == 2 ||
        origin->rute_nodes.at(origin_swap + i)->getNodeType() == 0) {
      if (i - 1 < sub_route) {
        sub_route = i - 1;
        break;
      }
    }
  }

  int destiny_day = destiny->getNodeDay(destiny_swap);
  int origin_day = origin->getNodeDay(origin_swap);

  if (destiny_day == origin_day &&
      destiny->rute_nodes.at(0)->getId() == origin->rute_nodes.at(0)->getId()) {
    return;
  }

  bool best_solution = DisturbaddToDay(origin, origin_swap, origin_day, destiny, destiny_swap, destiny_day, sub_route);
  this->check();
}

bool Solution::DisturbaddToDay(Route *origin_route, int origin_swap, int origin_day, Route *destiny_route, int destiny_swap, int destiny_day, int sub_route) {
  // remueve una subrute de largo sub_route desde la posicion origin_swap (del
  // día origin_dat) a destiny_route justo antes de la posición destiny_swap en
  // destiny_day.

  // origin_swap = posicion en origen
  // destiny_swap = posicion en destino
  bool debug = false;
  if (debug) cout << "Solution::DisturbaddToDay " << endl;

  if (debug) cout << "Tipo: " << origin_route->rute_nodes.at(origin_swap)->getNodeType() << endl;

  if (debug) origin_route->printRoute();
  if (debug) destiny_route->printRoute();
  if (debug) cout << "origin_day: " << origin_day << endl;
  if (debug) cout << "destiny_day: " << destiny_day << endl;
  if (debug) cout << "origin_swap: " << origin_swap << endl;
  if (debug) cout << "destiny_swap: " << destiny_swap << endl;
  if (debug) cout << "sub_route: " << sub_route << endl;
  
  float origin_daycost = origin_route->getRouteCost(origin_day);
  int origin_daytime = origin_route->getRouteTime(origin_day);

  if (debug) cout << "origin_daycost: " << origin_daycost << endl;
  if (debug) cout << "origin_daytime: " << origin_daytime << endl;

  float destiny_daycost = destiny_route->getRouteCost(destiny_day);
  int destiny_daytime = destiny_route->getRouteTime(destiny_day);

  if (debug) cout << "destiny_daycost: " << destiny_daycost << endl;
  if (debug) cout << "destiny_daytime: " << destiny_daytime << endl;

  // shared_ptr<Route> temp_route(new Route(*origin_route)); //estructura
  // temporal para crear

  if (debug) origin_route->printAll();
  Route *temp_route = new Route(*origin_route);

  bool delete_acomodation = true;
  bool flag_temp = false;
  int new_origin_swap = origin_swap;

  shared_ptr<Node> previus_destiny(
      new Node(*destiny_route->rute_nodes.at(destiny_swap - 1)));
  shared_ptr<Node> next_destiny(
      new Node(*destiny_route->rute_nodes.at(destiny_swap)));

  // Node *previus_destiny = new
  // Node(*destiny_route->rute_nodes.at(destiny_swap-1)); Node *next_destiny =
  // new Node(*destiny_route->rute_nodes.at(destiny_swap));

  shared_ptr<Node> previus_origin(
      new Node(*origin_route->rute_nodes.at(origin_swap - 1)));
  shared_ptr<Node> first_origin(
      new Node(*origin_route->rute_nodes.at(origin_swap)));
  shared_ptr<Node> last_origin(
      new Node(*origin_route->rute_nodes.at(origin_swap + sub_route)));
  shared_ptr<Node> next_origin(
      new Node(*origin_route->rute_nodes.at(origin_swap + sub_route + 1)));

  // Node *previus_origin = new
  // Node(*origin_route->rute_nodes.at(origin_swap-1)); Node *origin_node = new
  // Node(*origin_route->rute_nodes.at(origin_swap)); Node *next_origin = new
  // Node(*origin_route->rute_nodes.at(origin_swap+1));

  float origin_subroutecost = origin_route->getSubRouteCost(origin_swap, origin_swap + sub_route);
  int origin_subroutetime = origin_route->getSubRouteTime(origin_swap, origin_swap + sub_route);

  origin_daycost = origin_daycost - first_origin->getCost() - next_origin->getCost() - origin_subroutecost;
  origin_daytime = origin_daytime - first_origin->getTime() - next_origin->getTime() - origin_subroutetime;

  destiny_daycost -= next_destiny->getCost();
  destiny_daytime -= next_destiny->getTime();

  if (previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1) { // cuando se remueve la subruta desde origen
    changeDistance(previus_origin.get(), next_origin.get()); // remocion desde el origen
    delete_acomodation = false;

    origin_daycost += next_origin->getCost();
    origin_daytime += next_origin->getTime();
  } else {
    delete_acomodation = true; // remover el hotel
  }

  changeDistance(previus_destiny.get(), first_origin.get()); // incorporacion al destino
  changeDistance(last_origin.get(), next_destiny.get()); // incorporacion al destino

  destiny_daycost = destiny_daycost + first_origin->getCost() + next_destiny->getCost() + origin_subroutecost;
  destiny_daytime = destiny_daytime + first_origin->getTime() + next_destiny->getTime() + origin_subroutetime;

  if (destiny_daytime <= this->problemInstance->getworktime() && origin_daytime <= this->problemInstance->getworktime()) { // factible respecto al tiempo.
    if (debug) cout << "feasible" << endl;

    if (delete_acomodation) {
      if (debug) cout << "delete_acomodation" << endl;
      if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId()) {
        if (debug) cout << "same routes" << endl;

        changeDistance(temp_route->rute_nodes.at(destiny_swap - 1), temp_route->rute_nodes.at(origin_swap));
        changeDistance(temp_route->rute_nodes.at(origin_swap + sub_route), temp_route->rute_nodes.at(destiny_swap));

        for(int pos=0; pos<=sub_route; pos++){
            Node *node_temp = new Node(*temp_route->rute_nodes.at(new_origin_swap+pos));
            if (debug) node_temp->printAll();
            temp_route->rute_nodes.insert(temp_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
        }
        
        //temp_route->rute_nodes.insert(temp_route->rute_nodes.begin() + destiny_swap, temp_route->rute_nodes.begin() + new_origin_swap, temp_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);

        if (destiny_swap < origin_swap) {
          new_origin_swap += sub_route + 1;
        }
        
        temp_route->rute_nodes.erase( temp_route->rute_nodes.begin() + new_origin_swap, temp_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);

        this->deleteAccommodation(temp_route, new_origin_swap - 1);

        for (int new_time : temp_route->daytime) {
          if (new_time > this->problemInstance->getworktime()) {
            delete temp_route;
            return false;
          }
        }
        origin_route->setTotalRouteCost();

        new_origin_swap = origin_swap;
        changeDistance(origin_route->rute_nodes.at(destiny_swap - 1), origin_route->rute_nodes.at(origin_swap));
        changeDistance(origin_route->rute_nodes.at(origin_swap + sub_route), origin_route->rute_nodes.at(destiny_swap));

//         origin_route->rute_nodes.insert(origin_route->rute_nodes.begin() + destiny_swap, origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);
        
        if (destiny_swap < origin_swap){ //Se van moviendo al insertar en destiny  
            for(int pos=0, displacement=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos + displacement));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                origin_route->rute_nodes.insert(origin_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
                displacement++;
                if(debug) origin_route->printRoute();
            }
        }
        else{
            for(int pos=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                origin_route->rute_nodes.insert(origin_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
            }
        }

        if (destiny_swap < origin_swap) {
          new_origin_swap += sub_route + 1;
        }

        origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);


        this->deleteAccommodation(origin_route, new_origin_swap - 1);
        delete temp_route;
        return true;
      }

      else {
        if (debug) cout << "different routes" << endl;
        temp_route->rute_nodes.erase( temp_route->rute_nodes.begin() + origin_swap, temp_route->rute_nodes.begin() + origin_swap + sub_route + 1);
        // this->deleteAccommodation(temp_route.get(),origin_swap - 1);
        this->deleteAccommodation(temp_route, origin_swap - 1);

        for (int new_time : temp_route->daytime) {
          if (new_time > this->problemInstance->getworktime()) {
            delete temp_route;
            return false;
          }
        }

        origin_route->setTotalRouteCost();

        changeDistance(destiny_route->rute_nodes.at(destiny_swap - 1),
                       origin_route->rute_nodes.at(origin_swap));
        changeDistance(origin_route->rute_nodes.at(origin_swap + sub_route),
                       destiny_route->rute_nodes.at(destiny_swap));

        destiny_route->rute_nodes.insert(
            destiny_route->rute_nodes.begin() + destiny_swap,
            origin_route->rute_nodes.begin() + origin_swap,
            origin_route->rute_nodes.begin() + origin_swap + sub_route + 1);
        origin_route->rute_nodes.erase(
            origin_route->rute_nodes.begin() + origin_swap,
            origin_route->rute_nodes.begin() + origin_swap + sub_route + 1);

        destiny_route->setRouteTime(destiny_day, destiny_daytime);
        destiny_route->setRouteCost(destiny_day, destiny_daycost);

        this->deleteAccommodation(origin_route, origin_swap - 1);
        delete temp_route;

        return true;
      }

    } else {

      if (debug) cout << " else -- delete_acomodation" << endl;
      
      changeDistance(destiny_route->rute_nodes.at(destiny_swap - 1), origin_route->rute_nodes.at(origin_swap));
      changeDistance(origin_route->rute_nodes.at(origin_swap + sub_route), destiny_route->rute_nodes.at(destiny_swap));
      changeDistance(origin_route->rute_nodes.at(origin_swap - 1), origin_route->rute_nodes.at(origin_swap + sub_route + 1));

        if ((origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId()) && (destiny_swap < origin_swap)){ //Se van moviendo al insertar en destiny  
            for(int pos=0, displacement=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos + displacement));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
                displacement++;
                if(debug) origin_route->printRoute();
            }
        }
        else{
            for(int pos=0; pos<=sub_route; pos++){
                Node *node_temp = new Node(*origin_route->rute_nodes.at(new_origin_swap + pos));
                if (debug) cout << "agregando el nodo: " << node_temp->getId() << endl;
                destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap + pos, node_temp);
            }
        }

      
      if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId() && destiny_swap < origin_swap) {
        new_origin_swap += sub_route + 1;
      }

      //TODO: Revisar esta inserción y borrado cuando se trata de una misma ruta.
      
      //destiny_route->rute_nodes.insert(destiny_route->rute_nodes.begin() + destiny_swap, origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);

      destiny_route->setRouteTime(destiny_day, destiny_daytime);
      destiny_route->setRouteCost(destiny_day, destiny_daycost);

      origin_route->rute_nodes.erase( origin_route->rute_nodes.begin() + new_origin_swap, origin_route->rute_nodes.begin() + new_origin_swap + sub_route + 1);

      destiny_route->setRouteTime(destiny_day, destiny_daytime);
      destiny_route->setRouteCost(destiny_day, destiny_daycost);

      origin_route->setRouteTime(origin_day, origin_daytime);
      origin_route->setRouteCost(origin_day, origin_daycost);

      origin_route->setRouteCost();
      origin_route->setTotalRouteCost();
      origin_route->setRouteTime();

      destiny_route->setRouteCost();
      destiny_route->setTotalRouteCost();
      destiny_route->setRouteTime();
      delete temp_route;
      return true;
    }
  }
  delete temp_route;
  return false;
}

void Solution::DisturbRoutesNewDay() {

  int sub_route, selected_destiny_team, selected_origin_team;

  Route *destiny;
  Route *origin;
  do{
      sub_route = random_int_number(0, 2);
      selected_destiny_team = random_int_number(0, this->problemInstance->getTotalDepots() - 1);
      selected_origin_team = random_int_number(0, this->problemInstance->getTotalDepots() - 1);
      destiny = this->Teams.at(selected_destiny_team)->route;
      origin = this->Teams.at(selected_origin_team)->route;
  }while((origin->rute_nodes.size() - (sub_route + 1)) < 3); //TODO: Esto se puede quedar pegado cuando hay pocas opciones.   

  int origin_swap = random_int_number(1, origin->rute_nodes.size() - 2);

  if (origin->rute_nodes.at(origin_swap)->getNodeType() == 2) {
    origin_swap++;
  }

  for (int i = 1; i < sub_route + 1; i++) {
    if (origin->rute_nodes.at(origin_swap + i)->getNodeType() == 2 || origin->rute_nodes.at(origin_swap + i)->getNodeType() == 0) {
      if (i - 1 < sub_route) {
        sub_route = i - 1;
        break;
      }
    }
  }

  int origin_day = origin->getNodeDay(origin_swap);
  bool best_solution = DisturbaddToNewDay(origin, origin_swap, origin_day, destiny, sub_route);
}

bool Solution::DisturbaddToNewDay(Route *origin_route, int swap, int origin_day, Route *destiny_route, int sub_route) {
 
  bool debug = false ;
  if (debug) cout << "Entrando a Solution::DisturbaddToNewDay: " << endl << "Swap: " << swap << ", orign_day: " << origin_day << ", sub_route: " << sub_route << endl;
  if (debug) origin_route->printRoute();
  if (debug) destiny_route->printRoute();
    
  int lastworktime, daysworked;
  float daycost;

  float origin_daycost, destiny_daycost, new_daycost;
  int origin_daytime, destiny_daytime, new_daytime;
  int new_swap = swap;

  daysworked = destiny_route->getRouteDays();

  if (daysworked < this->problemInstance->getDays() - 1) {
    
    if (debug) cout << "daysworked < this->problemInstance->getDays() - 1 " << endl;

    // shared_ptr<Route> Aux(new Route(*destiny_route));

    Route *Aux = new Route(*destiny_route);
    bool delete_acomodation = false;

    // shared_ptr<Node> previus_origin(new
    // Node(*origin_route->rute_nodes.at(swap - 1))); shared_ptr<Node>
    // actual_node(new Node(*origin_route->rute_nodes.at(swap)));
    // shared_ptr<Node> next_origin(new Node(*origin_route->rute_nodes.at(swap +
    // 1)));

    Node *previus_origin = new Node(*origin_route->rute_nodes.at(swap - 1));
    Node *first_origin = new Node(*origin_route->rute_nodes.at(swap));
    Node *last_origin = new Node(*origin_route->rute_nodes.at(swap + sub_route));
    Node *next_origin = new Node(*origin_route->rute_nodes.at(swap + sub_route + 1));

    float origin_subroutecost = origin_route->getSubRouteCost(swap, swap + sub_route);
    int origin_subroutetime = origin_route->getSubRouteTime(swap, swap + sub_route);

    origin_daycost = origin_route->getRouteCost(origin_day);
    origin_daytime = origin_route->getRouteTime(origin_day);

    origin_daycost = origin_daycost - first_origin->getCost() - next_origin->getCost() - origin_subroutecost;
    origin_daytime = origin_daytime - first_origin->getTime() - next_origin->getTime() - origin_subroutetime;

    if (previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1) {
        
      if (debug) cout << "previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1" << endl;
      
      if (debug) cout << "previus_origin: " << previus_origin << endl;
      if (debug) cout << "next_origin: " << next_origin << endl;

      changeDistance(previus_origin, next_origin);
      delete_acomodation = false;

      origin_daycost += next_origin->getCost();
      origin_daytime += next_origin->getTime();

    }
    else {
      if (debug) cout << "Else -- previus_origin->getNodeType() == 1 || next_origin->getNodeType() == 1" << endl;
      if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId() && origin_day == daysworked - 1) {
        if (debug) cout << "Same routes " << endl;
        delete Aux;
        delete previus_origin;
        delete next_origin;
        delete last_origin;
        return false;
      }
      
      delete_acomodation = true;

    }

    destiny_daycost = destiny_route->getRouteTime(daysworked - 1);
    destiny_daytime = destiny_route->getRouteTime(daysworked - 1);

    destiny_daycost -= destiny_route->rute_nodes.back()->getCost();
    destiny_daytime -= destiny_route->rute_nodes.back()->getTime();

    Node *remove = Aux->rute_nodes.back();
    Aux->rute_nodes.pop_back();
    delete remove;

    this->selectAccommodation(Aux);

    destiny_daycost += Aux->rute_nodes.back()->getCost();
    destiny_daytime += Aux->rute_nodes.back()->getTime();

    changeDistance(Aux->rute_nodes.back(), first_origin);

    Aux->AddNode(first_origin);

    for (int i = 1; i < sub_route + 1; i++) {
      if (debug) cout << " agregando nodo: "  << origin_route->rute_nodes.at(swap + i)->getId() << endl;
      Aux->AddNode(new Node(*origin_route->rute_nodes.at(swap + i)));
    }
    
    if (debug) cout << "despues de agregados los nodos: " << endl;
    if (debug) Aux->printRoute();

    returnToDepot(Aux);

    if (debug) cout << "despues de returnToDepot: " << endl;
    if (debug) Aux->printRoute();
    
    new_daycost = first_origin->getCost() + Aux->rute_nodes.back()->getCost() + origin_subroutecost;
    new_daytime = first_origin->getTime() + Aux->rute_nodes.back()->getTime() + origin_subroutetime;

    if (delete_acomodation) {
      if (debug) cout << "delete_acomodation " << endl;
      if (origin_route->rute_nodes.at(0)->getId() == destiny_route->rute_nodes.at(0)->getId()) {
        if (debug) cout << "same routes" << endl;
        
        Aux->rute_nodes.erase(Aux->rute_nodes.begin() + swap, Aux->rute_nodes.begin() + swap + sub_route + 1);
        if (debug) cout << "despues de erase: " << endl;
        if (debug) Aux->printRoute();
        
        this->deleteAccommodation(Aux, swap - 1);
        if (debug) cout << "despues de deleteAccommodation: " << endl;
        if (debug) Aux->printRoute();
        
        for (int new_time : Aux->daytime) {
          if (debug) cout << "new_time: "  << new_time << endl;
          if (new_time > this->problemInstance->getworktime()) {
            delete Aux;
            delete previus_origin;
            delete next_origin;
            delete last_origin;
            return false;
          }
        }
        origin_route->setTotalRouteCost();

        delete Aux;
        delete previus_origin;
        delete next_origin;
        delete last_origin;

        destiny_route->rute_nodes.pop_back();
        this->selectAccommodation(destiny_route);

        changeDistance(destiny_route->rute_nodes.back(), origin_route->rute_nodes.at(swap));

        destiny_route->AddNode(origin_route->rute_nodes.at(swap));

        for (int i = 1; i < sub_route + 1; i++) {
          destiny_route->AddNode(origin_route->rute_nodes.at(swap + i));
        }
        
        returnToDepot(destiny_route);

        origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap, origin_route->rute_nodes.begin() + swap + sub_route + 1);
        this->deleteAccommodation(origin_route, swap - 1);

        destiny_route->setRouteCost();
        destiny_route->setRouteTime();
        destiny_route->setTotalRouteCost();

        origin_route->setRouteCost();
        origin_route->setTotalRouteCost();
        origin_route->setRouteTime();
        return true;

      }

      else {
          
        //cout << "new_daytime: " << new_daytime << endl;
        if (new_daytime > this->problemInstance->getworktime()){ //Verificacion de tiempo en la ruta destino
            delete Aux;
            delete previus_origin;
            delete next_origin;
            delete last_origin;
            return false;
        }  
        if (debug) cout << "no misma ruta" << endl;
        Route *origin_Aux = new Route(*origin_route);
        origin_Aux->rute_nodes.erase(origin_Aux->rute_nodes.begin() + swap, origin_Aux->rute_nodes.begin() + swap + sub_route + 1);
        this->deleteAccommodation(origin_Aux, swap - 1);

        for (int new_time : origin_Aux->daytime) { //Verificacion de tiempo en la ruta origen
          if (debug) cout << "new_time: "  << new_time << endl;
          if (new_time > this->problemInstance->getworktime()) {
            delete Aux;
            delete origin_Aux;
            delete previus_origin;
            delete next_origin;
            delete last_origin;
            return false;
          }
        }

        origin_route->setTotalRouteCost();

        destiny_route->rute_nodes.pop_back();
        this->selectAccommodation(destiny_route);

        changeDistance(destiny_route->rute_nodes.back(), origin_route->rute_nodes.at(swap));

        destiny_route->AddNode(origin_route->rute_nodes.at(swap));

        for (int i = 1; i < sub_route + 1; i++) {
          destiny_route->AddNode(origin_route->rute_nodes.at(swap + i));
        }
        returnToDepot(destiny_route);

        origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap, origin_route->rute_nodes.begin() + swap + sub_route + 1);
        this->deleteAccommodation(origin_route, swap - 1);

        delete Aux;
        delete origin_Aux;
        delete previus_origin;
        delete next_origin;
        delete last_origin;

        destiny_route->setRouteCost();
        destiny_route->setRouteTime();
        destiny_route->setTotalRouteCost();

        origin_route->setRouteCost();
        origin_route->setTotalRouteCost();
        origin_route->setRouteTime();

        return true;
      }

    }

    else {

      delete Aux;
      delete previus_origin;
      delete next_origin;
      delete last_origin;

      //cout << "new_daytime: " << new_daytime << endl;
      if (new_daytime > this->problemInstance->getworktime()){
        return false;
      }

      if (debug) cout << "else -- delete_acomodation " << endl;
      
      destiny_route->rute_nodes.pop_back();
      this->selectAccommodation(destiny_route);

      changeDistance(destiny_route->rute_nodes.back(), origin_route->rute_nodes.at(swap));

      destiny_route->AddNode(origin_route->rute_nodes.at(swap));
        
      for (int i = 1; i < sub_route + 1; i++) {
        destiny_route->AddNode(origin_route->rute_nodes.at(swap + i));
      }

      returnToDepot(destiny_route);

      origin_route->rute_nodes.erase(origin_route->rute_nodes.begin() + swap, origin_route->rute_nodes.begin() + swap + sub_route + 1);
      if (debug) cout << " --------------------------------------------------------------------------------------> " << endl;
      if (debug) origin_route->printAll();
      if (debug) origin_route->rute_nodes.at(swap-1)->printAll();
      if (debug) origin_route->rute_nodes.at(swap)->printAll();
      changeDistance(origin_route->rute_nodes.at(swap-1), origin_route->rute_nodes.at(swap));
      if (debug) origin_route->printAll();

      destiny_route->setRouteCost();
      destiny_route->setRouteTime();
      destiny_route->setTotalRouteCost();

      origin_route->setRouteCost();
      origin_route->setTotalRouteCost();
      origin_route->setRouteTime();

      return true;
    }

  } else {
    return false;
  }
}

void Solution::DisturbRoutesAccommodation() {

  int swap = 0;
  int day = 0;

  int selected_team = random_int_number(0, this->problemInstance->getTotalDepots() - 1);
  Team *t = this->Teams.at(selected_team);
  int total_days = t->route->getRouteDays();

  if (total_days > 1) {
    int selected_accommodation = random_int_number(1, total_days - 1);
    for (Node *n : t->route->rute_nodes) {
      if (n->getNodeType() == 2) {
        day++;
        if (day == selected_accommodation) {
          if (!DisturbAccommodation(t->route, swap, day))
              return;
          break;
        }
      }
      swap++;
    }
  }
}

bool Solution::DisturbAccommodation(Route *route, int swap, int day) {

  vector<Node *> toNode;

  this->available_visit.clear();
  toNode.clear();

  Node *previous_Node = new Node(*route->rute_nodes.at(swap - 1));
  Node *next_Node = new Node(*route->rute_nodes.at(swap + 1));
  Node *Accommodation_Node = new Node(*route->rute_nodes.at(swap));

  float toAccommodation_distance;
  float Accommodation_cost;
  int daytime = route->getRouteTime(day) - next_Node->getTime();
  float min_cost = Accommodation_Node->getCost() + next_Node->getCost();
  int min_i;
  bool change = false;
  float previous_daycost = route->getRouteCost(day - 1) - Accommodation_Node->getCost();
  float actual_daycost = route->getRouteCost(day) - next_Node->getCost();

  int previus_nodeID = previous_Node->getNodeId();
  int next_nodeID = next_Node->getNodeId();

  for (Node *a : this->problemInstance->Anodes) {
    Node *new_Accommodation = new Node(*a);
    Node *new_Next = new Node(*next_Node);
    toAccommodation_distance = this->problemInstance->distances_bxa[previus_nodeID][new_Accommodation->getNodeId()];
    Accommodation_cost = this->problemInstance->Accommodations.at(new_Accommodation->getNodeId())->getCost() + (this->distance_cost * (roundf(toAccommodation_distance * 10) / 10)); // Round con 1 decimal
    new_Accommodation->setDistance(toAccommodation_distance);
    new_Accommodation->setCost(Accommodation_cost);
    available_visit.push_back(new_Accommodation);
    changeDistance(new_Accommodation, new_Next);
    toNode.push_back(new_Next);
  }

  min_i = random_int_number(0, toNode.size() - 1);
  min_cost = available_visit.at(min_i)->getCost() + toNode.at(min_i)->getCost();

  Node *New_Accommodation = new Node(*available_visit.at(min_i));
  Node *New_toNode = new Node(*toNode.at(min_i));
  
  int computed_time = daytime + New_toNode->getTime();
  
  if (computed_time <= this->problemInstance->getworktime()) {// factible
    route->rute_nodes.at(swap) = New_Accommodation;
    route->rute_nodes.at(swap + 1) = New_toNode;
    route->setRouteTime(day, daytime + New_toNode->getTime());
    route->setRouteCost(day - 1, previous_daycost + New_Accommodation->getCost());
    route->setRouteCost(day, actual_daycost + New_toNode->getCost());
  }

  for (Node *a : this->available_visit) {
    delete a;
  }

  for (Node *n : toNode) {
    delete n;
  }

  delete previous_Node;
  delete next_Node;
  delete Accommodation_Node;
    
  this->available_visit.clear();
  this->available_visit.shrink_to_fit();
  toNode.clear();
  toNode.shrink_to_fit();
  
  if (computed_time > this->problemInstance->getworktime()) // infactible
      return false;
  return true;
}

void Solution::check() {
  return;
  set <int> Bridges;
  for (Node *bn : this->problemInstance->Bnodes) {
    Bridges.insert(bn->getId());
  }

  int team = 0;
  
  for (Team *t : this->Teams) {
    //cout << endl << "Team: " << team << endl;
    //t->printRoute();
    
    int deposit, bridgesVisited = 0;
    float totalCost = 0.0;
    
    int day = 0;
    if (t->route->rute_nodes.size() > 0) {
    
      if (day > this->problemInstance->getDays()){
         cout << "ERROR: El equipo sobrepasa el máximo de días." << endl;
         getchar();
         getchar();
         exit(1);
      }
      
      deposit = t->route->rute_nodes.at(0)->getId();
      float distance = 0.0;
      float time = 0.0;
      float cost = 0.0;
      bool first = true;
      Node *before = nullptr;
      for (Node *node : t->route->rute_nodes) {
                
        if (node->getNodeType() == 0 && node->getId() != deposit) {
          cout << "ERROR: La ruta visita un depósito diferente al original" << endl;
          getchar();
          getchar();
          exit(1);
        }

        if (node->getNodeType() == 1) { //Solo los puentes
          bridgesVisited++;
          
          if(!Bridges.erase(node->getId())){
                cout << "ERROR: " << endl;
                cout << "El nodo: " << node->getId() << "  fue visitado más de una vez! " << endl;
                getchar();
                getchar();
                exit(1);
         }
        }

        distance += node->getDistance();
        time += node->getTime();
        cost += node->getCost();
        
        if (before != nullptr){
            if (abs(node->getDistance() - this->problemInstance->getDistance(before, node)) > 0.1){
                cout << "ERROR: La distancia al nodo " << node->getId() << " no está bien computada: " << node->getDistance() << " vs: " << this->problemInstance->getDistance(before, node) << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
            
            if ((node->getNodeType() == 1) && (abs(node->getTime() - this->problemInstance->getTime(before, node)) > 0.1)){
                cout << "ERROR: El tiempo al nodo " << node->getId() << " no está bien computado: " << node->getTime() << " vs: " << this->problemInstance->getTime(before, node) << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
            
            if (abs(node->getCost() - this->problemInstance->getCost(before, node, this->distance_cost)) > 0.1){
                cout << "ERROR: El costo de: " << node->getId() << " no está bien computado: " << node->getCost() << " vs: " << this->problemInstance->getCost(before, node, this->distance_cost) << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
        }
        
        
        if ((node->getNodeType() == 0 && !first) || node->getNodeType() == 2) { 
            first = false;
            /*
            cout << endl << "day: " << day;
            cout << endl << "distance: " << distance;
            cout << endl << "time: " << time;
            cout << endl
            << "-----------------------------------------------" << endl;
            */
            
            if(abs(t->route->daytime[day] - time) > 0.1){
                cout << "ERROR: El tiempo del dia: " << day << " del equipo: " << team << " no corresponde!" << endl;
                cout << "Almacenado: "  << t->route->daytime[day] << " vs Calculado: " << time << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
                        
            if(time > this->problemInstance->getworktime()){
                cout << "ERROR: El tiempo del dia: " << day << " del equipo: " << team << " es mayor que el permitido: " << this->problemInstance->getworktime() << endl;
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }

            if(abs(t->route->daycost[day] - cost) > 0.1){
                cout << "ERROR: El costo del dia: " << day << " del equipo: " << team << " no corresponde!" << endl;
                cout << "Almacenado: " << t->route->daycost[day] << " vs Calculado: " << cost << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
            distance = 0.0;
            time = 0.0;
            totalCost += cost;
            cost = 0.0;
            
            day++;
        }
        delete before;
        before = new Node(*node);
      }
      delete before;
    }
    team++;
  }
  if (Bridges.size() > 0){
    cout << "ERROR: Puentes sin visitar: " << endl;
    for (int bridge: Bridges)
       cout << bridge << endl;
    getchar();
    exit(1);
  }
  
  return;

}

void Solution::alwaysCheck() {
  return;
  set <int> Bridges;
  for (Node *bn : this->problemInstance->Bnodes) {
    Bridges.insert(bn->getId());
  }

  int team = 0;
  
  for (Team *t : this->Teams) {
    //cout << endl << "Team: " << team << endl;
    //t->printRoute();
    
    int deposit, bridgesVisited = 0;
    float totalCost = 0.0;
    
    int day = 0;
    if (t->route->rute_nodes.size() > 0) {
    
      if (day > this->problemInstance->getDays()){
         cout << "ERROR: El equipo sobrepasa el máximo de días." << endl;
         getchar();
         getchar();
         exit(1);
      }
      
      deposit = t->route->rute_nodes.at(0)->getId();
      float distance = 0.0;
      float time = 0.0;
      float cost = 0.0;
      bool first = true;
      Node *before = nullptr;
      for (Node *node : t->route->rute_nodes) {
                
        if (node->getNodeType() == 0 && node->getId() != deposit) {
          cout << "ERROR: La ruta visita un depósito diferente al original" << endl;
          getchar();
          getchar();
          exit(1);
        }

        if (node->getNodeType() == 1) { //Solo los puentes
          bridgesVisited++;
          
          if(!Bridges.erase(node->getId())){
                cout << "ERROR: " << endl;
                cout << "El nodo: " << node->getId() << "  fue visitado más de una vez! " << endl;
                getchar();
                getchar();
                exit(1);
         }
        }

        distance += node->getDistance();
        time += node->getTime();
        cost += node->getCost();
        
        if (before != nullptr){
            if (abs(node->getDistance() - this->problemInstance->getDistance(before, node)) > 0.1){
                cout << "ERROR: La distancia al nodo " << node->getId() << " no está bien computada: " << node->getDistance() << " vs: " << this->problemInstance->getDistance(before, node) << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
            
            if ((node->getNodeType() == 1) && (abs(node->getTime() - this->problemInstance->getTime(before, node)) > 0.1)){
                cout << "ERROR: El tiempo al nodo " << node->getId() << " no está bien computado: " << node->getTime() << " vs: " << this->problemInstance->getTime(before, node) << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
            
            if (abs(node->getCost() - this->problemInstance->getCost(before, node, this->distance_cost)) > 0.1){
                cout << "ERROR: El costo de: " << node->getId() << " no está bien computado: " << node->getCost() << " vs: " << this->problemInstance->getCost(before, node, this->distance_cost) << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
        }
        
        
        if ((node->getNodeType() == 0 && !first) || node->getNodeType() == 2) { 
            first = false;
            /*
            cout << endl << "day: " << day;
            cout << endl << "distance: " << distance;
            cout << endl << "time: " << time;
            cout << endl
            << "-----------------------------------------------" << endl;
            */
            
            if(abs(t->route->daytime[day] - time) > 0.1){
                cout << "ERROR: El tiempo del dia: " << day << " del equipo: " << team << " no corresponde!" << endl;
                cout << "Almacenado: "  << t->route->daytime[day] << " vs Calculado: " << time << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
                        
            if(time > this->problemInstance->getworktime()){
                cout << "ERROR: El tiempo del dia: " << day << " del equipo: " << team << " es mayor que el permitido: " << this->problemInstance->getworktime() << endl;
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }

            if(abs(t->route->daycost[day] - cost) > 0.1){
                cout << "ERROR: El costo del dia: " << day << " del equipo: " << team << " no corresponde!" << endl;
                cout << "Almacenado: " << t->route->daycost[day] << " vs Calculado: " << cost << endl; 
                t->route->printAll();
                getchar();
                getchar();
                exit(1);
            }
            distance = 0.0;
            time = 0.0;
            totalCost += cost;
            cost = 0.0;
            
            day++;
        }
        delete before;
        before = new Node(*node);
      }
      delete before;
    }
    team++;
  }
  if (Bridges.size() > 0){
    cout << "ERROR: Puentes sin visitar: " << endl;
    for (int bridge: Bridges)
       cout << bridge << endl;
    getchar();
    exit(1);
  }
  
  cout << "Solution Checked!" << endl;
  return;

}
void Solution::printSummary() {
  
  float transportationCost = 0.0;
  float accomodationCost = 0.0;
  float dispatchingCost = 0.0;
  
  for (Team *t : this->Teams) {
    dispatchingCost += this->problemInstance->getdispatching_cost();
    if (t->route->rute_nodes.size() > 0) {
    
      Node *before = nullptr;
      for (Node *node : t->route->rute_nodes) {
                        
        if (before != nullptr){
            transportationCost+=this->problemInstance->getCost(before, node, this->distance_cost);
            if (node->getNodeType() == 2){ //Para hacer el cálculo de costo de hotel.
                transportationCost-= this->problemInstance->Accommodations.at(node->getNodeId())->getCost();
                accomodationCost+= this->problemInstance->Accommodations.at(node->getNodeId())->getCost();
            }
        }
        delete before;
        before = new Node(*node);
      }
      delete before;
    }
  }
  cout << transportationCost + accomodationCost + dispatchingCost << " " << transportationCost << " " << accomodationCost << " "  << dispatchingCost << " "; 
  return;

}
