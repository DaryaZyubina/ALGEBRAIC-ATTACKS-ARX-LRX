#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <math.h>

using namespace std;

const int LIMIT = 30;

class Equation_system {
    int length;
    int height;
    vector <vector<bool>> m;

    vector<vector <int>> solutions;

    void add_row(int row1, int row2) {
        for(int i = 0; i < length; i++) {
            m[row1][i] = m[row1][i] ^ m[row2][i];
        }
    }

public:

    vector<vector<int> > get_solutions() {
        return solutions;
    }
    /*
    Equation_system(string src) {
        ifstream file(src);
        if(!file.is_open()) {
            throw("Failed to open a read file!");
        }
        file >> length; file >> height;
        int c;
        vector <bool> v = {};
        m.push_back(v);
           for(int i = 0; i < height; i++) {
               m.push_back(v);
               for(int j = 0; j < length; j++) {
                   file >> c;
                   m[i].push_back(c);
               }
       }
        file.close();
    }
*/
    Equation_system(string src) {

        ifstream file(src);
        if(!file.is_open()) {
            throw("Failed to open a read file!");
        }

        file >> length; file >> height;

        vector <bool> v(length);
        for(int i = 0; i < height; i++) {
            m.push_back(v);
            int q, c; file >> q;
            for(int j = 0; j < q; j++) {
                file >> c;
                m[i][c] = 1;
            }
        }

        file.close();
    }

    Equation_system(vector<vector <int>> &mtrx, int l, int h) {
        length = l;
        height = h;

        vector <bool> v(length);
        for(int i = 0; i < height; i++) {
            m.push_back(v);
            for(int j = 0; j < mtrx[i].size(); j++) {
                m[i][mtrx[i][j]] = 1;
            }
        }
    }

