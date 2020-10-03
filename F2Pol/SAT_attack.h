#pragma once

#include "f2pol.h"
#include "equations.h"

using namespace F2Pol;
using namespace Equations;

void print_equations_for_sat(const vector<Polynomial>& equations) {
    vector<int> vars = get_all_equation_vars(equations, false);
    for (int i = 0; i < vars.size(); i++) {
        cout << *all_vars[vars[i]] << (i < (int)vars.size() - 1 ? ";" : "\n");
    }
    for (const auto& it : equations) {
        cout << it << "\n";
    }
    cout << "\n";
}

const int inf = (int)1e9 + 1;
int to_int(const string& s) {
    int z = 1, val = 0, st = 0;
    if ((int)s.size() > 0 && s[0] == '-') {
        z = -1;
        st = 1;
    }
    if (st >= s.size()) {
        return -inf;
    }
    for (int i = st; i < s.size(); i++) {
        if (!(s[i] >= '0' && s[i] <= '9')) {
            return -inf;
        }
        val = val * 10 + (s[i] - '0');
    }
    return z * val;
}


vector<vector<int> > parse_solutions_from_sat(const string& file_name, int num_variables) {
    ifstream fin;
    fin.open(file_name);
    vector<vector<int> > solutions;
    vector<int> solution;
    string tmp;
    while (fin >> tmp) {
        if (tmp == "s" || tmp == "v" || tmp == "SATISFIABLE" || tmp == "UNSATISFIABLE") {
            continue;
        } else {
            int val = to_int(tmp);
            if (val == 0) {
                solution.resize(num_variables);
                solutions.push_back(solution);
                solution.clear();
            } else {
                solution.push_back(val > 0 ? 1 : 0);
            }
        }
    }
    return solutions;
}

void SAT_attack(const vector<Polynomial>& equations) {

    print_equations_for_sat(equations);

    /*
     * Wait for SAT solver to find solutions
     */

    vector<int> vars = get_all_equation_vars(equations);
    vector<vector<int> > solutions = parse_solutions_from_sat("sat_solutions.txt", vars.size());

    cout << "Solutions:\n";
    for (int i1 = 0; i1 < solutions.size(); i1++) {
        cout << "Solution: #" << i1 + 1 << "\n";
        for (int j1 = 0; j1 < vars.size(); j1++) {
            cout << *all_vars[vars[j1]] << " " << solutions[i1][j1] << "\n";
        }
    }
}

/*
python3 anf2cnf-converter.py CNF 3 SS SS input.txt output.txt
cryptominisat5 --verb 0 --maxsol 10 output.txt
 */