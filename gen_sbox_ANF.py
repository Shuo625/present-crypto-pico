import sys
import copy
import math


def generate_x_permutation(num):
    permutation = []
    permutation.append([0 for i in range(num)])

    def add_one(l):
        l_add_one = copy.deepcopy(l)
        
        for idx in range(len(l_add_one)):
            if l_add_one[idx] == 0:
                l_add_one[idx] = 1
                break
            else:
                l_add_one[idx] = 0

        return l_add_one

    for i in range(1, 2 ** num):
        permutation.append(add_one(permutation[i - 1]))

    return permutation


def cal_stages(y):
    stage_nums = int(math.log2(len(y)))
    stages = []
    pre_stage = y
    
    for stage_num in range(stage_nums):
        step = 2 ** stage_num
        stage = []

        for idx, val in enumerate(pre_stage):
            if idx % (step * 2) < step:
                stage.append(val)
            else:
                stage.append((val + pre_stage[idx - step]) % 2)
        
        pre_stage = stage
        stages.append(stage)

    return stages

def _generate_ANF_formula(x, final_stage):
    formula = ''

    for idx, val in enumerate(final_stage):
        if val == 1:
            tmp = ''

            for _idx, _val in enumerate(x[idx]):
                if _val == 1:
                    if len(tmp) == 0:
                        tmp += f'x{_idx}'
                    else:
                        tmp += f' * x{_idx}'
            
            if len(tmp) == 0:
                tmp = '1'
            
            if len(formula) == 0:
                formula += tmp
            else:
                formula += f' + {tmp}'

    return formula

def print_ANF(num, x, y, stages):
    print(f' * y{num} ANF')
    print(f' * x3 x2 x1 x0 y{num} S1 S2 S3 S4')

    for i in range(16):
        print(f' * {x[i][3]}  {x[i][2]}  {x[i][1]}  {x[i][0]}  {y[i]}  {stages[0][i]}  {stages[1][i]}  {stages[2][i]}  {stages[3][i]}')

    formula = _generate_ANF_formula(x, stages[3])
    print(f' *\n * y{num} = {formula}\n *')

def generate_ANFs(sbox):
    NUM = 4
    print(sbox)
    ys = [[int(val[3 - i]) for val in sbox] for i in range(NUM)]
    x = generate_x_permutation(NUM)

    for i in range(NUM):
        print_ANF(i, x, ys[i], cal_stages(ys[i]))

def _hex_to_binary_str(val):
    bin_str = str(bin(int(val, base=16)))[2:]

    return (4 - len(bin_str)) * '0' + bin_str


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: python script sbox(of form 0x1, ..., 0xC)')

    sbox = sys.argv[1].split(', ')

    sbox = [_hex_to_binary_str(val) for val in sbox]

    generate_ANFs(sbox)
