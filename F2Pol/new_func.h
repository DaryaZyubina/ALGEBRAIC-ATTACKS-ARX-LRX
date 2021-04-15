#pragma once

#include "f2pol.h"
#include "equations.h"

int get_vars_in_equations(const vector<Polynomial>& equations, vector<vector<int> > &matrix, vector<int> &vars, bool need_full_matrix=true) {
    int n = (int)equations.size();
    vector<vector<int> > compressed_matrix(n);
    matrix = vector<vector<int> >(n);
    vars.clear();
    map<int, int> diff_vars;
    for (int i = 0; i < n; i++) {
        for (Term* t : equations[i].terms) {
            for (int var_id : t->var_ids) {
                if (diff_vars.find(var_id) == diff_vars.end()) {
                    diff_vars[var_id] = (int)vars.size();
                    vars.push_back(var_id);
                }
            }
        }
    }
    sort(vars.begin(), vars.end());
    for (int i = 0; i < vars.size(); i++) {
        diff_vars[vars[i]] = i;
    }
    for (int i = 0; i < n; i++) {
        for (Term* t : equations[i].terms) {
            for (int var_id : t->var_ids) {
                compressed_matrix[i].push_back(diff_vars[var_id]);
            }
        }
        sort(compressed_matrix[i].begin(), compressed_matrix[i].end());
        compressed_matrix[i].resize(unique(compressed_matrix[i].begin(), compressed_matrix[i].end()) - compressed_matrix[i].begin());
    }
    int num_columns = (int)vars.size();
    if (need_full_matrix) {
        for (int i = 0; i < n; i++) {
            matrix[i] = vector<int>(num_columns, 0);
            for (int x : compressed_matrix[i]) {
                matrix[i][x] = 1;
            }
        }
    } else {
        matrix = std::move(compressed_matrix);
    }
    return num_columns;
}

void solve_equations(const vector<Polynomial>& equations, vector<vector<int> > &solutions, vector<int> &vars) {
    vector<vector<int> > matrix;
    get_vars_in_equations(equations, matrix, vars);
    solutions.clear();
    int cnt_vars = (int)vars.size();
    for (int mask = 0; mask < (1 << cnt_vars); mask++) {
        temp_values.clear();
        eval_id += 1;

        bool bad = false;
        for (int i = 0; i < cnt_vars; i++) {
            if (vars[i] == zero && ((mask >> i) & 1) != 0) {
                bad = true;
                break;
            }
            if (vars[i] == one && ((mask >> i) & 1) != 1) {
                bad = true;
                break;
            }
            temp_values[vars[i]] = ((mask >> i) & 1) == 1 ? one_pol : zero_pol;
        }
        if (bad) {
            continue;
        }
        for (auto eq : equations) {
            Polynomial value = eval_pol(eq, true);
            if (value.terms.size() != 0) {
                bad = true;
                break;
            }
        }
        if (!bad) {
            vector<int> solution;
            for (int i = 0; i < cnt_vars; i++) {
                solution.push_back((mask >> i) & 1);
            }
            solutions.push_back(solution);
        }
    }
}