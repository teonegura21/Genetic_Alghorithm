#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;

const double pi = 3.14159265358979323846;

class FunctiiObiective {
protected:
    double stg;
    double dr;
    string nume;

public:
    FunctiiObiective(double stg, double dr, string nume) :
        stg(stg), dr(dr), nume(nume) {};

    virtual ~FunctiiObiective() = default;

    virtual double calcfitnes(const vector<double>& pcurent) = 0;
    virtual double calcComponentFitness(int index, double valoare) const = 0;

    double getstg() const { return stg; }
    double getdr() const { return dr; }
    string getNume() const { return nume; }
};

class DeJong1 : public FunctiiObiective
{
public:
    DeJong1() : FunctiiObiective(-5.12, 5.12, "DeJong1") {};
    double calcfitnes(const vector<double>& pcurent) override;
    double calcComponentFitness(int index, double valoare) const override;
};

class Rastrigin : public FunctiiObiective
{
public:
    Rastrigin() : FunctiiObiective(-5.12, 5.12, "Rastrigin") {};
    double calcfitnes(const vector<double>& pcurent) override;
    double calcComponentFitness(int index, double valoare) const override;
};

class Schwefel : public FunctiiObiective
{
public:
    Schwefel() : FunctiiObiective(-500, 500, "Schwefel") {};
    double calcfitnes(const vector<double>& pcurent) override;
    double calcComponentFitness(int index, double valoare) const override;
};


class Michalewicz : public FunctiiObiective
{
public:
    Michalewicz() : FunctiiObiective(0, pi, "Michalewicz") {};
    double calcfitnes(const vector<double>& pcurent) override;
    double calcComponentFitness(int index, double valoare) const override;
};
