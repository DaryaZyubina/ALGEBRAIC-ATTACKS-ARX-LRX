#pragma once

#include "f2pol.h"
#include "equations.h"
#include "gauss.h"

using namespace F2Pol;
using namespace Equations;

void XL_attack(vector<F2Pol::Polynomial> equations) {
    clock_t start = clock();

    // print some info
    cout << "Before multiplying by variables\n";
    vector<int> vars = get_all_equation_vars(equations, true);

    // apply XL
    multiply_by_variables(equations, vars);

    // print some info
    cout << "After multiplying by variables\n";
    get_all_equation_vars(equations, true);

    // linearize
    vector<vector<int> > matrix;
    vector<int> val_column;
    vector<F2Pol::Term*> terms;
    int num_columns = linearize(equations, matrix, val_column, terms, false, true);

    cout << matrix.size() << " " << num_columns << "\n";
    for (int i = 0; i < min(10, (int)matrix.size()); i++) {
        cout << matrix[i].size() << " ";
        for (int x : matrix[i]) {
            cout << x << " ";
        }
        cout << "\n";
    }
    if (matrix.size() > 10) {
        cout << "...\n";
    }
    /*for (int x : val_column) {
        cout << x << " ";
    }
    cout << "\n";
    for (F2Pol::Term* t : terms) {
        cout << *t << " ";
    }
    cout << "\n";*/
    cout << "\n";

    // solve linear system of equations
    Equation_system eq_system(matrix, num_columns, matrix.size());
    eq_system.find_solution();
    vector<vector<int> > solutions = eq_system.get_solutions();
    cout << "\n";

    solutions = remove_wrong_solutions(solutions, terms);
    cout << "Found " << solutions.size() << " solutions\n";
    for (const auto& it : solutions) {
        for (int x : it) {
            cout << x << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    cout << "\n";
    cout << "Overall attack time: " << (clock() - start) / CLOCKS_PER_SEC << "sec" <<"\n";
}