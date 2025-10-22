#include "Algoritmi.h"
#include "FunctiiModel.h"
#include <iostream>
#include <random>
#include <vector>
#include <algorithm> // For std::shuffle
#include <limits>   // For std::numeric_limits
#include <cmath> // for exp
#include <map>

double hillclimbing::simulatedAnnealing(double initial_temp, double beta, double best_improvement_result)
{
    fitnescurent = best_improvement_result;

    double temp = initial_temp;
    while (evaluari < maxEvaluari && temp > 1e-5)
    {
        // "Slice" and "hop" to a new point
        vector<double> new_point = genereazaPunctRandom();
        double new_fitness = functie.calcfitnes(new_point);
        evaluari++;

        if (new_fitness < fitnescurent)
        {
            pcurent = new_point;
            fitnescurent = new_fitness;
            initializeComponentFitness();
        }
        else
        {
            double prob = exp(-(new_fitness - fitnescurent) / temp);
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            if (dist(rng) < prob)
            {
                pcurent = new_point;
                fitnescurent = new_fitness;
                initializeComponentFitness();
            }
        }

        temp = temp / (1 + beta * temp);
    }

    return fitnescurent;
}


using namespace std;

// Functie pentru a genera un punct random in domeniul functiei
vector<double> hillclimbing::genereazaPunctRandom()
{
    std::uniform_real_distribution<double> dist(functie.getstg(), functie.getdr());
    std::vector<double> punct(D);
    for (double& coord : punct) {
        coord = dist(rng);
    }
    return punct;
}

// Constructor pentru clasa hillclimbing
hillclimbing::hillclimbing(FunctiiObiective& functie, int D, double delta)
    : functie(functie), D(D), delta(delta), componenteFitness(), evaluari(0),
      maxEvaluari(100000 * std::max(1, D)), rng()
{
    std::random_device rd;
    std::seed_seq seed{
        rd(), rd(), rd(), rd(),
        rd(), rd(), rd(), rd()
    };
    rng.seed(seed);

    pcurent = genereazaPunctRandom();
    fitnescurent = functie.calcfitnes(pcurent);
    evaluari++;
    initializeComponentFitness();
}

// Verifica daca o valoare se afla in intervalul definit de functie
bool hillclimbing::esteInInterval(double valoare) const {
    return valoare >= functie.getstg() && valoare <= functie.getdr();
}

// Genereaza vecinii punctului curent
vector<pair<vector<double>, int>> hillclimbing::genereazaVecini() const {
    vector<pair<vector<double>, int>> vecini;
    vecini.reserve(2 * D);
    vector<double> vecin_temp = pcurent;

    for (int i = 0; i < D; ++i) {
        double valoare_originala = vecin_temp[i];

        // Vecin cu delta adunat
        vecin_temp[i] = valoare_originala + delta;
        if (esteInInterval(vecin_temp[i])) {
            vecini.push_back({vecin_temp, i});
        }

        // Vecin cu delta scazut
        vecin_temp[i] = valoare_originala - delta;
        if (esteInInterval(vecin_temp[i])) {
            vecini.push_back({vecin_temp, i});
        }

        // Resetare valoare pentru urmatoarea dimensiune
        vecin_temp[i] = valoare_originala;
    }
    return vecini;
}

void hillclimbing::initializeComponentFitness()
{
    componenteFitness.assign(D, 0.0);
    double total = 0.0;
    for (int i = 0; i < D; ++i)
    {
        componenteFitness[i] = functie.calcComponentFitness(i, pcurent[i]);
        total += componenteFitness[i];
    }
    fitnescurent = total;
}


// Algoritmul Hill Climbing cu strategia First Improvement
double hillclimbing::firstimprovement()
{
    while (evaluari < maxEvaluari) 
    {
        bool imbunatatireGasita = false;
        auto vecini = genereazaVecini();
        std::shuffle(vecini.begin(), vecini.end(), rng);


        for (const auto& pereche_vecin : vecini)
        {
            const auto& vecin = pereche_vecin.first;
            int coordIndex = pereche_vecin.second;
            double componentNou = functie.calcComponentFitness(coordIndex, vecin[coordIndex]);
            evaluari++;
            double fitnesvecin = fitnescurent - componenteFitness[coordIndex] + componentNou;

            if (fitnesvecin < fitnescurent)
            {
                pcurent = vecin;
                fitnescurent = fitnesvecin;
                componenteFitness[coordIndex] = componentNou;
                imbunatatireGasita = true;
                break; 
            }
        }

        if (!imbunatatireGasita)
        {
            break; 
        }
    }
    return fitnescurent;
}

// Algoritmul Hill Climbing cu strategia Best Improvement
double hillclimbing::bestImprovement()
{
    while (evaluari < maxEvaluari)
    {
        auto vecini = genereazaVecini();
        vector<double> bestVecin;
        double bestFitnes = fitnescurent;
        int bestIndex = -1;
        double bestComponent = 0.0;

        for (const auto& pereche_vecin : vecini)
        {
            const auto& vecin = pereche_vecin.first;
            int coordIndex = pereche_vecin.second;
            double componentNou = functie.calcComponentFitness(coordIndex, vecin[coordIndex]);
            evaluari++;
            double fitnesvecin = fitnescurent - componenteFitness[coordIndex] + componentNou;

            if (fitnesvecin < bestFitnes)
            {
                bestFitnes = fitnesvecin;
                bestVecin = vecin;
                bestIndex = coordIndex;
                bestComponent = componentNou;
            }
        }

        if (bestFitnes < fitnescurent && bestIndex != -1)
        {
            pcurent = bestVecin;
            fitnescurent = bestFitnes;
            componenteFitness[bestIndex] = bestComponent;
        }
        else
        {
            break; // Nu s-a gasit nicio imbunatatire
        }
    }
    return fitnescurent;
}

// Algoritmul Hill Climbing cu strategia Worst Improvement
double hillclimbing::worstImprovement()
{
    while (evaluari < maxEvaluari)
    {
        auto vecini = genereazaVecini();
        vector<double> worstMejorVecin;
        double worstMejorFitnes = -std::numeric_limits<double>::infinity();
        int worstIndex = -1;
        double worstComponent = 0.0;

        for (const auto& pereche_vecin : vecini)
        {
            const auto& vecin = pereche_vecin.first;
            int coordIndex = pereche_vecin.second;
            double componentNou = functie.calcComponentFitness(coordIndex, vecin[coordIndex]);
            evaluari++;
            double fitnesvecin = fitnescurent - componenteFitness[coordIndex] + componentNou;

            if (fitnesvecin < fitnescurent)
            {
                if (fitnesvecin > worstMejorFitnes)
                {
                    worstMejorFitnes = fitnesvecin;
                    worstMejorVecin = vecin;
                    worstIndex = coordIndex;
                    worstComponent = componentNou;
                }
            }
        }

        if (worstMejorFitnes != -std::numeric_limits<double>::infinity() && worstIndex != -1)
        {
            pcurent = worstMejorVecin;
            fitnescurent = worstMejorFitnes;
            componenteFitness[worstIndex] = worstComponent;
        }
        else
        {
            break; // Nu s-a gasit nicio imbunatatire
        }
    }
    return fitnescurent;
}
