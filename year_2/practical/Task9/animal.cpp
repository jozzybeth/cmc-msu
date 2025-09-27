#include "animal.h"

Animal::Animal(const string& name, int age)
{        
    this->name = name;
    this->age = age;
}

void Animal::print() const
{
    cout << "Animal: " << name << ", Age: " << age << endl;
}

Animal::~Animal()
{
    cout << "Destructor Animal" << endl;
}

