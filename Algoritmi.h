#pragma once
#include "FunctiiModel.h"
#include <vector>
#include <random>
#include <set>

using namespace std;

enum StrategieHC {
    FIRST_IMPROVEMENT,
    BEST_IMPROVEMENT,
    WORST_IMPROVEMENT,
    SIMULATED_ANNEALING
};

class hillclimbing {
private:
    FunctiiObiective& functie;
    int D;
    double delta;
    vector<double> pcurent;
    double fitnescurent;
    vector<double> componenteFitness;
    int evaluari;
    int maxEvaluari;
    mt19937 rng;

    vector<double> genereazaPunctRandom();
    bool esteInInterval(double valoare) const;
    vector<pair<vector<double>, int>> genereazaVecini() const;
    void initializeComponentFitness();

public:
    hillclimbing(FunctiiObiective& functie, int D, double delta);
    ~hillclimbing() = default;

    double firstimprovement();
    double bestImprovement();
    double worstImprovement();
    double simulatedAnnealing(double initial_temp, double beta, double best_improvement_result);

    int getEvaluari() const { return evaluari; }
    double getFitnessCurent() const { return fitnescurent; }
    const vector<double>& getPunctCurent() const { return pcurent; }
};
