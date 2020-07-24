#include <iostream>
#include <fstream>
#include <vector>
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

public: /*
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
};

int main() {
    try {
        Equation_system M(""); // SOURCE PATH
        //M.gauss_elim();
        //M.write_system(""); // DESTINATION PATH

        //M.find_solution();
        //M.write_solution("C:\\Users\\Semen\\Documents\\Study\\Ctyptoschool\\Programs\\answer.txt");
    } catch(const char* msg) {
        cout << "Error: " << msg << '\n';
    } catch(...) {
        cout << "Unknown error!\n";
    }
    return 0;
}
