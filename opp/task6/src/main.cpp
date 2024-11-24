
#include <iostream>
#include <math.h>
#include <mpi.h>
#include <pthread.h>

// константы
const int maxIters = 8;             // общее количество итераций всего алгоритма (количество списков задач, которые надо выполнить)
const int criticalTasksNum = 10;    // если у процесса осталось столько задач или меньше, он перестаёт отдавать свои задачи и начинает запрашивать новые
const int tasksPerProcess = 100000; // максимальное количество задач на 1 процесс (изначальное количество задач у процесса после начала новой итерации)
const int L = 1000;                 // параметр для вычисления сложности задачи
const int rootRank = 0;             // номер корневого процесса

// глобальные переменные (могут изменяться из обоих потоков)
int *taskList;                 // список задач
int tasksLeft;                 // количество невыполненных задач
int currentlyRunningTask;      // номер задачи, которая сейчас выполняется потоком-работником
int taskToSend;                // номер задачи, которая отправится, если какой-то процесс запросить задачу
pthread_mutex_t taskListMutex; // для того, чтобы не было конфликтов при изменении taskList и переменных, связанных с taskList
int iterCounter = 0;           // текущий номер итерации
double globalRes;              // сумма результатов вычислений. Изменяется только процессом-работником
int tasksDone;                 // количество выполненных заданий (на данной итерации)

// класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;     // сообщение
    const int errorCode_; // код ошибки

public:
    explicit myException(const char *msg) : msg_(msg), errorCode_(0){};
    myException(const char *msg, int errorCode) : msg_(msg), errorCode_(errorCode){};
    const char *what() const noexcept override { return msg_; }
    int getErrorCode() const noexcept { return errorCode_; }
};

// инициализация списка задач (без заполнения)
void initTaskList()
{
    pthread_mutex_lock(&taskListMutex);
    taskList = (int *)malloc(sizeof(int) * tasksPerProcess);
    if (taskList == nullptr)
        throw new myException("Cannot initialize taskList");
    pthread_mutex_unlock(&taskListMutex);
}

// заполение списка задач (для конкретного процесса)
void fillTaskList(const int rank, const int size)
{
    for (int i = 0; i < tasksPerProcess; ++i)
        // taskList[i] = abs(tasksPerProcess / 2 - i) + abs(rank - (iterCounter % size)) * L;
        taskList[i] = abs(rank - (iterCounter % size)) * L;
}

// заполнение списка задач и переменных перед началом новой итерации
void fillData(const int rank, const int size)
{
    pthread_mutex_lock(&taskListMutex);
    fillTaskList(rank, size);
    tasksLeft = tasksPerProcess;
    currentlyRunningTask = 0;
    taskToSend = tasksPerProcess - 1;
    pthread_mutex_unlock(&taskListMutex);
    globalRes = 0;
    tasksDone = 0;
}

