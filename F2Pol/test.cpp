#include <vector>

#include "f2pol.h"
#include "equations.h"
#include "XL_attack.h"
#include "buchberger.h"
#include "SAT_attack.h"
#include "new_func.h"

using namespace F2Pol;
using namespace Equations;

vector<Polynomial> get_equations(bool print_info = false) {
    vector<string> z = {"11111010001001010110000111001101111101000100101011000011100110",
                        "10001110111110010011000010110101000111011111001001100001011010",
                        "10101111011100000011010010011000101000010001111110010110110011",
                        "11011011101011000110010111100000010010001010011100110100001111",
                        "11010001111001101011011000100000010111000011001010010011101111"};
    int n = 16, m = 1, T = 2, j = 0;
    vector<string> key = {"1011001011111110", "0111110010010111", "1010011100110100", "1000101001111111"};
    string I_L = from_hex("6565"), I_R = from_hex("6877");
    vector<Polynomial> equations;

    vector<vector<int> > k_var(max(T, m));
    vector<vector<Polynomial> > k(max(T, m));
    for (int i = 0; i < m; i++) {
        for (int j1 = 0; j1 < n; j1++) {
            int value = key[i][j1] == '1' ? 1 : 0;
            k_var[i].push_back(add_variable(build_var_name("k", i, j1), NULL));
            k[i].push_back(Polynomial({add_term({k_var[i][j1]})}));
        }
    }

    for (int i = m; i < T; i++) {
        vector<Polynomial> tmp = k[i - 1];
        circular_shift(tmp, -3);
        if (m == 4) {
            tmp = bin_xor(tmp, k[i - 3]);
        }
        vector<Polynomial> tmp1 = tmp;
        circular_shift(tmp1, -1);
        tmp = bin_xor(tmp, tmp1);

        k[i] = bin_not(k[i - m]);
        k[i] = bin_xor(k[i], tmp);
        if (z[j][(i - m) % 62] == '1') {
            flip_pos(k[i], -1);
        }
        flip_pos(k[i], -1);
        flip_pos(k[i], -2);

//            for (int j1 = 0; j1 < n; j1++) {
//                Polynomial* value = new Polynomial(k[i][j1]);
//                k_var[i].push_back(add_variable(build_var_name("k", i, j1), value));
//                // cout << *all_vars[k_var[i][j1]] << "=" << *value << endl;
//                k[i][j1] = Polynomial({add_term({k_var[i][j1]})});
//
//                add_equation(k[i][j1], *value, equations);
//            }
//            // cout << "\n";
    }

    vector<vector<int> > x_var(T + 2);
    vector<vector<Polynomial> > x(T + 2);
    for (int j1 = 0; j1 < n; j1++) {
        int int_value = I_R[j1] == '1' ? 1 : 0;
        x_var[0].push_back(add_variable(build_var_name("x", 0, j1), NULL, int_value));
        // cout << *all_vars[x_var[0][j1]] << "=" << int_value << endl;
        x[0].push_back(Polynomial({add_term({x_var[0][j1]})}));
    }
    for (int j1 = 0; j1 < n; j1++) {
        int int_value = I_L[j1] == '1' ? 1 : 0;
        x_var[1].push_back(add_variable(build_var_name("x", 1, j1), NULL, int_value));
        // cout << *all_vars[x_var[1][j1]] << "=" << int_value << endl;
        x[1].push_back(Polynomial({add_term({x_var[1][j1]})}));
    }


    for (int i = 0; i < T; i++) {
        x[i + 2] = x[i];
        vector<Polynomial> tmp0 = x[i + 1]; circular_shift(tmp0, 1);
        vector<Polynomial> tmp1 = x[i + 1]; circular_shift(tmp1, 8);
        vector<Polynomial> tmp2 = x[i + 1]; circular_shift(tmp2, 2);
        x[i + 2] = bin_xor(x[i + 2], bin_and(tmp0, tmp1));
        x[i + 2] = bin_xor(x[i + 2], tmp2);
        x[i + 2] = bin_xor(x[i + 2], k[i]);

        for (int j1 = 0; j1 < n; j1++) {
            Polynomial* value = new Polynomial(x[i + 2][j1]);
            x_var[i + 2].push_back(add_variable(build_var_name("x", i + 2, j1), value));
            // cout << *all_vars[x_var[i + 2][j1]] << "=" << *value << endl;
            x[i + 2][j1] = Polynomial({add_term({x_var[i + 2][j1]})});

            add_equation(x[i + 2][j1], *value, equations);
        }
        // cout << "\n";
    }

    temp_values.clear();
    eval_id += 1;

    for (int i = 0; i < m; i++) {
        for (int j1 = 0; j1 < n; j1++) {
            int value = key[i][j1] == '1' ? 1 : 0;
            temp_values[k_var[i][j1]] = value == 1 ? one_pol : zero_pol;
        }
    }

    string resL, resR;

    for (int i = 0; i < n; i++) {
        Polynomial value = eval_pol(x[T + 1][i], true);
        if ((int)value.terms.size() == 0) {
            resL += '0';
        } else if ((int)value.terms.size() == 1 && value.terms[0] == one_term) {
            resL += '1';
        } else {
            assert(false);
        }
    }
    for (int i = 0; i < n; i++) {
        Polynomial value = eval_pol(x[T][i], true);
        if ((int)value.terms.size() == 0) {
            resR += '0';
        } else if ((int)value.terms.size() == 1 && value.terms[0] == one_term) {
            resR += '1';
        } else {
            assert(false);
        }
    }

    if (print_info) {
        cout << resL << " " << resR << endl;
        cout << to_hex(resL) << " " << to_hex(resR) << endl;
        cout << endl;
    }

    temp_values.clear();
    eval_id += 1;

    for (int j1 = 0; j1 < n; j1++) {
        int value = resR[j1] == '1' ? 1 : 0;
        temp_values[x_var[T][j1]] = value == 1 ? one_pol : zero_pol;
    }
    for (int j1 = 0; j1 < n; j1++) {
        int value = resL[j1] == '1' ? 1 : 0;
        temp_values[x_var[T + 1][j1]] = value == 1 ? one_pol : zero_pol;
    }

    for (int i = 0; i < (int)equations.size(); i++) {
        equations[i] = eval_pol(equations[i], false);
    }

    if (print_info) {
        cout << "Equations\n";
        for (auto item : equations) {
            cout << item << endl;
        }
        cout << "\n";
    }

    return equations;
}

void test_get_equation_vars() {
    vector<Polynomial> equations = get_equations();

    vector<vector<int> > matrix;
    vector<int> vars;
    get_vars_in_equations(equations, matrix, vars);
    for (auto eq : equations) {
        cout << eq << "\n";
    }
    cout << "\n";
    int cnt_vars = (int)vars.size();
    for (int _id : vars) {
        cout << *F2Pol::all_vars[_id] << " ";
    }
    cout << "\n";
    for (auto it : matrix) {
        for (int i : it) {
            cout << i << " ";
        }
        cout << "\n";
    }
}

void test_equations_solver() {
//    vector<Polynomial> equations = parse_equations_from_file("test_eq.txt");
    vector<Polynomial> equations = get_equations();
    vector<vector<int> > solutions;
    vector<int> vars;
    solve_equations(equations, solutions, vars);

    for (int _id : vars) {
        cout << *F2Pol::all_vars[_id] << " ";
    }
    cout << "\n";
    for (auto it : solutions) {
        for (int i : it) {
            cout << i << " ";
        }
        cout << "\n";
    }
}

int main() {
    test_get_equation_vars();
    test_equations_solver();

}