all_vars = []
all_terms = []
term_to_id = {}

temp_values = {}
eval_id = -1

# value = [0, 1, None, Polynomial]
class Variable:
    def __init__(self, _id, _name, _value=None):
        self.id = _id
        self.name = _name
        self.value = _value

    def __str__(self):
        return self.name

    def __repr__(self):
        return self.__str__()


def add_variable(_name, _value=None):
    _id = len(all_vars)
    new_var = Variable(_id, _name, _value)
    all_vars.append(new_var)
    return _id


zero = add_variable("0", 0)
one = add_variable("1", 1)

# non-empty product of variables. 0 = ["0"], 1 = ["1"]
class Term:
    def __init__(self, _id, _var_ids=None):
        self.var_ids = _var_ids
        self.id = _id

        self.last_eval_id = -2
        self.last_eval_value = None

    def __str__(self):
        l = self.var_ids
        s = ''
        for x in l:
            if s:
                s += '*'
            s += str(all_vars[x])
        return s

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.id == other.id

    def __lt__(self, other):
        return self.id < other.id


def add_term(_var_ids=None):
    var_list = []
    if _var_ids is not None:
        _var_ids = _var_ids.copy()
        _var_ids.sort()
        for _id in _var_ids:
            if all_vars[_id].value == 0:
                var_list = None
                break
            elif all_vars[_id].value == 1:
                pass
            else:
                if not var_list or _id != var_list[-1]:
                    var_list.append(_id)
    else:
        var_list = None

    if var_list is None:
        var_tuple = None
        var_list = [zero]
    elif not var_list:
        var_tuple = ()
        var_list = [one]
    else:
        var_tuple = tuple(var_list)

    if var_tuple not in term_to_id:
        _id = len(all_terms)
        new_term = Term(_id, var_list)
        all_terms.append(new_term)
        term_to_id[var_tuple] = _id

    return all_terms[term_to_id[var_tuple]]


zero_term = add_term(None)
one_term = add_term([])


def mult_terms(a, b):
    return add_term(a.var_ids + b.var_ids)


class Polynomial:
    def __init__(self, _terms=[], need_clean=False):
        if need_clean:
            _terms = _terms.copy()
            _terms.sort()
            terms = []
            for t in _terms:
                if t == zero_term:
                    continue
                elif not terms or t != terms[-1]:
                    terms.append(t)
                else:
                    terms.pop()
        else:
            terms = _terms.copy()
        self.terms = terms

    def __str__(self):
        s = ''
        for t in self.terms:
            if s:
                s += '+'
            s += str(t)
        if not s:
            return '0'
        return s

    def __repr__(self):
        return self.__str__()


zero_pol = Polynomial([zero_term], True)
one_pol = Polynomial([one_term], True)


def add_pol(a, b):
    return Polynomial(a.terms + b.terms, True)


def mult_pol(a, b):
    l3 = []
    for t1 in a.terms:
        for t2 in b.terms:
            l3.append(mult_terms(t1, t2))
    return Polynomial(l3, True)


# result is polynomial
def eval_term(t, recursive=False):
    #print('!!!', t)
    if t.last_eval_id == eval_id:
        return t.last_eval_value
    result = one_pol
    for _id in t.var_ids:
        if all_vars[_id].value == 1:
            pass
        elif all_vars[_id].value == 0:
            result = zero_pol
            break
        elif _id in temp_values:
            if temp_values[_id] == 1:
                pass
            elif temp_values[_id] == 0:
                result = zero_pol
                break
        elif all_vars[_id].value is None or not recursive:
            result = mult_pol(result, Polynomial([add_term([_id])]))
        else:
            result = mult_pol(result, eval_pol(all_vars[_id].value, recursive))

    t.last_eval_id = eval_id
    t.last_eval_value = result
    return result


def eval_pol(a, recursive=False):
    new_terms = []
    for t in a.terms:
        res = eval_term(t, recursive)
        for item in res.terms:
            new_terms.append(item)
    return Polynomial(new_terms, True)


def circular_shift(s, j):
    return s[j:] + s[:j]


def log_xor_pos(s, pos, b):
    #print(s, pos, b)
    s = s.copy()
    if b == '0':
        return s
    if pos < 0:
        pos += len(s)
    s[pos] = add_pol(s[pos], Polynomial([one_term]))
    return s


def log_xor(a, b):
    assert len(a) == len(b)
    c = []
    for i in range(len(a)):
        c.append(add_pol(a[i], b[i]))
    return c


def log_and(a, b):
    assert len(a) == len(b)
    c = []
    for i in range(len(a)):
        c.append(mult_pol(a[i], b[i]))
    return c


def log_not(a):
    c = []
    for i in range(len(a)):
        c.append(add_pol(a[i], Polynomial([one_term])))
    return c


def from_hex(s):
    res = ''
    for c in s:
        _id = ord(c) - ord('0') if (c >= '0' and c <= '9') else 10 + ord(c) - ord('a')
        for i in range(3, -1, -1):
            res += chr(ord('0') + ((_id >> i) & 1))
    return res


