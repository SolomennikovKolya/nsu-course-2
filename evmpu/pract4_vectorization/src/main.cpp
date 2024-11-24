
#include "blas_matrix.h"
#include "intrinsic_matrix.h"
#include "usual_matrix.h"
#include <iostream>

using namespace std;

#define debug 0
#if (debug)
const int N = 8;
const int M = 1000;
#else
const int N = 512;
const int M = 10;
#endif

template <class MartixType>
float testMatrix()
{
    MartixType A(N);
    for (int j = 0; j < N; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            A[j][i] = float(j * N + i);
        }
    }
    MartixType Res(N);
    clock_t startTime = clock();
    A.reverse(Res, M);
    clock_t finalTime = clock();
#if (debug)
    cout << "\n";
    Res.print();
#endif
    return float(finalTime - startTime) / CLOCKS_PER_SEC;
}

int main(int argc, char **argv)
{
    float time;
    time = testMatrix<UsualMatrix>();
    cout << "Usual Time: " << time << "\n";
    time = testMatrix<BlasMatrix>();
    cout << "Blas Time: " << time << "\n";
    time = testMatrix<IntrinsicMatrix>();
    cout << "Intrinsic Time: " << time << "\n";
    return 0;
}
