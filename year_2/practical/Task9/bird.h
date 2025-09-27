#ifndef BIRD_H
#define BIRD_H

#include "animal.h"

//Птица
class Bird: virtual public Animal {
    double wingspan;
public:
    Bird(const string& name = "No name", int age = 0, double wingspan = 0);
    void print() const;
    double popularity() const;
    ~Bird();
};

#endif 
