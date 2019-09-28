// refer to http://fusharblog.com/solving-linear-recurrence-for-programming-contest/

#include <iostream>
#include <vector>

using namespace std;

typedef vector<vector<long long int> > matrix;

matrix mat_mul(const matrix &mat_in1, const matrix &mat_in2)
{
    const int LEN = static_cast<int>(mat_in1.size());
    matrix mat_out(LEN, vector<long long int>(LEN, 0));

    for (int r=0; r<LEN; ++r) {
        for (int c=0; c<LEN; ++c) {
            for (int p=0; p<LEN; ++p) {
                mat_out[r][c] += (mat_in1[r][p] * mat_in2[p][c]);
            }
        }
    }

    return mat_out;
}

matrix mat_pow(const matrix &mat_in, int n)
{
    if (n == 1) {
        return mat_in;
    }
    else if (n & 1) {
        return mat_mul(mat_in, mat_pow(mat_in, n-1));
    }
    else {
        const matrix half = mat_pow(mat_in, n/2);
        return mat_mul(half, half);
    }
}

int main(int argc, char** argv)
{
    int LEN = 2;
    int N = 6;

    matrix mat_trans(LEN, vector<long long int>(LEN, 0));
    mat_trans[0][1] = 1;
    mat_trans[1][0] = 1;
    mat_trans[1][1] = 1;

    if (N <= 2) {
        cout << 1 << endl;
        return 0;
    }

    mat_trans = mat_pow(mat_trans, N-1);
    cout << (mat_trans[0][0] + mat_trans[0][1]) << endl;;
    return 0;
}
