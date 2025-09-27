#include "bird.h"

Bird::Bird(const string& name, int age, double wingspan): Animal(name, age)
{
    this->wingspan = wingspan;
}

double Bird::popularity() const 
{
    return wingspan * 0.05;
}

void Bird::print() const 
{
    cout << "Wingspan: " << wingspan << " cm" << endl;
}

Bird::~Bird()
{
    cout << "Destructor Bird" << endl;
}
