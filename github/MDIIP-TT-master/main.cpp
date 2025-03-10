#include "Constructor.h"
#include "Iterator.h"
#include "ProblemInstance.h"
#include "Reader.h"
#include "Solution.h"
#include <chrono>
#include <iostream>
#include <time.h>

using namespace std::chrono;

int main(int argc, char *argv[]) {

  if (argc == 1) {
    string instance = "15puentes.dat";
    int seed = 3;
    int max_iter_vns = 10;
    int max_iter_disturb = 10;
    int max_iter = 1000;
    srand(seed);
    auto start = high_resolution_clock::now();

    Reader r(instance);

    cout << "For instance " << instance << endl;
    cout << "Seed: " << seed << endl;
    cout << "Max Iter: " << max_iter << endl;
    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile();

    // problemInstance->printAll();

    cout << "total depots: " << problemInstance->getTotalDepots() << endl;

    auto *currentSolution = new Solution(problemInstance);

    auto *construction = new Constructor();
    construction->feasibleSolution(currentSolution);

    cout << "Solucion Construida: " << endl;
    currentSolution->printAll();

    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "
            "- - - - - - - - - - - -"
         << endl;

    // currentSolution->printAll();
    // currentSolution->changeRoutesNodeSwap();
    // currentSolution->changeRoutesRouteSwap(1);
    // currentSolution->changeRoutesNodeDay();
    // currentSolution->changeRoutesNodeNewDay(currentSolution->Teams)

    // currentSolution->VNSAll(max_iter);
    // currentSolution->DisturbAll(max_iter);
    // cout << "Solucion Mejorada: " << endl;

    Iterator *iterator = new Iterator();

    iterator->IterateSolution(currentSolution, max_iter, max_iter_vns,
                              max_iter_disturb);

    delete problemInstance;

    delete currentSolution;

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Tiempo de ejecucion: " << duration.count() << " milisegundos "
         << endl;
  }

  else {
    string instance = argv[1];
    int seed = atoi(argv[2]);
    int max_iter = atoi(argv[5]);
    srand(seed);

    auto start = high_resolution_clock::now();

    Reader r(instance);

    cout << "For instance " << instance << endl;
    cout << "Seed: " << seed << endl;
    ProblemInstance *problemInstance;

    problemInstance = r.readInputFile();
    problemInstance->printAll();

    int max_iter_vns = int(atof(argv[3])*problemInstance->getTotalBridges());
    int max_iter_disturb = int(atof(argv[4])*problemInstance->getTotalBridges());

    if (max_iter_vns < 1) max_iter_vns = 1;
    if (max_iter_disturb < 1) max_iter_disturb = 1;

    max_iter = max_iter / ((float)max_iter_vns);


    cout << "Max Iter: " << max_iter << ", max_iter_vns: " << max_iter_vns << ", max_iter_disturb: " << max_iter_disturb << endl;
    
    cout << "total depots: " << problemInstance->getTotalDepots() << endl;

    auto *currentSolution = new Solution(problemInstance);

    auto *construction = new Constructor();
    
    while (!construction->feasibleSolution(currentSolution)){
        delete currentSolution;
        auto *currentSolution = new Solution(problemInstance);
    }

    cout << "Solucion Construida: " << endl;
    currentSolution->printAll();
    currentSolution->check();

    cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "     << endl;

    Iterator *iterator = new Iterator();

    iterator->IterateSolution(currentSolution, max_iter, max_iter_vns,
                              max_iter_disturb);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << seed << " "  << duration.count()/1000 << endl;
    
    delete problemInstance;
    delete currentSolution;
    
    
  }

  return 0;
}
