#include "Reader.h"
#include "algorithm"

Reader::Reader(string option) : filePath(option) {}

Reader::~Reader() { /*cout << "Deleting File Reader" << endl; */
}

ProblemInstance *Reader::readInputFile() {
  ProblemInstance *problemInstance;
  problemInstance = new ProblemInstance();

  string filePath = this->filePath;
  input.open(filePath);

  if (!input.is_open()) {
    cerr << "Could not open the file: " << filePath << std::endl;
    exit(EXIT_FAILURE);
  }

  cout << "A: " << filePath << endl;

  vector<string> data;

  this->findDef("O:=");
  this->CountDepots(problemInstance);

  this->findDef("N:=");
  this->CountBridges(problemInstance);

  this->findDef("M:=");
  this->CountAccommodations(problemInstance);

  cout << "b: " << filePath << endl;

  problemInstance->setTotalDistances();

  cout << "c: " << filePath << endl;

  this->findDef("K:=");
  this->readDays(problemInstance);

  this->findDef("Q:=");
  this->readWorkTime(problemInstance);

  this->findDef("v:=");
  this->readDispatchingCost(problemInstance);

  this->findDef("f:=");
  this->readAccomodations(problemInstance);

  this->findDef(":=");
  this->readNodes(problemInstance);

  cout << "d: " << filePath << endl;

  // problemInstance->setBxBDistance();
  // problemInstance->setBxDDistance();
  // problemInstance->setBxADistance();

  this->findDef(":=");
  this->readDistances(problemInstance, 0, problemInstance->getTotalBridges(),
                      problemInstance->getTotalBridges()); //distancia de viaje entre puentes

  this->findDef(":=");
  this->readDistances(problemInstance, 1, problemInstance->getTotalBridges(),
                      problemInstance->getTotalAccommodations()); //distancia viaje entre puentes y alojamientos

  this->findDef(":=");
  this->readDistances(problemInstance, 2, problemInstance->getTotalAccommodations(),
                      problemInstance->getTotalBridges()); //distancia entre alojamientos y puentes

  this->findDef(":=");
  this->readDistances(problemInstance, 3, problemInstance->getTotalDepots(),
                      problemInstance->getTotalBridges()); //distancia entre depositos y puentes

  this->findDef(":=");
  this->readDistances(problemInstance, 4, problemInstance->getTotalBridges(),
                      problemInstance->getTotalDepots()); //distancia entre puentes y depositos

  cout << "e: " << filePath << endl;

  input.close();

  return problemInstance;
}

void Reader::findDef(string def) {
  string word;
  while (true) {
    input >> word;
    if (word.find(def) != std::string::npos) {
      break;
    }
  }
}
void Reader::CountDepots(ProblemInstance *problemInstance) {
  string node;
  int matrix_size(0);
  while (true) {
    input >> node;

    matrix_size++;
    if (node.find(';') != std::string::npos) {
      node.erase(std::remove(node.begin(), node.end(), ';'), node.end());
      problemInstance->depot_nodes.push_back(stoi(node));
      problemInstance->setTotalDepots(matrix_size);
      break;
    }
    problemInstance->depot_nodes.push_back(stoi(node));
  }
}
void Reader::CountBridges(ProblemInstance *problemInstance) {
  string node;
  int matrix_size(0);

  while (true) {
    input >> node;
    matrix_size++;

    if (node.find(';') != std::string::npos) {
      node.erase(std::remove(node.begin(), node.end(), ';'), node.end());
      problemInstance->bridge_nodes.push_back(stoi(node));
      problemInstance->setTotalBridges(matrix_size);
      break;
    }

    problemInstance->bridge_nodes.push_back(stoi(node));
  }
}
void Reader::CountAccommodations(ProblemInstance *problemInstance) {
  string node;
  int matrix_size(0);
  while (true) {
    input >> node;
    matrix_size++;
    if (node.find(';') != std::string::npos) {
      node.erase(std::remove(node.begin(), node.end(), ';'), node.end());
      problemInstance->accommodation_nodes.push_back(stoi(node));
      problemInstance->setTotalAccommodations(matrix_size);
      break;
    }
    problemInstance->accommodation_nodes.push_back(stoi(node));
  }
}

