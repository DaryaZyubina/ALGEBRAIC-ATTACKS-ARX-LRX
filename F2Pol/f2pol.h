#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace F2Pol {

    using namespace std;

    struct Polynomial;

    /*
     * Структура "переменная"
     * id - уникальный идентификатор
     * name - название переменной (например "x1")
     * int_value - только для двух переменных "0" и "1" соответственно равно 0 и 1
     * value - способ указать, что переменная заменяет собой некое выражение (многочлен)
     * например, если нужно заменить выражение x+y на новую переменную z, то у переменной z
     * value будет ссылаться на многочлен x+y. Если переменная новая, то value равен NULL
     */
    struct Variable {
        int id{};
        string name;
        Polynomial *value{};
        int int_value{};

        Variable() {};

        /*
         * Не нужно напрямую вызывать конструктор, нужно использовать функцию add_variable
         */
        Variable(int _id, string _name, Polynomial *_value = NULL, int _int_value = -1) {
            id = _id;
            name = _name;
            value = _value;
            int_value = _int_value;
        }
    };

    /*
     * Вывод переменной, печатается ее название, без переноса строки
     */
    ostream &operator<<(ostream &out, const Variable &v) {
        out << v.name;
        return out;
    }

    /*
     * Текущий список всех переменных, созданных на данный момент выполнения программы
     */
    vector<Variable *> all_vars;

    /*
     * Нужно использовать эту функцию, чтобы создавать новые переменные. Автоматически добавляет
     * созданную переменную в спискок all_vars
     */
    int add_variable(string name, Polynomial *value = NULL, int int_value = -1) {
        int id = (int) all_vars.size();
        all_vars.push_back(new Variable(id, name, value, int_value));
        return id;
    }

    /*
     * Переменные zero и one, обозначающие 0 и 1 соотвественно. Никакие другие переменные не должны иметь
     * int_value не равный -1
     */
    int zero = add_variable("0", NULL, 0);
    int one = add_variable("1", NULL, 1);

    /*
     * Структура "моном" (слово терм обычно немного другое обозначает, но ладно)
     * Всегда соответствует произведению переменных, например x1 * y * k
     * id - уникальный идентификатор
     * var_ids - массив идентификаторов переменных, входящих в моном, без повторений (так как x*x=x)
     * отсортирован по возрастанию идентификаторов переменных
     * есть выделенные мономы 0 и 1, в первом случае var_ids = {zero}, во втором var_ids = {one}
     * var_ids не может быть пустым. За исключением двух случаев выше, в него не могут входить переменные zero и one.
     * Но переменные, заменяющие некоторое выражение (то есть с value != NULL) могут входить в моном
     *
     * last_eval_id, last_eval_value - переменные для кеширования вычисления значения монома при означивании
     * некоторых переменных
     */
    struct Term {
        int id{};
        vector<int> var_ids;
        int last_eval_id = -2;
        Polynomial *last_eval_value = NULL;

        Term() {};

        /*
         * Не нужно напрямую вызывать конструктор, нужно использовать функцию add_term
         */
        Term(int _id, vector<int> _var_ids = {}) {
            id = _id;
            var_ids = _var_ids;
        }

        /*
         * Степень монома - число переменных. Степень 0 и 1 равна 0
         */
        int degree() {
            if (var_ids.size() >= 2) {
                return var_ids.size();
            } else if (var_ids.size() == 1) {
                if (var_ids[0] == zero || var_ids[0] == one) {
                    return 0;
                } else {
                    return 1;
                }
            } else {
                assert(false);
            }
        }
    };

    /*
     * Вывод монома в формате x*y*z. Без перевода строки
     */
    ostream &operator<<(ostream &out, const Term &v) {
        for (int i = 0; i < (int) v.var_ids.size(); i++) {
            out << *all_vars[v.var_ids[i]] << (i < (int) v.var_ids.size() - 1 ? "*" : "");
        }
        return out;
    }

    /*
     * Проверка равенства мономов. Если создавать мономы только через функцию add_term, то кажый
     * моном будет существовать в единственном экземпляре. Поэтому достаточно сравнить id.
     */
    bool operator==(const Term &t1, const Term &t2) {
        return t1.id == t2.id;
    }

    /*
     * Сравнение мономов с помощью grevlex
     */
    bool grevlex(Term &t1, Term &t2) {
        int d1 = t1.degree(), d2 = t2.degree();
        if (d1 != d2) {
            return d1 < d2;
        }
        for (int i = 0; i < t1.var_ids.size(); i++) {
            int v1 = t1.var_ids[i], v2 = t2.var_ids[i];
            if (v1 != v2) {
                return v1 < v2;
            }
        }
        return false;
    }

    /*
     * Текущий способ сравнения мономов. Можно изменять на нужный.
     */
    bool operator<(Term &t1, Term &t2) {
        return t1.id < t2.id;
        //return grevlex(t1, t2);
    }

    /*
     * all_terms - текущий список всех мономов, созданных на данный момент выполнения программы
     * term_to_id - map из отсортированных наборов переменных в идентификаторы мономов
     * (чтобы не создавать копии одинаковых мономов)
     *
     * temp_values - означивание переменных. Некоторым переменным по из идентификатору можно сопоставит многочлен.
     * Переменные, у которых value != NULL означивать нельзя. Означивание это только добавление значения переменной
     * в temp_values. Присваивать многочлены в поле value у переменной не надо.
     * eval_id - номер текущего означивания. После изменения temp_values нужно eval_id увеличить на 1
     */
    vector<Term *> all_terms;
    map<vector<int>, int> term_to_id;
    map<int, Polynomial> temp_values = {};
    int eval_id = -1;

    /*
     * Функция, принимающая массив переменных и создающая моном, соответствующий их произведению.
     * Если такой моном уже существует, новый не будет создан, а вернется ссылка на существующий
     * Переменные можно передавать и с повторениями, и в неотсортированном порядке
     * Если среди переменных есть one, то он игнорируется (за исключением если кроме one больше нет переменных)
     * Если одна из переменных это zero, то вернется моном 0 с var_ids = {zero}
     */
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

    /*
     * Мономы zero_term и one_term.
     * zero_term - это произведение одной переменной zero
     * one_term - это произведение одной переменной one
     */
    Term *one_term = add_term({});
    Term *zero_term = add_term({zero});

    /*
     * Умножение мономов. Принимает два монома, возвращает произведение
     * Приводит "подобные"
     * Примеры:
     * mult_terms("x*y", "0") = "0"
     * mult_terms("1", "x*a*b") = "x*a*b"
     * mult_terms("a*b*c", "a*b*d") = "a*b*c*d"
     */
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

    /*
     * Структура многочлен
     * Является суммой мономов, хранящихся в массиве terms
     * В отличии от переменных и мономов, в программе может быть несколько копий одинаковых массивов
     */
    struct Polynomial {
        vector<Term *> terms;

        Polynomial() {}

        /*
         * Контруктор. Многочлены нужно создавать через него.
         * По умолчанию need_clean=true, что означает, что нужно привести подобные и отсортировать
         * мономы в том порядке, который задается оператором < для мономов.
         */
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

        /*
         * Конструктор, позволяющий создать копию многочлена
         */
        Polynomial(Polynomial const &p) {
            terms = p.terms;
        }
    };

    /*
     * Вывод многослена в формате x*y+y*z+1. Без перевода строки
     */
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

    /*
     * Многочлены zero_pol и one_pol.
     * zero_term - у него terms = {}
     * one_term - у него terms = {one_term}
     */
    Polynomial zero_pol = Polynomial(vector<Term*> ());
    Polynomial one_pol = Polynomial({one_term});

    /*
     * Сумма многочленов.
     * С приведением подобных слагаемых
     */
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

    /*
     * Произведение многочленов.
     * С приведением подобных слагаемых
     */
    Polynomial mult_pol(Polynomial a, Polynomial b) {
        vector<Term *> terms;
        for (Term *x : a.terms) {
            for (Term *y : b.terms) {
                terms.push_back(mult_terms(x, y));
            }
        }
        return Polynomial(terms);
    }

    /*
     * Проверка того, что мночлен является нулевым.
     * В отличии от переменных и мономов, может быть много копий нулевого многочлена
     */
    bool is_zero_pol(const Polynomial &a) {
        return a.terms.size() == 0;
    }

    /*
     * Проверка того, что мночлен является 1.
     * В отличии от переменных и мономов, может быть много копий многочлена 1
     */
    bool is_one_pol(const Polynomial &a) {
        return a.terms.size() == 1 && a.terms[0] == one_term;
    }

    /*
     * Проверка того, что многочлен линейный. То есть нет мономов x*y
     */
    bool is_linear_pol(const Polynomial &a) {
        for (Term* t : a.terms) {
            if (t->var_ids.size() > 1) {
                return false;
            }
        }
        return true;
    }

    Polynomial eval_pol(const Polynomial &a, bool recursive = false);

    /*
     * Вычисление значения монома при заданном temp_values означивании переменных.
     * Значение это многочлен, так как в означивании переменной можно сопоставить многочлен.
     *
     * Кроме того, есть параметр recursive, по умолчанию равный false
     * От него зависит то, как функция обращается с переменными, которые заменяют собой некоторый многочлен.
     * Если recursive=true, то вместо переменных подставляются многочлены, хранящиеся в поле value
     *
     * Пример:
     * Пусть есть переменные
     * 0, value=None, int_value=0
     * 1, value=None, int_value=1
     * x, value=None, int_value=-1
     * y, value=None, int_value=-1
     * z, value=None, int_value=-1
     * a, value=x+y, int_value=-1
     * b, value=y+x*z, int_value=-1
     *
     * Пусть есть моном a*b*x
     *
     * Пусть никакого дополнительного означивания нет, то есть temp_values = {}
     * eval_term(a*b*x, false) = a*b*x
     * eval_term(a*b*x, true) = x*z+x*y*z, так как (x+y)*(y+x*z)*x = x*y*x + x*z*x + y*y*x + x*y*z*x = x*z+x*y*z
     *
     * Пусть теперь есть означивание и temp_values = {x: y+z}
     * eval_term(a*b*x, false) = a*b*y+a*b*x
     * eval_term(a*b*x, true) = то, что получится если раскрыть скобки и привести подобные в ((y+z)+y)*(y+(y+z)*z)*(y+z)
     * то есть если переменная есть в temp_values, то в ответе ее не будет
     */
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

    /*
     * Аналогично eval_term, но теперь для многочленов.
     * В основном нужно вызывать эту функцию, а уже она будет вызывать eval_term от мономов, входящих в многочлен
     */
    Polynomial eval_pol(const Polynomial &a, bool recursive) {
        vector<Term *> new_terms = {};
        for (Term *t : a.terms) {
            for (Term *nt : eval_term(t, recursive).terms) {
                new_terms.push_back(nt);
            }
        }
        return Polynomial(new_terms);
    }

    /*
     * Аналог https://en.cppreference.com/w/cpp/algorithm/rotate для массива из многочленов
     */
    void circular_shift(vector<Polynomial> &s, int j) {
        if ((int)s.size() == 0) {
            return;
        }
        j %= (int)s.size();
        if (j < 0) {
            j += (int) s.size();
        }
        rotate(s.begin(), s.begin() + j, s.end());
    }

    /*
     * К многочлену в позиции pos массива многочленов прибавляется 1
     */
    void flip_pos(vector<Polynomial> &s, int pos) {
        assert((int)s.size() > 0);
        pos %= (int)s.size();
        if (pos < 0) {
            pos += (int) s.size();
        }
        s[pos] = add_pol(s[pos], one_pol);
    }

    /*
     * Аналог XOR двоичной строчек, только вместо символов 0 и 1 в строках тут многочлены
     */
    vector<Polynomial> bin_xor(const vector<Polynomial> &a, const vector<Polynomial> &b) {
        assert((int) a.size() == (int) b.size());
        int sz = (int) a.size();
        vector<Polynomial> c = {};
        for (int i = 0; i < sz; i++) {
            c.emplace_back(add_pol(a[i], b[i]));
        }
        return c;
    }

    /*
     * Аналог AND двоичной строчек, только вместо символов 0 и 1 в строках тут многочлены
     */
    vector<Polynomial> bin_and(const vector<Polynomial> &a, const vector<Polynomial> &b) {
        assert((int) a.size() == (int) b.size());
        int sz = (int) a.size();
        vector<Polynomial> c = {};
        for (int i = 0; i < sz; i++) {
            c.emplace_back(mult_pol(a[i], b[i]));
        }
        return c;
    }

    /*
     * Аналог NOT двоичной строки, только вместо символов 0 и 1 в строках тут многочлены
     */
    vector<Polynomial> bin_not(const vector<Polynomial> &a) {
        int sz = (int) a.size();
        vector<Polynomial> c = {};
        for (int i = 0; i < sz; i++) {
            c.emplace_back(add_pol(a[i], one_pol));
        }
        return c;
    }

    /*
     * Натсоящее деление мономов, возвращается моном.
     * Если мономы не делятся, то возвращается нулевой моном.
     */
    Term* divide_terms(Term* a, Term* b) {
        if (a->degree() < b->degree()) {
            return zero_term;
        }
        if (a == zero_term || b == zero_term) {
            return zero_term;
        }
        vector<int> var_ids;
        int cnt_match = 0;
        for (int x : a->var_ids) {
            bool match = false;
            for (int y : b->var_ids) {
                if (x == y) {
                    match = true;
                    break;
                }
            }
            if (match) {
                cnt_match++;
            } else {
                var_ids.push_back(x);
            }
        }
        if (cnt_match < (int)b->var_ids.size()) {
            return zero_term;
        } else {
            return add_term(var_ids);
        }
    }
}
