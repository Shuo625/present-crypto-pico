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

def print_ANF(i, x, y, stages):
    print(f'* y{i} ANF')
    print(f'* x3 x2 x1 x0 y{i} S1 S2 S3 S4')

    for i in range(16):
        print(f'* {x[i][3]}  {x[i][2]}  {x[i][1]}  {x[i][0]}  {y[i]}  {stages[0][i]}  {stages[1][i]}  {stages[2][i]}  {stages[3][i]}')

    

def generate_ANFs(sbox):
    NUM = 4

    ys = [[val[i] for val in sbox] for i in range(NUM)]
    x = generate_x_permutation(NUM)

    stages_y0 = cal_stages(ys[0])
    stages_y1 = cal_stages(ys[1])
    stages_y2 = cal_stages(ys[2])
    stages_y3 = cal_stages(ys[3])
