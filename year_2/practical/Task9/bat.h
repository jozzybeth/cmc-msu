#ifndef BAT_H
#define BAT_H

#include "bird.h"
#include "mammal.h"

//Летучая мышь
class Bat: public Bird, public Mammal {
    double speed;
public:
    Bat(const string& name = "No name", int age = 0, double weight = 0, double wingspan = 0, double speed = 0);
    void print() const;
    double popularity() const;
    ~Bat();
};

#endif 