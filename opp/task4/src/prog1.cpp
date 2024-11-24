
#include <iostream>
#include <mpi.h>
#include <string>

// N1 * N2 - размер матрицы A
// N2 * N3 - размер матрицы B
// N1 * N3 - размер матрицы C

int N1 = 4;
int N2 = 4;
int N3 = 4;

// Класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;

public:
    explicit myException(const char *msg) : msg_(msg){};
    const char *what() const noexcept override { return msg_; }
};

// Инициализация размеров матриц (по умолчанию есть заданные размеры, но их можно явно указать через аргументы командной строки)
inline void initSizes(const int argc, char **argv)
{
    // Параметры программы: N1 N2 N3 P1 P2
    if (argc >= 4)
    {
        N1 = std::stoi(argv[1]);
        N2 = std::stoi(argv[2]);
        N3 = std::stoi(argv[3]);
    }
}

// Освобождение памяти
inline void deleteData(double *&A, double *&B, double *&C)
{
    free(A);
    free(B);
    free(C);
}

// Инициализация матриц для корневого процесса (именно здесь задаются значения в матрицах)
inline void initData(double *&A, double *&B, double *&C)
{
    A = (double *)malloc(sizeof(double) * N1 * N2);
    B = (double *)malloc(sizeof(double) * N2 * N3);
    C = (double *)malloc(sizeof(double) * N1 * N3);
    if (A == nullptr || B == nullptr || C == nullptr)
    {
        deleteData(A, B, C);
        throw myException("Malloc fail");
    }

    for (int y = 0; y < N1; ++y)
    {
        for (int x = 0; x < N2; ++x)
        {
            A[y * N2 + x] = y * N2 + x;
        }
    }

    for (int y = 0; y < N2; ++y)
    {
        for (int x = 0; x < N3; ++x)
        {
            B[y * N3 + x] = y * N3 + x;
        }
    }
}

// Вычисления
inline void calculate(double *const A, double *const B, double *const C)
{
    for (int i = 0; i < N1; ++i)
    {
        for (int j = 0; j < N3; ++j)
        {
            C[i * N3 + j] = 0;
            for (int k = 0; k < N2; ++k)
            {
                C[i * N3 + j] += A[i * N2 + k] * B[k * N3 + j];
            }
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    const double startTime = MPI_Wtime();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 1)
    {
        if (rank == 0)
            std::cerr << "Sequential program must be run by a single process\n";
        return 0;
    }

    double *A, *B, *C;
    double *LocA, *LocB, *LocC;

    try
    {
        initSizes(argc, argv);
        initData(A, B, C);
        calculate(A, B, C);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    // printf("Matrix C:\n");
    // for (int y = 0; y < N1; ++y)
    // {
    //     for (int x = 0; x < N3; ++x)
    //     {
    //         std::cout << C[y * N3 + x] << " ";
    //     }
    //     std::cout << "\n";
    // }
    // std::cout << "\n";

    const double endTime = MPI_Wtime();
    std::cout << "Total time: " << endTime - startTime << "\n";

    deleteData(A, B, C);
    return 0;
}
