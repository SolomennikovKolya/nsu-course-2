
#include <iostream>
#include <mpi.h>

#include "settings.h"

int main(int argc, char **argv)
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double startTime, endTime;
    int vecPartSize = N / size;

    // Выделение памяти
    double *a = (double *)malloc(sizeof(double) * N);
    double *b = rank == 0 ? (double *)malloc(sizeof(double) * N) : nullptr; // Если процесс не корневой, то этот массив не нужен
    double *rec = (double *)malloc(sizeof(double) * vecPartSize);           // Для получения частей вектора b

    // Инициализация для нулевого процесса
    if (rank == 0)
    {
        for (int i = 0; i < N; ++i)
        {
            a[i] = 1.0;
            b[i] = 1.0;
        }

        startTime = MPI_Wtime();
    }

    // Передача вектора a всем процессам
    MPI_Bcast(a, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Распределение вектора b по всем процессам
    MPI_Scatter(b, vecPartSize, MPI_DOUBLE, rec, vecPartSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Вычисления
    double partOfSum = 0;
    for (int j = 0; j < vecPartSize; ++j)
    {
        for (int i = 0; i < N; ++i)
        {
            partOfSum += a[i] * rec[j];
        }
    }
    if (rank == 0)
    {
        for (int j = vecPartSize * size; j < N; ++j)
        {
            for (int i = 0; i < N; ++i)
            {
                partOfSum += a[i] * b[j];
            }
        }
    }

    // Суммирование результатов
    double s = 0;
    MPI_Reduce(&partOfSum, &s, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        endTime = MPI_Wtime();
        std::cout << "Total time: " << endTime - startTime << "\n";
        std::cout << "Check result: " << s << "\n";
    }

    // for (int p = 0; p < size; ++p)
    // {
    //     MPI_Barrier(MPI_COMM_WORLD);
    //     if (p == rank)
    //     {
    //         std::cout << "I am " << rank << "\n";
    //         for (int i = 0; i < N; ++i)
    //         {
    //             std::cout << rec[i] << " ";
    //         }
    //         std::cout << "\n";
    //     }
    // }

    free(a);
    free(b);
    free(rec);

    MPI_Finalize();
    return 0;
}
