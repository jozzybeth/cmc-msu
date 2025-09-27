#include "mammal.h"

Mammal::Mammal(const string& name, int age, double weight) : Animal(name, age)
{
    this->weight = weight;
}

double Mammal::popularity() const
{
    return weight * 0.1;
}

void Mammal::print() const 
{
    cout << "Weight: " << weight << " kg" << endl;
}

Mammal::~Mammal()
{
    cout << "Destructor Mammal" << endl;
}
