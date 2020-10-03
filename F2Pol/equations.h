#pragma once

#include "f2pol.h"

/*
 * Работа с уравнениями.
 * Уравнение - это объект типа Polynomial, что означает что этот многочлен равен нулю
 */

namespace Equations {

    using namespace F2Pol;

    /*
     * шестнадцатеричная строка -> двоичная строка
     */
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

    /*
     * двоичная строка -> шестнадцатеричная строка
     */
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

    /*
     * Число в строку
     */
    string int_to_str(int x) {
        if (x == 0) {
            return "0";
        }
        string res;
        bool is_neg = false;
        if (x < 0) {
            is_neg = true;
            x = -x;
        }
        while (x > 0) {
            res += char('0' + x % 10);
            x /= 10;
        }
        reverse(res.begin(), res.end());
        if (is_neg) {
            res = "-" + res;
        }
        return res;
    }

    /*
     * Добавление индексов к названию переменной
     * Можно добавит не более двух индексов
     * build_var_name("x", -1, -1) = "x"
     * build_var_name("x", 1, -1) = "x[1]"
     * build_var_name("x", 1, 2) = "x[1,2]"
     */
    string build_var_name(string name, int ind1 = -1, int ind2 = -1) {
        if (ind1 != -1 && ind2 != -1) {
            return name + "[" + int_to_str(ind1) + "," + int_to_str(ind2) + "]";
        } else if (ind1 != -1) {
            return name + "[" + int_to_str(ind1) + "]";
        } else if (ind2 != -1) {
            return name + "[" + int_to_str(ind2) + "]";
        } else {
            return name;
        }
    }

    /*
     * Добавление в массив уравнений вида многочлен=0 уравнения a=b (в массив добавится элемент a+b, так как
     * a=b <=> a+b=0)
     */
    void add_equation(const Polynomial &a, const Polynomial &b, vector<Polynomial> &equations) {
        equations.push_back(add_pol(a, b));
    }

    /*
     * Вывод уравнений
     */
    void print_equations(const vector<Polynomial> &equations, bool print_equation_count= false) {
        if (print_equation_count) {
            cout << (int)equations.size() << " equations \n";
        }
        for (const Polynomial &p : equations) {
            cout << p << "\n";
        }
        cout << "\n";
    }

    /*
     * Получить список идентификаторов всех переменных, встречающиеся в массиве уравнений
     */
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

    /*
     * Вывести информацию о количество уравнения, различных переменных (без учета 0 и 1), различных мономов в системе
     * уравнений
     */
    void print_equations_info(const vector<Polynomial> &equations) {
        get_all_equation_vars(equations, true);
    }

    /*
     * Поумножать все уравнения системы на переменные (заданные массивом идентификаторов) и добавить новые
     * уравнения в систему
     */
    void multiply_by_variables(vector<Polynomial> &equations, const vector<int>& vars) {
        int basic_eq_num = equations.size();
        for (int _id : vars) {
            Polynomial pol_var({add_term({_id})});
            for (int i = 0; i < basic_eq_num; i++) {
                equations.push_back(mult_pol(equations[i], pol_var));
            }
        }
    }

