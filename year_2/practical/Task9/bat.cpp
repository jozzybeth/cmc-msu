#include "bat.h"

Bat::Bat(const string& name, int age, double weight, double wingspan, double speed)
    : Animal(name, age), Mammal(name, age, weight), Bird(name, age, wingspan)
{
    this->speed = speed;
}

double Bat::popularity () const
{
    return Bird::popularity() + Mammal::popularity() + speed*0.06;
}

void Bat::print() const
{
    cout << "Speed: " << speed << " m/s" << endl;
}

Bat::~Bat()
{
    cout << "Destructor Bat" << endl;
}


