# refer to http://fusharblog.com/solving-linear-recurrence-for-programming-contest/

def do_mat_mul(mat_in1, mat_in2):
    mat_out = [[0, 0], [0, 0]]

    for r in range(2):
        for c in range(2):
            for k in range(2):
                mat_out[r][c] += (mat_in1[r][k] * mat_in2[k][c])

    return mat_out

def do_mat_pow(mat_in, p):
    if (1 == p):
        return mat_in
    elif (1 & p):
        return do_mat_mul(mat_in, do_mat_pow(mat_in, p-1))
    else:
        half = do_mat_pow(mat_in, p//2)
        return do_mat_mul(half, half)

def fib(N):
    if (N <= 0):
        return 0
    elif (N <= 2):
        return 1

    mat_trans = [[0, 1], [1, 1]]
    mat_trans = do_mat_pow(mat_trans, N-1)
    return mat_trans[0][0] + mat_trans[0][1]

print (fib(300))