    /*
     * Линеаризация системы уравнений.
     * matrix - куда будет помещена матрица с ответом. Строки - уравнения, столбцы - мономы
     * каждая строка соотвествует уравнению. Если need_full_matrix=true, то в каждой строке столько же элементов,
     * сколько всего мономов, встречающиеся в этой системе уравнений. Если моном есть в этом уравнении,
     * то значение в соответствующей ячейке равно 1, иначе 0.
     * Если need_full_matrix=false, то в каждой строке записаны номера столбцов (в которых с полной матрице стояли бы 1)
     *
     * val_column - куда будет помещен столбец из 0 и 1
     * terms - список мономов в том порядке, в котором им сопоставлены столбцы матрицы
     *
     * Если append_val_column=false, то в списке terms нет монома one_term (и соотвественно в матрице накак не отражено
     * есть ли в уравнении +1), зато в столбце val_column будет стоять 0 или 1
     *
     * Если append_val_column=true, то one_term считается таким же мономом как и все, а val_column состоит из нулей.
     * В текущей реализации мономы сортируются по убыванию степени, так что последний столбец в этом случае
     * соотвествует one_term, что соответствует матрице при решении системы линейных уравнений, к которой
     * присоединили столбец значений. Так что это может быть удобно для дальнейшего использования метода Гаусса.
     */
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
                }
            }
        }
        sort(terms.begin(), terms.end(), [](Term* t1, Term* t2) {
            int d1 = t1->degree(), d2 = t2->degree();
            if (d1 != d2) {
                return d1 > d2;
            } else {
                return t1->id < t2->id;
            }
        });
        for (int i = 0; i < terms.size(); i++) {
            diff_terms[terms[i]->id] = i;
        }
        for (int i = 0; i < n; i++) {
            for (Term* t : equations[i].terms) {
                if (t->id != zero_term->id && t->id != one_term->id) {
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
                val_column[i] = 0;
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

    /*
     * Обратная функция, которая по матрице и списоку terms (который показывает какие мономы соотвествуют
     * каким столбцам) возвращает систему уравнений. is_full_matrix говорит, является переданная матрица сжатой
     * в том смысле, что в функции linearize
     *
     * В текущей реализации не поддерживается передача и столбца val_column из 0 и 1, чтобы в нужных местах
     * добавить к уравнениям +1. Так что в текущей реализации эта функция является обратной к функции linearize только
     * в случае если linearize была вызвана с append_val_column=true
     */
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

    /*
     * Принимает набор решений линейной системы и проверяет, удовлетворяют они дополнительным нелинейным соотношениям.
     * То есть если решение линеаризованной системы приписывает мономам x и y значение 0, а моному x*y значение 1, то
     * это неправильное решение и оно будет отсеяно. Возвращаются только те решения, которые прошли проверку.
     *
     * Массив terms имеет такую же длину как каждый элемент solutions и содержит мономы, которым приписаны значения
     * Чаще всего terms берется из результата работы функции linearize
     *
     * Важно, что для правильной проверки среди мономов должны быть все мономы, состоящие из одной переменной
     * Например, функция не сможет проверить, что решение сопоставляющее x*y=1, x*z=1, y*z=0 - неправильное
     */
    vector<vector<int> > remove_wrong_solutions(const vector<vector<int> >& solutions, const vector<Term*>& terms) {
        vector<vector<int> > res;
        for (vector<int> solution : solutions) {
            temp_values.clear();
            eval_id += 1;

            for (int i = 0; i < terms.size(); i++) {
                if (terms[i] == one_term) {
                    assert(i == (int)terms.size() - 1 && (int)terms.size() == (int)solution.size() + 1);
                    continue;
                }
                if (terms[i]->var_ids.size() == 1) {
                    temp_values[terms[i]->var_ids[0]] = solution[i] == 1 ? one_pol : zero_pol;
                }
            }
            bool ok = true;
            for (int i = 0; i < terms.size(); i++) {
                if (terms[i] == one_term) {
                    assert(i == (int)terms.size() - 1 && (int)terms.size() == (int)solution.size() + 1);
                    continue;
                }
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

    /*
     * Берет название переменной и превращает его в формат a[1,2,3]. Например, a_1_2_3 превратит в a[1,2,3]
     * Переменные в таком формате могут быть нужны SAT-solver
     */
    string change_name(const string& s) {
        if (s == "0" || s == "1") {
            return s;
        }
        char letter = 0;
        string cur_num;
        vector<string> nums;
        for (char c : s) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                assert(letter == 0);
                letter = c;
            } else if (c >= '0' && c <= '9') {
                cur_num += c;
            } else {
                if (cur_num.size() > 0) {
                    nums.push_back(cur_num);
                    cur_num = "";
                }
            }
        }
        assert(letter != 0);
        string res;
        res += letter;
        if (nums.size() > 0) {
            res += '[';
            for (int i = 0; i < nums.size(); i++) {
                res += nums[i];
                if (i + 1 < nums.size()) {
                    res += ',';
                } else {
                    res += ']';
                }
            }
        }
        return res;
    }

    /*
     * Считывает все уравнения из файла. В уравнениях должны использоваться знаки +,*,=.
     * Мономы могут находиться по разную сторону от =.
     * Подобные приводятся, возвращается массив Polynomial.
     * Например, если в файле есть строка
     * x*y + 1 * x*y = z + 0
     * то в массив добавтся многочлен z, что означает, что z=0
     *
     * если change_names=true, то названия переменных будут заменяться с помощью change_name
     */
    vector<Polynomial> parse_equations_from_file(const string& file_name, bool change_names=false) {
        ifstream fin;
        fin.open(file_name);
        string s;
        vector<Polynomial> equations;
        map<string, int> parsed_vars;
        while (getline(fin, s)) {
            vector<Term*> terms;
            parsed_vars["0"] = zero;
            parsed_vars["1"] = one;
            string cur_var_name;
            vector<int> cur_term;
            auto append_char = [&](char c) {
                cur_var_name += c;
            };
            auto end_var_name = [&]() {
                if (cur_var_name.size() == 0) {
                    return;
                }
                if (change_names) {
                    cur_var_name = change_name(cur_var_name);
                }
                if (parsed_vars.find(cur_var_name) == parsed_vars.end()) {
                    parsed_vars[cur_var_name] = add_variable(cur_var_name);
                }
                cur_term.push_back(parsed_vars[cur_var_name]);
                cur_var_name = "";
            };
            auto end_term = [&]() {
                end_var_name();
                if (cur_term.size() == 0) {
                    return;
                }
                terms.push_back(add_term(cur_term));
                cur_term.clear();
            };
            for (int i = 0; i < s.size(); i++) {
                if (s[i] == ' ' || s[i] == '\n' || s[i] == '\r' || s[i] == '\t') {
                    end_var_name();
                } else if (s[i] == '+' || s[i] == '=') {
                    end_term();
                } else if (s[i] == '*') {
                    end_var_name();
                } else {
                    append_char(s[i]);
                }
            }
            end_term();
            equations.emplace_back(Polynomial(terms));
        }
        fin.close();
        return equations;
    }
}