    void print() {
        cout<<"Variables: "<<length-1<<"\nEquations = "<<height<<"\nCombinations = 2^"<<length-1-height<<'\n';
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < length-1; j++) {
                cout << m[i][j] << ' ';
            }
            cout << "= " << m[i][length-1] << '\n';
        }
    }

    void gauss_elim() {
        for(int k = 0, l = 0; l < length-1 && k < height;) {
            //if(l % 1000 == 0) cout << l << " columns\n";
            if(!m[k][l]) {
                for(int q = k+1; q < height; q++) {
                    if(m[q][l]) {
                        vector <bool> tmp = m[k];
                        m[k] = m[q]; m[q] = tmp;
                        break;
                    }
                }
            }
            if(m[k][l]) {
                for (int i = 0; i < height; i++) {
                    if (m[i][l] && i != k) {
                        add_row(i, k);
                    }
                }
                k++; l++;
            } else {
                l++;
            }
        }

        for(int i = 0; i < height; i++) {
            int non_zero = false;
            for(int j = 0; j < length-1; j++) {
                non_zero = non_zero or m[i][j];
            }
            if(!non_zero) {
                m.erase(m.begin() + i);
                height--; i--;
            }
        }
    }

    void find_solution() {
        gauss_elim();

        cout<<"Variables: "<<length-1<<"\nEquations = "<<height<<"\nCombinations = 2^"<<length-1-height<<'\n';

        if(length-1-height > LIMIT) {
            throw("Impossible enumaraition.");
        }

        int solutions_quantity = pow(2, length-1-height);

        solutions = vector<vector<int>>(solutions_quantity);
        for(int i = 0; i < solutions_quantity; i++) {
            solutions[i] = vector<int>(length-1);
        }

        vector<int> free_variables(length-1);
        for(int i = 0; i < length-1; i++) {
            free_variables[i] = true;
        }
        for(int i = 0, k = 0; i < height && k < length-1; i++, k++) {
            for(;!m[i][k]; k++);
            free_variables[k] = false;
        }

        vector <int> templ(length-1);
        for(int j = 0, l = 0; l < height && j < length-1; j++, l++) {
            while(free_variables[j]) {
                j++;
            }
            templ[j] = m[l][length-1];
        }
        for(int i = 0; i < solutions_quantity; i++) {
            solutions[i] = templ;
        }

        for(int i = 0; i < solutions_quantity; i++) {
            for(int j = 0, k = 0; j < length-1-height && k < length-1; j++, k++) {
                while(!free_variables[k]) {
                    k++;
                }
                solutions[i][k] = (i >> j)&1;

                for(int l = 0, p = 0; l < min(height, k) && p < length-1; l++, p++) {
                    while(free_variables[p]) {
                        p++;
                    }
                    if(m[l][k]) {
                        solutions[i][p] ^= solutions[i][k];
                    }
                }
            }
        }
    }

    void print_solution() {
        cout << "Solutions:\n";
        for(int j = 0; j < length-1; j++) {
            cout << 'x' << j+1 << ' ';
        }
        cout << '\n';
        for(int i = 0; i < solutions.size(); i++) {
            for(int j = 0; j < length-1; j++) {
                cout << solutions[i][j] << "  ";
            }
            cout << '\n';
        }
    }

    void write_solution(string dst) {
        ofstream file(dst);
        if(!file.is_open()) {
            throw("Failed to open a write file!");
        }

        for(int i = 0; i < solutions.size(); i++) {
            for(int j = 0; j < length-1; j++) {
                file << solutions[i][j] << " ";
            }
            file << "\n";
        }

        file.close();
    }

    void write_system(string dst) {
        ofstream file(dst);
        if(!file.is_open()) {
            throw("Failed to open a write file!");
        }

        file << length << ' ' << height << '\n';

        for(int i = 0; i < height; i++) {
            int counter = 0;
            for(int j = 0; j < length; j++) {
                if(m[i][j]) {
                    counter++;
                }
            }
            file << counter << ' ';
            for(int j = 0, written = 0; written < counter && j < length; j++) {
                if(m[i][j]) {
                    file << j << ' ';
                    written++;
                }
            }
            file << '\n';
        }

        file.close();
    }

    vector <vector <int>> get_system() {
        vector<vector <int>> result;
        vector<int> v;
        for(int i = 0; i < height; i++) {
            result.push_back(v);
            for(int j = 0; j < length; j++) {
                if(m[i][j]) {
                    result[i].push_back(j);
                }
            }
        }
        return result;
    }
};

namespace F2Pol {

    struct Polynomial;

    struct Variable {
        int id{};
        string name;
        Polynomial *value{};
        int int_value{};

        Variable() {};

        Variable(int _id, string _name, Polynomial *_value = NULL, int _int_value = -1) {
            id = _id;
            name = _name;
            value = _value;
            int_value = _int_value;
        }
    };

    ostream &operator<<(ostream &out, const Variable &v) {
        out << v.name;
        return out;
    }

    vector<Variable *> all_vars;

    int add_variable(string name, Polynomial *value = NULL, int int_value = -1) {
        int id = (int) all_vars.size();
        all_vars.push_back(new Variable(id, name, value, int_value));
        return id;
    }

    int zero = add_variable("0", NULL, 0);
    int one = add_variable("1", NULL, 1);

    struct Term {
        int id{};
        vector<int> var_ids;
        int last_eval_id = -2;
        Polynomial *last_eval_value = NULL;

        Term() {};

        Term(int _id, vector<int> _var_ids = {}) {
            id = _id;
            var_ids = _var_ids;
        }
    };

    ostream &operator<<(ostream &out, const Term &v) {
        //out << v.var_ids.size() << " ? ";
        for (int i = 0; i < (int) v.var_ids.size(); i++) {
            out << *all_vars[v.var_ids[i]] << (i < (int) v.var_ids.size() - 1 ? "*" : "");
        }
        return out;
    }

    bool operator==(const Term &t1, const Term &t2) {
        return t1.id == t2.id;
    }

    bool operator<(const Term &t1, const Term &t2) {
        return t1.id < t2.id;
    }

