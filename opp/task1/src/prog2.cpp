
#include <iostream>
#include <mpi.h>

#include "settings.h"

int main(int argc, char **argv)
{
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int vecPartSize = N / size; // Количество элементов вектора b, которые будут передаваться другим процессам

    if (rank == 0)
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

        double startTime = MPI_Wtime();

        // Распределение вектора a и части вектора b другим процессам
        for (int p = 1; p < size; ++p)
        {
            MPI_Send(a, N, MPI_DOUBLE, p, 1, MPI_COMM_WORLD);
            MPI_Send(b + vecPartSize * (p - 1), vecPartSize, MPI_DOUBLE, p, 2, MPI_COMM_WORLD);
        }

        // Вычисления с участием нераспределённого куска вектора b
        for (int j = vecPartSize * (size - 1); j < N; ++j)
        {
            for (int i = 0; i < N; ++i)
            {
                s += a[i] * b[j];
            }
        }

        // Приём результатов вычислений процессов
        MPI_Status st;
        double partOfSum;
        for (int p = 1; p < size; ++p)
        {
            MPI_Probe(MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &st);
            MPI_Recv(&partOfSum, 1, MPI_DOUBLE, st.MPI_SOURCE, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // MPI_Recv(&partOfSum, 1, MPI_DOUBLE, p, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            s += partOfSum;
        }

        double endTime = MPI_Wtime();
        std::cout << "Total time: " << endTime - startTime << "\n";
        std::cout << "Check result: " << s << "\n";

        free(a);
        free(b);
    }
    else
    {
        // Инициализация данных
        double *a = (double *)malloc(sizeof(double) * N);
        double *b = (double *)malloc(sizeof(double) * vecPartSize);

        // Приём данных
        MPI_Recv(a, N, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(b, vecPartSize, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Вычисления
        double partOfSum = 0;
        for (int j = 0; j < vecPartSize; ++j)
        {
            for (int i = 0; i < N; ++i)
            {
                partOfSum += a[i] * b[j];
            }
        }

        // Отправка результата
        MPI_Send(&partOfSum, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);

        free(a);
        free(b);
    }

    MPI_Finalize();
    return 0;
}