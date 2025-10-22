#include "FunctiiModel.h"
#include <cmath>
#include <numeric> // Pentru std::accumulate

using namespace std;

double DeJong1::calcfitnes(const vector<double>& pcurent)
{
    double sum = 0.0;
    for (size_t i = 0; i < pcurent.size(); ++i)
    {
        sum += calcComponentFitness(static_cast<int>(i), pcurent[i]);
    }
    return sum;
}

double DeJong1::calcComponentFitness(int, double valoare) const
{
    return valoare * valoare;
}

double Rastrigin::calcfitnes(const vector<double>& pcurent)
{
    double sum = 0.0;
    for (size_t i = 0; i < pcurent.size(); ++i)
    {
        sum += calcComponentFitness(static_cast<int>(i), pcurent[i]);
    }
    return sum;
}

double Rastrigin::calcComponentFitness(int, double valoare) const
{
    return valoare * valoare - 10.0 * cos(2.0 * pi * valoare) + 10.0;
}

double Michalewicz::calcfitnes(const vector<double>& pcurent)
{
    double sum = 0.0;
    for (size_t i = 0; i < pcurent.size(); ++i)
    {
        sum += calcComponentFitness(static_cast<int>(i), pcurent[i]);
    }
    return sum;
}

double Michalewicz::calcComponentFitness(int index, double valoare) const
{
    constexpr int m = 10;
    double sin_part = sin((index + 1) * valoare * valoare / pi);
    return -sin(valoare) * pow(sin_part, 2 * m);
}

double Schwefel::calcfitnes(const vector<double>& pcurent)
{
    double sum = 0.0;
    for (size_t i = 0; i < pcurent.size(); ++i)
    {
        sum += calcComponentFitness(static_cast<int>(i), pcurent[i]);
    }
    return sum;
}

double Schwefel::calcComponentFitness(int, double valoare) const
{
    return 418.9829 - valoare * sin(sqrt(fabs(valoare)));
}