    vector<Term *> all_terms;
    map<vector<int>, int> term_to_id;
    map<int, Polynomial> temp_values = {};
    int eval_id = -1;

    Term *add_term(vector<int> _var_ids) {
        vector<int> var_list = {};
        sort(_var_ids.begin(), _var_ids.end());
        for (int _id : _var_ids) {
            if (all_vars[_id]->int_value == 0) {
                var_list = {zero};
                break;
            } else if (all_vars[_id]->int_value == 1) {
                continue;
            } else {
                int sz = (int) var_list.size();
                if (sz == 0 || _id != var_list[sz - 1]) {
                    var_list.push_back(_id);
                }
            }
        }
        if ((int) var_list.size() == 0) {
            var_list.push_back(one);
        }

        auto it = term_to_id.find(var_list);
        if (it == term_to_id.end()) {
            int _id = (int) all_terms.size();
            Term *new_term = new Term(_id, var_list);
            all_terms.push_back(new_term);
            term_to_id[var_list] = _id;
        }
        return all_terms[term_to_id[var_list]];
    }

    Term *one_term = add_term({});
    Term *zero_term = add_term({zero});

    Term *mult_terms(Term *a, Term *b) {
        vector<int> var_ids = {};
        for (int x : a->var_ids) {
            var_ids.push_back(x);
        }
        for (int x : b->var_ids) {
            var_ids.push_back(x);
        }
        return add_term(var_ids);
    }

    struct Polynomial {
        vector<Term *> terms;

        Polynomial() {}

        Polynomial(vector<Term *> _terms, bool need_clean = true) {
            if (need_clean) {
                sort(_terms.begin(), _terms.end(), [](Term *t1, Term *t2) { return *t1 < *t2; });
                for (Term *t : _terms) {
                    int sz = (int) terms.size();
                    if (t == zero_term) {
                        continue;
                    } else if (sz == 0 || terms[sz - 1]->id != t->id) {
                        terms.push_back(t);
                    } else {
                        terms.pop_back();
                    }
                }
            } else {
                terms = _terms;
            }
        }

        Polynomial(Polynomial const &p) {
            terms = p.terms;
        }
    };

    ostream &operator<<(ostream &out, const Polynomial &p) {
        if ((int) p.terms.size() == 0) {
            out << "0";
            return out;
        }
        for (int i = 0; i < (int) p.terms.size(); i++) {
            out << *p.terms[i] << (i < (int) p.terms.size() - 1 ? "+" : "");
        }
        return out;
    }

    Polynomial zero_pol = Polynomial(vector<Term*> ());
    Polynomial one_pol = Polynomial({one_term});

    Polynomial add_pol(const Polynomial &a, const Polynomial &b) {
        vector<Term *> terms;
        for (Term *x : a.terms) {
            terms.push_back(x);
        }
        for (Term *x : b.terms) {
            terms.push_back(x);
        }
        return Polynomial(terms);
    }

    Polynomial mult_pol(Polynomial a, Polynomial b) {
        vector<Term *> terms;
        for (Term *x : a.terms) {
            for (Term *y : b.terms) {
                terms.push_back(mult_terms(x, y));
            }
        }
        return Polynomial(terms);
    }


    bool is_zero_pol(const Polynomial &a) {
        return a.terms.size() == 0;
    }

    bool is_one_pol(const Polynomial &a) {
        return a.terms.size() == 1 && a.terms[0] == one_term;
    }

    bool is_linear_pol(const Polynomial &a) {
        for (Term* t : a.terms) {
            if (t->var_ids.size() > 1) {
                return false;
            }
        }
        return true;
    }


    Polynomial eval_pol(const Polynomial &a, bool recursive = false);


