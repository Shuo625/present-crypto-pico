def print_enslice():
    for i in range(32):
        print(f'tmp = (pt[{i} * CRYPTO_IN_SIZE /* which text */ + i / 8 /* which byte */] >> (i % 8 /* which bit */)) & 0x1;')
        print(f'state_bs[i] |= tmp << {i};')

def print_unslice():
    for i in range(32):
        print(f'tmp = (state_bs[i] >> {i}) & 0x1;')
        print(f'pt[{i} * CRYPTO_IN_SIZE + i / 8] |= tmp << (i % 8);')


if __name__ == '__main__':
    print_unslice()
