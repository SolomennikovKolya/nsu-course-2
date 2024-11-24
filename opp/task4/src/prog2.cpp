
#include <iostream>
#include <mpi.h>
#include <string>

// P1 * P2 - размеры двумерной решётки
// N1 * N2 - размер матрицы A
// N2 * N3 - размер матрицы B
// N1 * N3 - размер матрицы C

int P1 = 2; // Количество горизонтальных полос, на которые делатся матрица A
int P2 = 2; // Количество вертикальных полос, на которые делатся матрица B
int N1 = 4;
int N2 = 4;
int N3 = 4;
int DY = N1 / P1; // Высота горизонтальных полос
int DX = N3 / P2; // Ширина вертикальных полос

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
    if (argc >= 6)
    {
        N1 = std::stoi(argv[1]);
        N2 = std::stoi(argv[2]);
        N3 = std::stoi(argv[3]);
        P1 = std::stoi(argv[4]);
        P2 = std::stoi(argv[5]);
    }
    if (N1 % P1 != 0 || N3 % P2 != 0)
        throw myException("The dimensions of the matrix must be a multiple of the dimensions of the two-dimensional grid");
    DY = N1 / P1;
    DX = N3 / P2;
}

// Освобождение памяти
inline void deleteData(double *&A, double *&B, double *&C, double *&LocA, double *&LocB, double *&LocC)
{
    free(A);
    free(B);
    free(C);
    free(LocA);
    free(LocB);
    free(LocC);
}

