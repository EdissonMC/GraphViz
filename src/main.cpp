#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include "SimpleGraph.h"

using namespace std;

void Welcome();



int main() {
    Welcome();

    return 0;
}

void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}