// запросить и принять задачи у других процессов. Возвращает количество принятых задач
inline int getNewTasks(const int rank, const int size)
{
    // отправка запроса на задачи
    const bool needTasks = true;
    for (int p = 0; p < size; ++p)
    {
        if (p == rank)
            continue;
        MPI_Send(&needTasks, 1, MPI_CXX_BOOL, p, p, MPI_COMM_WORLD);
    }

    // приём задач
    int receivedTasksNum = 0;
    int receivedTask;
    for (int p = 0; p < size; ++p)
    {
        if (p == rank)
            continue;

        MPI_Recv(&receivedTask, 1, MPI_INT, p, rank + size, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (receivedTask != -1) // -1 означает, что процесс p не может отдать задачу
        {
            pthread_mutex_lock(&taskListMutex);
            tasksLeft++;
            taskToSend = (taskToSend + 1) % tasksPerProcess;
            taskList[taskToSend] = receivedTask;
            pthread_mutex_unlock(&taskListMutex);
            receivedTasksNum++;
        }
    }
    return receivedTasksNum;
}

// выполнить задачу
void executeTask()
{
    double sum = 0;
    for (int i = 0; i < taskList[currentlyRunningTask]; ++i)
        sum += sin(i);
    globalRes += sum;
    tasksDone++;

    pthread_mutex_lock(&taskListMutex);
    taskList[currentlyRunningTask] = 0;
    tasksLeft--;
    currentlyRunningTask = (currentlyRunningTask + 1) % tasksPerProcess;
    pthread_mutex_unlock(&taskListMutex);
}

// инициализация массивов для вывода информации
void initBuffersForPrintInfo(int *&tasksDoneArr, double *&globalResArr, double *&deltaTimeArr, const int size)
{
    tasksDoneArr = (int *)malloc(sizeof(int) * size);
    globalResArr = (double *)malloc(sizeof(double) * size);
    deltaTimeArr = (double *)malloc(sizeof(double) * size);
    if (tasksDoneArr == nullptr || globalResArr == nullptr || deltaTimeArr == nullptr)
    {
        free(tasksDoneArr);
        free(globalResArr);
        free(deltaTimeArr);
        throw new myException("Cannot malloc arrays for printInfo");
    }
}

// вычисление времени дисбаланса и доли дисбаланса и вывод информации на экран
void printInfo(const int rank, const int size, const double deltaTime, int *const tasksDoneArr, double *const globalResArr, double *const deltaTimeArr)
{
    // сбор данных со всех процессов
    MPI_Gather(&tasksDone, 1, MPI_INT, tasksDoneArr, 1, MPI_INT, rootRank, MPI_COMM_WORLD);
    MPI_Gather(&globalRes, 1, MPI_DOUBLE, globalResArr, 1, MPI_DOUBLE, rootRank, MPI_COMM_WORLD);
    MPI_Gather(&deltaTime, 1, MPI_DOUBLE, deltaTimeArr, 1, MPI_DOUBLE, rootRank, MPI_COMM_WORLD);

    if (rank == rootRank)
    {
        // вечисление времени дисбаланса
        double timeOfImbalance = 0;
        for (int i = 0; i < size; ++i)
            for (int j = 0; j < size; ++j)
                timeOfImbalance = std::max(timeOfImbalance, abs(deltaTimeArr[i] - deltaTimeArr[j]));

        // вычисление доли дисбаланса
        double proportionOfImbalance = deltaTimeArr[0];
        for (int i = 0; i < size; ++i)
            proportionOfImbalance = std::max(proportionOfImbalance, deltaTimeArr[i]);
        proportionOfImbalance = timeOfImbalance / proportionOfImbalance * 100;

        // вывод информации
        std::cout << "iterCounter = " << iterCounter << "\n";
        for (int p = 0; p < size; ++p)
        {
            std::cout << "P" << p << ":\t";
            std::cout << "tasksDone = " << tasksDoneArr[p] << "\t";
            std::cout << "globalRes = " << globalResArr[p] << "\t";
            std::cout << "deltaTime = " << deltaTimeArr[p] << "\t";
            std::cout << "\n";
        }
        std::cout << "timeOfImbalance = " << timeOfImbalance << "\n";
        std::cout << "proportionOfImbalance = " << proportionOfImbalance << "\n";
        std::cout << "\n";
        std::cout << std::flush;
    }
}

// функция для потока-работника
void workerFunction(const int rank, const int size)
{
    // инициализация списка задач
    initTaskList();

    // инициализация массивов для сбора информации на вывод (только для процесса, который будет собирать информацию)
    int *tasksDoneArr = nullptr;
    double *globalResArr = nullptr;
    double *deltaTimeArr = nullptr;
    if (rank == rootRank)
        initBuffersForPrintInfo(tasksDoneArr, globalResArr, deltaTimeArr, size);

    for (; iterCounter < maxIters; iterCounter++)
    {
        // заполнение списка задач и переменных перед началом новой итерации
        fillData(rank, size);

        const double startTime = MPI_Wtime();
        while (true)
        {
            if (tasksLeft == 0)
            {
                // получение новых задач
                const int receivedTasksNum = getNewTasks(rank, size);
                if (receivedTasksNum == 0)
                    break;
            }
            else
            {
                // выполнение задачи
                executeTask();
            }
        }
        const double endTime = MPI_Wtime();

        // Вывод результатов на экран
        printInfo(rank, size, endTime - startTime, tasksDoneArr, globalResArr, deltaTimeArr);
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // освобождение памяти
    if (rank == rootRank)
    {
        free(tasksDoneArr);
        free(globalResArr);
        free(deltaTimeArr);
    }

    // отправка потоку-слушателю сигнала, что пора закончить работу
    const bool exitCode = false;
    MPI_Send(&exitCode, 1, MPI_CXX_BOOL, rank, rank, MPI_COMM_WORLD);
}

// отправить задачу на выполнение другому процессу
void sendTask(const int rank, const int size, const int requester)
{
    int task = -1; // задача на отправку

    // pthread_mutex_lock(&taskListMutex);
    if (tasksLeft > criticalTasksNum)
    {
        task = taskList[taskToSend];
        taskList[taskToSend] = 0;
        tasksLeft--;
        taskToSend = (taskToSend - 1 + tasksPerProcess) % tasksPerProcess;
    }
    // pthread_mutex_unlock(&taskListMutex);

    MPI_Send(&task, 1, MPI_INT, requester, requester + size, MPI_COMM_WORLD);
}

// функция для потока-слушателя
void *listenerFunction(void *args)
{
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (iterCounter < maxIters)
    {
        // приём запроса на задачу
        bool needTasks = false;
        MPI_Status status;
        MPI_Recv(&needTasks, 1, MPI_CXX_BOOL, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, &status);
        const int requester = status.MPI_SOURCE;

        // выходит из цикла, если принят сигнал на завершение программы
        if (needTasks == false)
            break;

        // отправка задачи (если возможно)
        sendTask(rank, size, requester);
    }

    return nullptr;
}

int main(int argc, char **argv)
{
    int rank, size, actualLevelProvided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &actualLevelProvided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    try
    {
        if (actualLevelProvided != MPI_THREAD_MULTIPLE)
            throw new myException("It is not possible to provide the requested level of thread support", 1);
        if (size < 2)
            throw new myException("There must be at least 2 processes", 1);

        pthread_attr_t attrsForListener;
        if (pthread_attr_init(&attrsForListener))
            throw new myException("Cannot initialize attributes");
        if (pthread_attr_setdetachstate(&attrsForListener, PTHREAD_CREATE_JOINABLE))
            throw new myException("Error in setting attributes");

        // создание потока-слушателя
        pthread_t listenerThread;
        if (pthread_create(&listenerThread, &attrsForListener, listenerFunction, nullptr))
            throw new myException("Cannot create listenerThread");
        pthread_attr_destroy(&attrsForListener);

        // логика потока-работника (основного потока)
        const double startTime = MPI_Wtime();
        workerFunction(rank, size);
        const double endTime = MPI_Wtime();
        if (rank == rootRank)
            std::cout << "Total time: " << endTime - startTime << "\n";

        // ожидание завершения потока-слушателя
        if (pthread_join(listenerThread, NULL))
            throw new myException("Cannot join a thread");
    }
    catch (const myException *const e)
    {
        const int code = e->getErrorCode();
        if ((code == 1 && rank == rootRank) || code != 1)
            std::cerr << e->what() << '\n';
    }

    MPI_Finalize();
}

// MPI_Barrier(MPI_COMM_WORLD);
// for (int p = 0; p < size; ++p)
// {
//     if (p == rank && rank == 0)
//         std::cout << "iterCounter = " << iterCounter << "\n";
//     if (p == rank)
//     {
//         std::cout << "P" << p << ":\t";
//         std::cout << "tasksDone = " << tasksDone << "\t";
//         std::cout << "globalRes = " << globalRes << "\t";
//         std::cout << "deltaTime = " << endTime - startTime << "\t";
//         std::cout << "\n";
//     }
//     if (p == rank && rank == size - 1)
//     {
//         std::cout << "\n";
//         double timeOfImbalance;
//     }
//     std::cout << std::flush;
//     MPI_Barrier(MPI_COMM_WORLD);
// }
