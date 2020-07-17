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


def circular_shift(s, j):
    return s[j:] + s[:j]


def log_xor_pos(s, pos, b):
    if b == '0':
        return s
    if pos < 0:
        pos += len(s)
    res = ''
    for i in range(len(s)):
        if i == pos:
            if s[i] == '0':
                res += '1'
            else:
                res += '0'
        else:
            res += s[i]
    return res


def log_xor(a, b):
    assert len(a) == len(b)
    c = ''
    for i in range(len(a)):
        if a[i] == b[i]:
            c += '0'
        else:
            c += '1'
    return c


def log_and(a, b):
    assert len(a) == len(b)
    c = ''
    for i in range(len(a)):
        if a[i] == '1' and b[i] == '1':
            c += '1'
        else:
            c += '0'
    return c


def log_not(a):
    c = ''
    for i in range(len(a)):
        if a[i] == '1':
            c += '0'
        else:
            c += '1'
    return c


# n = word size (16, 24, 32, 48, or 64)
# m = number of key words (must be 4 if n = 16, 3 or 4 if n = 24 or 32, 2 or 3 if n = 48, 2, 3, or 4 if n = 64)
# x in binary
# y in binary
# key list in binary
def encrypt(n, m, x, y, key):

    z = ['11111010001001010110000111001101111101000100101011000011100110',
         '10001110111110010011000010110101000111011111001001100001011010',
         '10101111011100000011010010011000101000010001111110010110110011',
         '11011011101011000110010111100000010010001010011100110100001111',
         '11010001111001101011011000100000010111000011001010010011101111']

    # (T, j) = (32,0)                     if n = 16
    #        = (36,0) or (36,1)           if n = 24, m = 3 or 4
    #        = (42,2) or (44,3)           if n = 32, m = 3 or 4
    #        = (52,2) or (54,3)           if n = 48, m = 2 or 3
    #        = (68,2), (69,3), or (72,4)  if n = 64, m = 2, 3, or 4

    if n == 16:
        T, j = 32, 0
    elif n == 24:
        if m == 3:
            T, j = 36, 0
        elif m == 4:
            T, j = 36, 1
    elif n == 32:
        if m == 3:
            T, j = 42, 2
        elif m == 4:
            T, j = 44, 3
    elif n == 48:
        if m == 2:
            T, j = 52, 2
        elif m == 3:
            T, j = 54, 3
    elif n == 64:
        if m == 2:
            T, j = 68, 2
        elif m == 3:
            T, j = 69, 3
        elif m == 4:
            T, j = 72, 4


    k = [None] * T
    for i in range(m):
        k[i] = key[i]


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


    for i in range(T):
        tmp = x
        vals = [y, log_and(circular_shift(x, 1), circular_shift(x, 8)), circular_shift(x, 2), k[i]]
        x = vals[0]
        for p in range(1, len(vals)):
            x = log_xor(x, vals[p])
        y = tmp

    return x, y


tests = [
    [16, 4, ['1918', '1110', '0908', '0100'][::-1], '6565', '6877', 'c69b', 'e9bb'],
    [24, 3, ['121110', '0a0908', '020100'][::-1], '612067', '6e696c', 'dae5ac', '292cac'],
    [24, 4, ['1a1918', '121110', '0a0908', '020100'][::-1], '726963', '20646e', '6e06a5', 'acf156'],
    [32, 3, ['13121110', '0b0a0908', '03020100'][::-1], '6f722067', '6e696c63', '5ca2e27f', '111a8fc8'],
    [32, 4, ['1b1a1918', '13121110', '0b0a0908', '03020100'][::-1], '656b696c', '20646e75', '44c8fc20', 'b9dfa07a'],
    [48, 2, ['0d0c0b0a0908', '050403020100'][::-1], '2072616c6c69', '702065687420', '602807a462b4', '69063d8ff082'],
    [48, 3, ['151413121110', '0d0c0b0a0908', '050403020100'][::-1], '746168742074', '73756420666f', 'ecad1c6c451e', '3f59c5db1ae9'],
    [64, 2, ['0f0e0d0c0b0a0908', '0706050403020100'][::-1], '6373656420737265', '6c6c657661727420', '49681b1e1e54fe3f', '65aa832af84e0bbc'],
    [64, 3, ['1716151413121110', '0f0e0d0c0b0a0908', '0706050403020100'][::-1], '206572656874206e', '6568772065626972', 'c4ac61effcdc0d4f', '6c9c8d6e2597b85b'],
    [64, 4, ['1f1e1d1c1b1a1918', '1716151413121110', '0f0e0d0c0b0a0908', '0706050403020100'][::-1], '74206e69206d6f6f', '6d69732061207369', '8d2b5579afc8a3a0', '3bf72a87efe7b868'],
]

for test in tests:
    n = test[0]
    m = test[1]
    key = [from_hex(i) for i in test[2]]
    x = from_hex(test[3])
    y = from_hex(test[4])

    en_x, en_y = encrypt(n, m, x, y, key)
    hex_en_x = to_hex(en_x)
    hex_en_y = to_hex(en_y)
    passed_test = (hex_en_x == test[5] and hex_en_y == test[6])

    print('OK' if passed_test else 'WA')
    print(f'Simon {n * 2}/{n * m}')
    print(f'Key:       ', *test[2])
    print(f'Plaintext: ', test[3], test[4])
    print(f'Ciphertext:', test[5], test[6])
    print(f'My result: ', hex_en_x, hex_en_y)
    print()