    Polynomial eval_term(Term *t, bool recursive = false) {
        if (t->last_eval_id == eval_id) {
            return *t->last_eval_value;
        }
        Polynomial res = one_pol;
        for (int _id : t->var_ids) {
            if (all_vars[_id]->int_value == 0) {
                res = zero_pol;
                break;
            } else if (all_vars[_id]->int_value == 1) {
                continue;
            } else if (temp_values.find(_id) != temp_values.end()) {
                res = mult_pol(res, eval_pol(temp_values[_id], recursive));
            } else {
                if (all_vars[_id]->value == NULL || !recursive) {
                    res = mult_pol(res, Polynomial({add_term({_id})}));
                } else {
                    res = mult_pol(res, eval_pol(*all_vars[_id]->value, recursive));
                }
            }
        }

        t->last_eval_id = eval_id;
        t->last_eval_value = new Polynomial(res);
        return res;
    }

    Polynomial eval_pol(const Polynomial &a, bool recursive) {
        vector<Term *> new_terms = {};
        for (Term *t : a.terms) {
            for (Term *nt : eval_term(t, recursive).terms) {
                new_terms.push_back(nt);
            }
        }
        return Polynomial(new_terms);
    }


    void circular_shift(vector<Polynomial> &s, int j) {
        if (j < 0) {
            j += (int) s.size();
        }
        rotate(s.begin(), s.begin() + j, s.end());
    }


    void log_xor_pos(vector<Polynomial> &s, int pos, char b) {
        if (b == '0') {
            return;
        }
        if (pos < 0) {
            pos += (int) s.size();
        }
        s[pos] = add_pol(s[pos], one_pol);
    }


    vector<Polynomial> log_xor(const vector<Polynomial> &a, const vector<Polynomial> &b) {
        assert((int) a.size() == (int) b.size());
        int sz = (int) a.size();
        vector<Polynomial> c = {};
        for (int i = 0; i < sz; i++) {
            c.emplace_back(add_pol(a[i], b[i]));
        }
        return c;
    }

    vector<Polynomial> log_and(const vector<Polynomial> &a, const vector<Polynomial> &b) {
        assert((int) a.size() == (int) b.size());
        int sz = (int) a.size();
        vector<Polynomial> c = {};
        for (int i = 0; i < sz; i++) {
            c.emplace_back(mult_pol(a[i], b[i]));
        }
        return c;
    }


    vector<Polynomial> log_not(const vector<Polynomial> &a) {
        int sz = (int) a.size();
        vector<Polynomial> c = {};
        for (int i = 0; i < sz; i++) {
            c.emplace_back(add_pol(a[i], one_pol));
        }
        return c;
    }


    string from_hex(const string &s) {
        string res;
        for (char c : s) {
            int _id = (c >= '0' && c <= '9') ? c - '0' : 10 + c - 'a';
            for (int i = 3; i >= 0; i--) {
                res += char('0' + ((_id >> i) & 1));
            }
        }
        return res;
    }


    string to_hex(const string &s) {
        assert((int) s.size() % 4 == 0);
        string res;
        for (int i = 0; i < (int) s.size(); i += 4) {
            int val = 0;
            for (int b = 0; b < 4; b++) {
                val += (1 << (3 - b)) * (s[i + b] == '1' ? 1 : 0);
            }
            if (val < 10) {
                res += char('0' + val);
            } else {
                res += char('a' + val - 10);
            }
        }
        return res;
    }


    string int_to_str(int x) {
        if (x == 0) {
            return "0";
        }
        string res;
        while (x > 0) {
            res += char('0' + x % 10);
            x /= 10;
        }
        reverse(res.begin(), res.end());
        return res;
    }


    string build_var_name(string name, int ind1, int ind2 = -1) {
        if (ind2 != -1) {
            return name + "[" + int_to_str(ind1) + "," + int_to_str(ind2) + "]";
        } else {
            return name + "[" + int_to_str(ind1) + "]";
        }
        name += "[" + int_to_str(ind1) + "]";
        if (ind2 != -1) {
            name += "[" + int_to_str(ind2) + "]";
        }
        return name;
    }

    void add_equation(const Polynomial &a, const Polynomial &b, vector<Polynomial> &equations) {
        equations.push_back(add_pol(a, b));
    }

