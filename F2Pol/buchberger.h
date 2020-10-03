#pragma once

#include "f2pol.h"
#include "equations.h"

using namespace F2Pol;
using namespace Equations;

Polynomial reduce_pol(Polynomial pol, vector<Polynomial> &G) {
    while (true) {
        bool found = false;
        for (int i = (int)pol.terms.size() - 1; i >= 0; i--) {
            for (Polynomial a : G) {
                Term* g = divide_terms(pol.terms[i], a.terms.back());
                if (g != zero_term) {
                    pol = add_pol(pol, mult_pol(a, Polynomial({g})));
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        if (!found) {
            break;
        }
    }
    return pol;
}


Polynomial S(Polynomial g1, Polynomial g2) {
    if (is_zero_pol(g1) || is_zero_pol(g2)) {
        return Polynomial(zero_pol);
    }
    Term* lcm = mult_terms(g1.terms.back(), g2.terms.back());
    if (lcm->degree() == g1.terms.back()->degree() + g2.terms.back()->degree()) {
        return zero_pol;
    }
    Term* t_1 = divide_terms(lcm, g1.terms.back());
    Term* t_2 = divide_terms(lcm, g2.terms.back());
    return add_pol(mult_pol(Polynomial({t_1}), g1), mult_pol(Polynomial({t_2}), g2));
}


vector<Polynomial> Buchberger(vector<Polynomial> equations) {
    vector<Polynomial> G;
    vector<pair<Polynomial, F2Pol::Polynomial> > B;
    auto update = [&](Polynomial &f) {
        if (is_zero_pol(f)) {
            return;
        }
        for (Polynomial &g : G) {
            B.emplace_back(make_pair(f, g));
        }
        G.push_back(f);
    };
    for (Polynomial f : equations) {
        f = reduce_pol(f, G);
        update(f);
    }
    while (B.size() > 0) {
        auto p = B.back();
        B.pop_back();
        Polynomial g = reduce_pol(S(p.first, p.second), G);
        //cout << p.first << " " << p.second << " " << S(p.first, p.second) << " " << g << endl;
        update(g);
    }
    return G;
}