#include <iostream>
#include "animal.h"
#include "mammal.h"
#include "bird.h"
#include "bat.h"
#define N 3
using namespace std;

int main() {
    Animal *p[N];
    double complete_popularity = 0;
    Mammal a("Lion", 3, 10);
    Bird b("Teterev",5, 40);
    Bat c("Vampyrus", 2, 1, 1.5, 8);
    p[0] = &a;
    p[1] = &b;
    p[2] = &c;
    Animal *d = new Bat("abc",2,3,4,5);
    cout << "Popular Bat: " << c.popularity() << endl;
    cout << "Popular Mammal: " << a.popularity() << endl;
    cout << "Popular Bird: " << b.popularity() << endl;

    for (int i = 0; i < N; i++)
        complete_popularity += p[i]->popularity();

    cout << "complete_popularity = " << complete_popularity << endl;

    for (int i=0; i < N; i++)
        p[i]->print();
    
    delete d;
    return 0;
}
