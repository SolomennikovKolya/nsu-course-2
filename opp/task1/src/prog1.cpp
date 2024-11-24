
#include <iostream>
#include <time.h>

#include "settings.h"

int main()
{
    // Инициализация данных
    double *a = (double *)malloc(sizeof(double) * N);
    double *b = (double *)malloc(sizeof(double) * N);
    for (int i = 0; i < N; ++i)
    {
        a[i] = 1.0;
        b[i] = 1.0;
    }
    double s = 0;

    time_t startTime = clock();

    // Вычисления
    for (int j = 0; j < N; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            s += a[i] * b[j];
        }
    }

    time_t endTime = clock();
    std::cout << "Total time: " << double(endTime - startTime) / CLOCKS_PER_SEC << "\n";

    return 0;
}
