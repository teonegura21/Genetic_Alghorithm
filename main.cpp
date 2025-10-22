#include "Algoritmi.h"
#include "FunctiiModel.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <limits>
#include <iomanip>
#include <sstream>
#include <future>
#include <thread>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <map>

enum Strategie { FIRST, BEST, WORST, SA };

struct RunResult {
    double fitness;
    int evaluations;
};

struct Statistics {
    double best;
    double worst;
    double mean;
    double stddev;
    double meanEvaluations;
};

Statistics computeStatistics(const std::vector<RunResult>& runs) {
    Statistics stats{std::numeric_limits<double>::infinity(),
                     -std::numeric_limits<double>::infinity(),
                     0.0,
                     0.0,
                     0.0};

    if (runs.empty()) {
        return stats;
    }

    double sum = 0.0;
    double sumEvaluations = 0.0;
    for (const auto& run : runs) {
        stats.best = std::min(stats.best, run.fitness);
        stats.worst = std::max(stats.worst, run.fitness);
        sum += run.fitness;
        sumEvaluations += run.evaluations;
    }
    stats.mean = sum / runs.size();
    stats.meanEvaluations = sumEvaluations / runs.size();

    double variance = 0.0;
    for (const auto& run : runs) {
        double diff = run.fitness - stats.mean;
        variance += diff * diff;
    }
    variance /= runs.size();
    stats.stddev = std::sqrt(variance);

    return stats;
}

RunResult ruleazaExperiment(FunctiiObiective& functie, int D, double delta, Strategie strategie, double best_improvement_result = 0) {
    hillclimbing hc(functie, D, delta);
    double rezultat = 0.0;  // Initialize to safe default value

    switch (strategie) {
        case FIRST:
            rezultat = hc.firstimprovement();
            break;
        case BEST:
            rezultat = hc.bestImprovement();
            break;
        case WORST:
            rezultat = hc.worstImprovement();
            break;
        case SA:
            rezultat = hc.simulatedAnnealing(100, 0.001, best_improvement_result);
            break;
        default:
            throw std::invalid_argument("Unknown strategy type");
    }
    return {rezultat, hc.getEvaluari()};
}

int main() {
    const double delta = 0.0000001;
    const int num_runs = 50;
    std::vector<int> dimensiuni = {5, 10, 30, 100};
    std::vector<Strategie> strategii = {FIRST, BEST, WORST, SA};

    std::vector<std::unique_ptr<FunctiiObiective>> functii;
    functii.push_back(std::make_unique<DeJong1>());
    functii.push_back(std::make_unique<Rastrigin>());
    functii.push_back(std::make_unique<Schwefel>());
    functii.push_back(std::make_unique<Michalewicz>());

    // First pass: Run Best Improvement for all (function, dimension) combinations
    // to get reference values for Simulated Annealing
    std::map<std::pair<std::string, int>, double> best_references;
    std::vector<std::future<std::pair<std::pair<std::string, int>, double>>> ref_futures;
    
    for (const auto& functiePtr : functii) {
        FunctiiObiective* functie_raw = functiePtr.get();
        for (int d : dimensiuni) {
            ref_futures.emplace_back(std::async(std::launch::async, [functie_raw, d, delta, num_runs]() {
                std::vector<RunResult> run_results;
                run_results.reserve(num_runs);
                for (int i = 0; i < num_runs; ++i) {
                    run_results.push_back(ruleazaExperiment(*functie_raw, d, delta, BEST));
                }
                Statistics stats = computeStatistics(run_results);
                return std::make_pair(std::make_pair(functie_raw->getNume(), d), stats.best);
            }));
        }
    }
    
    // Collect reference values
    for (auto& fut : ref_futures) {
        auto result = fut.get();
        best_references[result.first] = result.second;
    }
    
    // Second pass: Parallelize at (function, dimension, strategy) level
    std::vector<std::future<std::string>> results;
    for (const auto& functiePtr : functii) {
        FunctiiObiective* functie_raw = functiePtr.get();
        for (int d : dimensiuni) {
            for (Strategie s : strategii) {
                results.emplace_back(std::async(std::launch::async, [functie_raw, d, delta, s, num_runs, &best_references]() {
                    std::stringstream ss;
                    std::vector<RunResult> run_results;
                    run_results.reserve(num_runs);
                    
                    double best_ref = 0.0;
                    if (s == SA) {
                        best_ref = best_references.at(std::make_pair(functie_raw->getNume(), d));
                    }
                    
                    for (int i = 0; i < num_runs; ++i) {
                        RunResult rezultat;
                        if (s == SA) {
                            rezultat = ruleazaExperiment(*functie_raw, d, delta, s, best_ref);
                        } else {
                            rezultat = ruleazaExperiment(*functie_raw, d, delta, s);
                        }
                        run_results.push_back(rezultat);
                    }
                    
                    Statistics stats = computeStatistics(run_results);
                    
                    std::string numeStrategie;
                    switch (s) {
                        case FIRST:
                            numeStrategie = "First Improvement";
                            break;
                        case BEST:
                            numeStrategie = "Best Improvement";
                            break;
                        case WORST:
                            numeStrategie = "Worst Improvement";
                            break;
                        case SA:
                            numeStrategie = "Simulated Annealing";
                            break;
                    }
                    
                    ss << std::fixed << std::setprecision(7)
                       << "Functia: " << functie_raw->getNume()
                       << ", Dimensiune: " << d
                       << ", Strategie: " << numeStrategie
                       << ", Best: " << stats.best
                       << ", Worst: " << stats.worst
                       << ", Mean: " << stats.mean
                       << ", Sigma: " << stats.stddev
                       << ", Evaluari medii: " << stats.meanEvaluations
                       << ", Rulari: " << num_runs << std::endl;
                    
                    return ss.str();
                }));
            }
        }
    }

    for (auto& result : results) {
        std::cout << result.get();
    }
    std::cout << "------------------------------------" << std::endl;

    return 0;
}