    vector<Polynomial> get_equations(bool print_info = false) {
        vector<string> z = {"11111010001001010110000111001101111101000100101011000011100110",
                            "10001110111110010011000010110101000111011111001001100001011010",
                            "10101111011100000011010010011000101000010001111110010110110011",
                            "11011011101011000110010111100000010010001010011100110100001111",
                            "11010001111001101011011000100000010111000011001010010011101111"};
        int n = 16, m = 1, T = 4, j = 0;
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
                tmp = log_xor(tmp, k[i - 3]);
            }
            vector<Polynomial> tmp1 = tmp;
            circular_shift(tmp1, -1);
            tmp = log_xor(tmp, tmp1);

            k[i] = log_not(k[i - m]);
            k[i] = log_xor(k[i], tmp);
            log_xor_pos(k[i], -1, z[j][(i - m) % 62]);
            log_xor_pos(k[i], -1, '1');
            log_xor_pos(k[i], -2, '1');

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
            x[i + 2] = log_xor(x[i + 2], log_and(tmp0, tmp1));
            x[i + 2] = log_xor(x[i + 2], tmp2);
            x[i + 2] = log_xor(x[i + 2], k[i]);

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


    vector<int> get_all_equation_vars(const vector<Polynomial> &equations, bool print_info = false) {
        set<int> diff_var, diff_term;
        for (const Polynomial &eq : equations) {
            for (Term* t : eq.terms) {
                if (t->id != zero_term->id && t->id != one_term->id) {
                    diff_term.insert(t->id);
                }
                for (int _id : t->var_ids) {
                    if (_id != zero && _id != one) {
                        diff_var.insert(_id);
                    }
                }
            }
        }
        int num_equations = equations.size();
        int num_variables = diff_var.size();
        int num_terms = diff_term.size();
        if (print_info) {
            cout << "Number of variables: " << num_variables << endl;
            cout << "Number of equations: " << num_equations << endl;
            cout << "Number of terms    : " << num_terms << endl;
            cout << "\n";
        }

        vector<int> res;
        for (int x : diff_var) {
            res.push_back(x);
        }
        return res;
    }


    void multiply_by_variables(vector<Polynomial> &equations, const vector<int>& vars) {
        int basic_eq_num = equations.size();
        for (int _id : vars) {
            Polynomial pol_var({add_term({_id})});
            for (int i = 0; i < basic_eq_num; i++) {
                equations.push_back(mult_pol(equations[i], pol_var));
            }
        }
    }