// Инициализация матриц для корневого процесса (именно здесь задаются значения в матрицах)
inline void initRootData(double *&A, double *&B, double *&C, double *&LocA, double *&LocB, double *&LocC)
{
    A = (double *)malloc(sizeof(double) * N1 * N2);
    B = (double *)malloc(sizeof(double) * N2 * N3);
    C = (double *)malloc(sizeof(double) * N1 * N3);
    LocA = (double *)malloc(sizeof(double) * DY * N2);
    LocB = (double *)malloc(sizeof(double) * N2 * DX);
    LocC = (double *)malloc(sizeof(double) * DY * DX);
    if (A == nullptr || B == nullptr || C == nullptr || LocA == nullptr || LocB == nullptr || LocC == nullptr)
    {
        deleteData(A, B, C, LocA, LocB, LocC);
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

// Инициализация матриц для не корневых процессов
inline void initSideData(double *&LocA, double *&LocB, double *&LocC)
{
    LocA = (double *)malloc(sizeof(double) * DY * N2);
    LocB = (double *)malloc(sizeof(double) * N2 * DX);
    LocC = (double *)malloc(sizeof(double) * DY * DX);
    if (LocA == nullptr || LocB == nullptr || LocC == nullptr)
    {
        free(LocA);
        free(LocB);
        free(LocC);
        throw myException("Malloc fail");
    }
}

// Вычисления (LocC = LocA * LocB)
inline void calculate(double *const LocA, double *const LocB, double *const LocC)
{
    for (int i = 0; i < DY; ++i)
    {
        for (int j = 0; j < DX; ++j)
        {
            LocC[i * DX + j] = 0;
            for (int k = 0; k < N2; ++k)
            {
                LocC[i * DX + j] += LocA[i * N2 + k] * LocB[k * DX + j];
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

    double *A = nullptr, *B = nullptr, *C = nullptr;
    double *LocA = nullptr, *LocB = nullptr, *LocC = nullptr;

    try
    {
        initSizes(argc, argv);
        if (rank == 0 && P1 * P2 != size)
            throw myException("The grid size should correspond to the number of processes");

        if (rank == 0)
            initRootData(A, B, C, LocA, LocB, LocC);
        else
            initSideData(LocA, LocB, LocC);
    }
    catch (const std::exception &e)
    {
        if (rank == 0)
            std::cerr << e.what() << '\n';
        return 0;
    }

    // Создание коммуникаторов для строк и столбцов
    int rankX = rank % P2;
    int rankY = rank / P2;
    MPI_Comm commColumn; // Коммуникатор по столбцу
    MPI_Comm commRow;    // Коммуникатор по строке
    MPI_Comm_split(MPI_COMM_WORLD, rankX, rank, &commColumn);
    MPI_Comm_split(MPI_COMM_WORLD, rankY, rank, &commRow);

    // Нулевой процесс разделяет матрицу A на горизонтальные полосы и передаёт их нулевому столбцу процессов
    if (rankX == 0)
        MPI_Scatter(A, DY * N2, MPI_DOUBLE, LocA, DY * N2, MPI_DOUBLE, 0, commColumn);

    // Горизонтальные полосы матрицы A раздаются от каждого элемента нулевого столбца процессов всем процессам в соответствующей строке
    MPI_Bcast(LocA, DY * N2, MPI_DOUBLE, 0, commRow);

    // Новый тип для хранения целого столбца матрицы B
    MPI_Datatype columnType;
    // count - число блоков; blocklength - число элементов базового типа в каждом блоке
    // stride - шаг между началами соседних блоков, измеренный числом элементов базового типа; oldtype - базовый тип данных
    MPI_Type_vector(N2, DX, N3, MPI_DOUBLE, &columnType);
    MPI_Type_commit(&columnType);

    // Нулевой процесс разделяет матрицу B на вертикальнные столбцы и передаёт их нулевой строке процессов
    // Процессы из первой строки принимают данные
    if (rank == 0)
    {
        for (int p = 1; p < P2; ++p)
            MPI_Send(B + DX * p, 1, columnType, p, p, commRow);
        // Копирование нулевого столбца матрицы B в LocB для нулевого процесса
        for (int y = 0; y < N2; ++y)
        {
            for (int x = 0; x < DX; ++x)
                LocB[y * DX + x] = B[y * N3 + x];
        }
    }
    else if (rank < P2)
    {
        MPI_Recv(LocB, N2 * DX, MPI_DOUBLE, 0, rank, commRow, MPI_STATUS_IGNORE);
    }

    // Вертикальные полосы матрицы B раздаются от каждого элемента нулевой строки процессов всем процессам в соответствующем столбце
    MPI_Bcast(LocB, N2 * DX, MPI_DOUBLE, 0, commColumn);

    calculate(LocA, LocB, LocC);

    // Новый тип для того, чтобы можно было удобно собирать части матрицы C без использования промежуточного буфера
    MPI_Datatype submatrixType; // По сути это 1/P1 часть от columnType
    // count - число блоков; blocklength - число элементов базового типа в каждом блоке
    // stride - шаг между началами соседних блоков, измеренный числом элементов базового типа; oldtype - базовый тип данных
    MPI_Type_vector(DY, DX, N3, MPI_DOUBLE, &submatrixType);
    MPI_Type_commit(&submatrixType);

    // Сбор частей матрицы C в одну большую матрицу C на нулевом процессе
    if (rank != 0)
        MPI_Send(LocC, DY * DX, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
    else
    {
        // Копирование LocC в левый верхний угол C для нулевого процесса
        for (int y = 0; y < DY; ++y)
        {
            for (int x = 0; x < DX; ++x)
                C[y * N3 + x] = LocC[y * DX + x];
        }
        for (int p = 1; p < size; ++p)
        {
            const int x = p % P2;
            const int y = p / P2;
            MPI_Recv(C + N3 * DY * y + DX * x, 1, submatrixType, p, p, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    // // Вывод матрицы C
    // if (rank == 0)
    // {
    //     printf("Matrix C:\n");
    //     for (int y = 0; y < N1; ++y)
    //     {
    //         for (int x = 0; x < N3; ++x)
    //         {
    //             std::cout << C[y * N3 + x] << " ";
    //         }
    //         std::cout << "\n";
    //     }
    //     std::cout << "\n";
    // }

    // // Вывод матриц LocC
    // for (int p = 0; p < size; ++p)
    // {
    //     if (p == rank)
    //     {
    //         printf("rank: %d, rankX: %d, rankY: %d\n", rank, rankX, rankY);
    //         for (int y = 0; y < DY; ++y)
    //         {
    //             for (int x = 0; x < DX; ++x)
    //             {
    //                 std::cout << LocC[y * DX + x] << " ";
    //             }
    //             std::cout << "\n";
    //         }
    //         std::cout << "\n";
    //     }
    //     MPI_Barrier(MPI_COMM_WORLD);
    // }

    // Вывод времени работы программы
    // MPI_Barrier(MPI_COMM_WORLD);
    const double endTime = MPI_Wtime();
    if (rank == 0)
        std::cout << "Total time: " << endTime - startTime << "\n";

    deleteData(A, B, C, LocA, LocB, LocC);
    MPI_Type_free(&columnType);
    MPI_Type_free(&submatrixType);
    MPI_Finalize();
    return 0;
}