void Reader::readDays(ProblemInstance *problemInstance) {
  string day;
  int days(0);
  while (true) {
    input >> day;
    days++;
    if (day.find(';') != std::string::npos) {
      problemInstance->setDays(days);
      break;
    }
  }
}

void Reader::readWorkTime(ProblemInstance *problemInstance) {
  string worktime;
  while (true) {
    input >> worktime;
    if (worktime.find(';') != std::string::npos) {
      break;
    }
  }
  problemInstance->setworktime(stoi(worktime));
}

void Reader::readDispatchingCost(ProblemInstance *problemInstance) {
  string dispatching;
  while (true) {
    input >> dispatching;
    if (dispatching.find(';') != std::string::npos) {
      break;
    }
  }
  problemInstance->setdispatching_cost(stof(dispatching));
}

void Reader::readAccomodations(ProblemInstance *problemInstance) {
  string id;
  int node_id = 0;
  int cost;
  while (true) {
    input >> id;
    if (id.find(';') != std::string::npos) {
      break;
    }
    input >> cost;
    auto *accommodation = new Accommodation(stoi(id), cost, node_id);
    problemInstance->addAccomodation(accommodation);
    node_id++;
  }
}

void Reader::readNodes(ProblemInstance *problemInstance) {
  string id;
  double Xcord;
  double Ycord;
  int bridge_wt;
  int auxid;
  int nodeidD, nodeidB, nodeidA;
  int type;

  cout << "readNodes" << endl;

  nodeidD = nodeidB = nodeidA = 0;
  while (true) {
    input >> id;
    cout << "id: " << id << " " ;
    if (id.find(';') != std::string::npos) {
      problemInstance->sortNodes();
      break;
    } else if (id.find('#') != std::string::npos) {
      input >> Xcord >> Ycord >> bridge_wt >> auxid;
      continue;
    }
    input >> Xcord >> Ycord >> bridge_wt >> auxid;

    if (find(problemInstance->depot_nodes.begin(),
             problemInstance->depot_nodes.end(),
             stoi(id)) != problemInstance->depot_nodes.end()) {
      type = 0;
      auto *node = new Node(stoi(id), Xcord, Ycord, bridge_wt, nodeidD, type);
      problemInstance->addDepotNode(node);
      nodeidD++;

    } else if (find(problemInstance->accommodation_nodes.begin(),
                    problemInstance->accommodation_nodes.end(),
                    stoi(id)) != problemInstance->accommodation_nodes.end()) {
      type = 2;
      auto *node = new Node(stoi(id), Xcord, Ycord, bridge_wt, nodeidA, type);
      problemInstance->addAccommodationNode(node);
      nodeidA++;

    } else if (find(problemInstance->bridge_nodes.begin(),
                    problemInstance->bridge_nodes.end(),
                    stoi(id)) != problemInstance->bridge_nodes.end()) {
      type = 1;
      auto *node = new Node(stoi(id), Xcord, Ycord, bridge_wt, nodeidB, type);
      problemInstance->addBridgeNode(node);
      nodeidB++;
    }
    cout << "type: " << type << endl;
  }
}

void Reader::readDistances(ProblemInstance *problemInstance, int Mtype,
                           int t_row, int t_columns) {
  string id;
  string distance;
  int row_count = 0;
  while (row_count < t_row) {
    int col_count = 0;
    input >> id;
    while (col_count < t_columns) {
      input >> distance;
      problemInstance->setDistance(Mtype, row_count, col_count, stof(distance));
      col_count++;
    }
    row_count++;
  }
}
