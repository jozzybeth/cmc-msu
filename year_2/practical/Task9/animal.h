#ifndef ANIMAL_H
#define ANIMAL_H

#include <iostream>
#include <string>
using namespace std;

//Животное
class Animal {
    string name;
    int age; 
public:
    Animal(const string& name, int age);
    //вычисление популярности
    virtual double popularity() const = 0;
    virtual void print() const; 
    ~Animal();
};

#endif 