def to_hex(s):
    assert len(s) % 4 == 0
    res = ''
    for i in range(0, len(s), 4):
        val = 0
        for b in range(4):
            val += (1 << (3 - b)) * (1 if s[i + b] == '1' else 0)
        if val < 10:
            res += chr(ord('0') + val)
        else:
            res += chr(ord('a') + (val - 10))
    return res


# list of polynomials which are equal to 0
equations = []


# add equation a=b (a, b - polynomials)
def add_equation(a, b):
    equations.append(add_pol(a, b))


z = ['11111010001001010110000111001101111101000100101011000011100110',
     '10001110111110010011000010110101000111011111001001100001011010',
     '10101111011100000011010010011000101000010001111110010110110011',
     '11011011101011000110010111100000010010001010011100110100001111',
     '11010001111001101011011000100000010111000011001010010011101111']


n = 16
m = 2
T = 5
j = 0


#real_key = list(map(from_hex, ['1918', '1110', '0908', '0100'][::-1]))
#real_key = ['0000000100000000']
#real_key = ['0000000000000000', '1111111111111111']
# real_key = ['1011001011111110', '0111110010010111']
real_key = ['1011001011111110', '0111110010010111', '1010011100110100', '1000101001111111']
#1010011100100000

I_L, I_R = map(from_hex, ['6565', '6877'])
# O_L, O_R = map(from_hex, ['f2b5', 'fcd6'])
# O_L, O_R = map(from_hex, ['585e', '9616'])


k_var = [[] for i in range(max(T, m))]
k = [[] for i in range(max(T, m))]
for i in range(m):
    for j1 in range(n):
        value = 1 if real_key[i][j1] == '1' else 0
        k_var[i].append(add_variable(f'k{i}_{j1}', None))
        k[i].append(Polynomial([add_term([k_var[i][j1]])]))


for i in range(m, T):
    tmp = circular_shift(k[i - 1], -3)
    if m == 4:
        tmp = log_xor(tmp, k[i - 3])
    tmp = log_xor(tmp, circular_shift(tmp, -1))

    k[i] = log_not(k[i - m])
    k[i] = log_xor(k[i], tmp)
    k[i] = log_xor_pos(k[i], -1, z[j][(i - m) % 62])
    k[i] = log_xor_pos(k[i], -1, '1')
    k[i] = log_xor_pos(k[i], -2, '1')

    for j1 in range(n):
        value = k[i][j1]
        k_var[i].append(add_variable(f'k{i}_{j1}', value))
        # print(f'{all_vars[k_var[i][j1]]}={value}')
        k[i][j1] = Polynomial([add_term([k_var[i][j1]])])

        add_equation(k[i][j1], value)
    # print()


# After i rounds x[i] = right part, x[i+1] = left part
x_var = [[] for i in range(T + 2)]
x = [[] for i in range(T + 2)]
for j1 in range(n):
    value = 1 if I_R[j1] == '1' else 0
    # x_var[0].append(add_variable(f'x_{0}[{j1}]', value))
    x_var[0].append(add_variable(f'x{0}_{j1}', value))
    # print(f'{all_vars[x_var[0][j1]]}={value}')
    x[0].append(Polynomial([add_term([x_var[0][j1]])]))
#print()
for j1 in range(n):
    value = 1 if I_L[j1] == '1' else 0
    x_var[1].append(add_variable(f'x{1}_{j1}', value))
    # print(f'{all_vars[x_var[1][j1]]}={value}')
    x[1].append(Polynomial([add_term([x_var[1][j1]])]))
#print()


for i in range(T):
    vals = [x[i], log_and(circular_shift(x[i + 1], 1), circular_shift(x[i + 1], 8)), circular_shift(x[i + 1], 2), k[i]]
    x[i + 2] = vals[0]
    for p in range(1, len(vals)):
        x[i + 2] = log_xor(x[i + 2], vals[p])

    for j1 in range(n):
        value = x[i + 2][j1]
        x_var[i + 2].append(add_variable(f'x{i + 2}_{j1}', value))
        # print(f'{all_vars[x_var[i + 2][j1]]}={value}')
        x[i + 2][j1] = Polynomial([add_term([x_var[i + 2][j1]])])

        add_equation(x[i + 2][j1], value)

    # print()


temp_values.clear()
eval_id += 1

for i in range(m):
    for j1 in range(n):
        value = 1 if real_key[i][j1] == '1' else 0
        temp_values[k_var[i][j1]] = value


resL = ''
resR = ''

for i in range(len(x[T + 1])):
    resL += str(eval_pol(x[T + 1][i], recursive=True))
for i in range(len(x[T])):
    resR += str(eval_pol(x[T][i], recursive=True))

print(resL, resR)
print(to_hex(resL), to_hex(resR))


temp_values.clear()
eval_id += 1

for j1 in range(n):
    # value = 1 if O_R[j1] == '1' else 0
    value = 1 if resR[j1] == '1' else 0
    temp_values[x_var[T][j1]] = value

    # value = 1 if O_L[j1] == '1' else 0
    value = 1 if resL[j1] == '1' else 0
    temp_values[x_var[T + 1][j1]] = value


print()
print('Equations')
eq_id = 0
for item in equations:
    print('eq_' + str(eq_id) + '=' + str(eval_pol(item, recursive=False)) + '==0')
    eq_id += 1
