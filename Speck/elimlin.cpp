#include <iostream>
#include <fstream>
#include "F2Pol.cpp"

vector <vector<int>> elimlin(/*system source*/) {
    /*
    get the system
    create an empty array of polynomials D
    create an array of bools vars(size = number of unknown variables)
    linearize
    (*)
    get the matrix
    Gauss elimination
    get the polynomials fo the eliminated system
    for all equations in the system {
        if it's linear
            add it to D
            for term in equation {
                for(int i = 0; i < vars.size(); i++) {
                    if !vars[i] {
                        v = i-th variable
                        vars[i] = true
                        jump to (**)
                        }
                }
            }
            (**)
            get the v representation
            substitute v in all equations in D and in the current system
            delete the current linear equation from the system
    }
    if there are no linear equations {
        transform D into a vector<vector<int>>
        return it
        }
    jump to (*)
     */
}

int main() {

    return 0;
}
