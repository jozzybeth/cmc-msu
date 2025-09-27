#ifndef MAMMAL_H
#define MAMMAL_H

#include "animal.h"
//Млекопитающее

class Mammal: virtual public Animal {
    double weight;
public:
    Mammal(const string& name = "No name", int age = 0, double weight = 0);
    double popularity() const;
    void print() const;
    ~Mammal();
};

#endif 