    int linearize(const vector<Polynomial>& equations, vector<vector<int> > &matrix, vector<int> &val_column, vector<Term*> &terms, bool need_full_matrix= false, bool append_val_column= false) {
        int n = (int)equations.size();
        vector<vector<int> > compressed_matrix(n);
        matrix = vector<vector<int> >(n);
        val_column = vector<int>(n, 0);
        terms.clear();
        map<int, int> diff_terms;
        for (int i = 0; i < n; i++) {
            for (Term* t : equations[i].terms) {
                if (t->id != zero_term->id && t->id != one_term->id) {
                    if (diff_terms.find(t->id) == diff_terms.end()) {
                        diff_terms[t->id] = (int)terms.size();
                        terms.push_back(t);
                    }
                    compressed_matrix[i].push_back(diff_terms[t->id]);
                } else if (t->id == one_term->id) {
                    val_column[i] ^= 1;
                }
            }
        }
        int num_columns = (int)terms.size();
        if (append_val_column) {
            for (int i = 0; i < n; i++) {
                if (val_column[i] == 1) {
                    compressed_matrix[i].push_back(num_columns);
                }
            }
            terms.emplace_back(one_term);
            num_columns++;
        }
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

    vector<Polynomial> anti_linearize(const vector<Term*> &terms, const vector<vector<int> > &matrix, bool is_full_matrix=false) {
        vector<Polynomial> res;
        for (const vector<int> &row : matrix) {
            vector<Term*> term_row = {};
            if (is_full_matrix) {
                for (int j = 0; j < row.size(); j++) {
                    if (row[j] == 1) {
                        term_row.push_back(terms[j]);
                    }
                }
            } else {
                for (int p : row) {
                    term_row.push_back(terms[p]);
                }
            }
            res.emplace_back(Polynomial(term_row));
        }
        return res;
    }

    vector<vector<int> > remove_wrong_solutions(const vector<vector<int> >& solutions, const vector<Term*>& terms) {
        vector<vector<int> > res;
        for (vector<int> solution : solutions) {
            temp_values.clear();
            eval_id += 1;

            for (int i = 0; i < terms.size(); i++) {
                if (terms[i]->var_ids.size() == 1) {
                    temp_values[terms[i]->var_ids[0]] = solution[i] == 1 ? one_pol : zero_pol;
                }
            }
            bool ok = true;
            for (int i = 0; i < terms.size(); i++) {
                Polynomial value = eval_term(terms[i], true);
                if (value.terms.size() == 0) {
                    if (solution[i] != 0) {
                        ok = false;
                        break;
                    }
                } else if (value.terms.size() == 1 && value.terms[0]->id == one_term->id) {
                    if (solution[i] != 1) {
                        ok = false;
                        break;
                    }
                } else {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                res.push_back(solution);
            }
        }
        return res;
    }

}


void Simon_XL() {
    // get all equations
    auto equations = F2Pol::get_equations(false);

    clock_t start = clock();

    // get all variables in these equations
    auto vars = F2Pol::get_all_equation_vars(equations, false);

    // print equations to be passed to a anf2cnf converter
    for (int i = 0; i < vars.size(); i++) {
        cout << *F2Pol::all_vars[vars[i]] << (i < (int)vars.size() - 1 ? ";" : "\n");
    }
    for (auto it : equations) {
        // cout << it << "   " << F2Pol::is_linear_pol(it) << "\n";
        cout << it << "\n";
    }
    cout << "\n";

    // print some info
    F2Pol::get_all_equation_vars(equations, true);

    // apply XL
    F2Pol::multiply_by_variables(equations, vars);

    // print some info
    F2Pol::get_all_equation_vars(equations, true);

    // linearize
    vector<vector<int> > matrix;
    vector<int> val_column;
    vector<F2Pol::Term*> terms;
    int num_columns = F2Pol::linearize(equations, matrix, val_column, terms, false, true);
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
    for (int x : val_column) {
        cout << x << " ";
    }
    cout << "\n";
    for (F2Pol::Term* t : terms) {
        cout << *t << " ";
    }
    cout << "\n";
    cout << "\n";

    // test anti_linearize
    /*equations = F2Pol::anti_linearize(terms, matrix);
    for (auto it : equations) {
        cout << it << "\n";
    }*/

    // solve linear system of equations
    Equation_system eq_system(matrix, num_columns, matrix.size());
    eq_system.find_solution();
    vector<vector<int> > solutions = eq_system.get_solutions();
    /*// reading all solutions
    ifstream fin;
    fin.open("solutions.txt");
    vector<vector<int> > solutions;
    int tmp;
    while (fin >> tmp) {
        solutions.emplace_back(vector<int> ());
        solutions.back().push_back(tmp);
        for (int i = 0; i < (int)terms.size() - 1; i++) {
            fin >> tmp;
            solutions.back().push_back(tmp);
        }
    }
    fin.close();*/
    cout << "\n";
    solutions = F2Pol::remove_wrong_solutions(solutions, terms);
    cout << "Found " << solutions.size() << " solutions\n";
    for (auto it : solutions) {
        for (int x : it) {
            cout << x << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    cout << "\n";
    cout << "Overall attack time: " << clock() - start << "\n";
}


int main() {
    Simon_XL();
    return 0;
}


/*
python3 anf2cnf-converter.py CNF 3 SS SS input.txt output.txt
cryptominisat5 --verb 0 --maxsol 10 output.txt
 */
