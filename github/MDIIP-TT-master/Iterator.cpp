#include "Iterator.h"
#include "algorithm"

Iterator::Iterator(){};

Iterator::~Iterator() = default;

void Iterator::IterateSolution(Solution *solution, int iterations, int vns_iterations, int disturb_iterations) {

  bool debug = false;    
  Solution *best = new Solution(*solution);
  Solution *current = new Solution(*solution);
  int iter = 0;
  int mejora = 1;
  int it_stuck = 0;
  
  while (iter < iterations) {
    //cout << it_stuck << " " << (0.1*iterations) << endl;
    if (it_stuck > (0.05*iterations)) {
        cout << "Stucked" << endl;
        break;
    }
    if (debug) cout << "iter: "  << iter << endl;
    
    current->VNSAll(vns_iterations);

    if(debug) cout << "En IterateSolution, posterior a VNSAll " << endl;
    current->check();
    //getchar();
    
    if (current->getTotalCost() < best->getTotalCost()) {
      best->reset(*current);
      best->check();
      cout << iter << " ";
      best->printSummary();
      cout << endl;
      it_stuck=0;
    }
    
    current->DisturbAll(disturb_iterations);
    if(debug) cout << "En IterateSolution, posterior a DisturbAll " << endl;
    current->check();
    iter++;
    it_stuck++;
  }

  best->printAll();
  best->alwaysCheck();
  best->printSummary();
  return;
}
