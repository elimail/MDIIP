#include "Constructor.h"
#include "algorithm"

Constructor::Constructor(){};

Constructor::~Constructor() = default;

bool Constructor::feasibleSolution(Solution *solution) {

  vector<int> depot_IDs; // Lista de Equipos disponibles en cada iteración
  int index = 0;
  int DepotID;

  bool debug = false;

  // Cumplir visitas minimas por equipo
  for (Team *t : solution->Teams) {
    if (debug) cout << "iterando por equipo" << endl;
    depot_IDs.push_back(t->visited_nodes.front()->getNodeId());
    while (t->TotalVisited < solution->minvisit) {
      if (debug) cout << "solution->minvisit: " << solution->minvisit << endl;
      if (debug) cout << "mientras no se cumpla la cantidad minima de visitas" << endl;
      solution->getAvailableBridges(t);

      if (!solution->available_visit.empty()) {
        if (debug) cout << "selectBridge" << endl;
        solution->selectBridge(t);

      } else {
        if (t->workdays < solution->problemInstance->getDays()) {
          if (debug) cout << "selectAccommodation" << endl;
          solution->selectAccommodation(t);

        } else {
          solution->returnToDepot(t);
          if (debug) cout << "returnToDepot" << endl;
          depot_IDs.erase(depot_IDs.begin() + index);
        }
      }
      if (t->TotalVisited == solution->maxvisit) {
        if (debug) cout << "t->TotalVisited == solution->maxvisit" << endl;
        solution->returnToDepot(t);
        depot_IDs.erase(depot_IDs.begin() + index);
      }
    }
    index++;
  }


  if(debug) cout << "-----------------------------------------------------------------------------------------" << endl;
  while (!solution->Unvisited_bridges.empty()) {
    if(debug) cout << "iterando por acá!" << endl;
    if(depot_IDs.size()<1){ //TODO:Si se acaban los depósitos retorno falso y trato otra vez?
          return false;
    }
    if(debug) cout << "depot_IDs.size(): " << depot_IDs.size() << endl;
    
    index = solution->random_int_number(0, depot_IDs.size() - 1);
    
    DepotID = depot_IDs[index];
    if(debug) cout << "DepotID: " << DepotID << endl;

    solution->getAvailableBridges(solution->Teams.at(DepotID));
    if (!solution->available_visit.empty()) {
      solution->selectBridge(solution->Teams.at(DepotID));
      if(debug) cout << "seleccionando puente" << endl;
    } else {
      if (solution->Teams.at(DepotID)->workdays <
          solution->problemInstance->getDays()) {
        solution->selectAccommodation(solution->Teams.at(DepotID));
        if(debug) cout << "seleccionando hotel" << endl;
      } else {
        solution->returnToDepot(solution->Teams.at(DepotID));
        depot_IDs.erase(depot_IDs.begin() + index);
      }
    }
    if (solution->Teams.at(DepotID)->TotalVisited == solution->maxvisit) {
      solution->returnToDepot(solution->Teams.at(DepotID));
      if(debug) cout << "de vuelta al depot" << endl;
      depot_IDs.erase(depot_IDs.begin() + index);
    }
  }

  for (Team *t : solution->Teams) {
    if (t->visited_nodes.back()->getNodeType() != 0) {
      if (t->visited_nodes.back()->getNodeType() == 2) {
        t->RestoreAccommodation();
      }
      solution->returnToDepot(t);
    }
    t->NodetoRoute();
  }
  if (debug) cout << "apunto de un true!" << endl;
  solution->setTotalCost();
  return true;
}
