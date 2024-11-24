
#include <chrono>
#include <fstream>
#include <intrin.h>
#include <iostream>
#include <random>
#include <set>
#include <vector>

using namespace std;

constexpr int N_min = 1 * 1024 / 4;         // 1 KB
constexpr int N_max = 64 * 1024 * 1024 / 4; // 32 MB
const int cycles_count = 7;                 // Количество повторов обхода массива
const double K = 1.1;                       // Коэффициент умножения размера массива
const int extra_cycles = 3;                 // Дополнительные повторы цикла, чтобы избежать возможного влияния других процессов

int loadCPU()
{
    const int n = 1000;
    vector<vector<int>> A(n, vector<int>(n, 10));
    vector<vector<int>> B(n, vector<int>(n, 10));
    vector<vector<int>> C(n, vector<int>(n, 10));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i][k] += A[i][j] * B[j][k];
    return C[0][0];
}

void fill_in_1(int arr[], int arr_size)
{
    for (int i = 0; i < arr_size - 1; ++i)
        arr[i] = i + 1;
    arr[arr_size - 1] = 0;
}

void fill_in_2(int arr[], int arr_size)
{
    arr[0] = int(arr_size) - 1;
    for (int i = int(arr_size) - 1; i > 0; --i)
        arr[i] = i - 1;
}

int mod(int x, int base)
{
    if (x > 0)
        return x % base;
    return x % base + base;
}

void fill_in_3(int arr[], int arr_size)
{
    for (int i = 0; i < arr_size; ++i)
        arr[i] = -1;
    mt19937 gen(static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count()));
    uniform_int_distribution<> dist(1, arr_size - 1);
    int cur_pos = 0;
    for (int q = 0; q < arr_size - 1; ++q)
    {
        int next = dist(gen);
        if (arr[next] != -1 || next == cur_pos)
        {
            int next_r = next;
            while (arr[next_r] != -1 || next_r == cur_pos)
            {
                next_r++;
                if (next_r == arr_size)
                    next_r = 0;
            }
            int next_l = next;
            while (arr[next_l] != -1 || next_l == cur_pos)
            {
                next_l--;
                if (next_l == 0)
                    next_l = arr_size - 1;
            }
            if (mod(next_r - next, arr_size) < mod(next_l - next, arr_size))
                next = next_r;
            else
                next = next_l;
        }
        arr[cur_pos] = next;
        cur_pos = next;
    }
    arr[cur_pos] = 0;
}

int main()
{
    ofstream out("results.txt"); // Для вывода результатов
    int *arr = new int[N_max];

    // Разгон процессора
    if (loadCPU() == 0)
        cout << "QQ";

    for (int arr_size = N_min; arr_size < N_max; arr_size = int(arr_size * K))
    {
        cout << arr_size << "\t";
        // fill_in_1(arr, arr_size); // прямой обход
        // fill_in_2(arr, arr_size); // обратный обход
        fill_in_3(arr, arr_size); // случайный обход

        // Прогрев кэша
        int x = 0;
        for (int i = 0; i < arr_size * cycles_count; ++i)
            x = arr[x];
        if (x != 0)
            cout << "How did it happen?\n";

        unsigned long long min_ticks = 0;
        for (int q = 0; q < extra_cycles; ++q)
        {
            x = 0;
            unsigned long long tick_start = __rdtsc();
            for (int i = 0; i < arr_size * cycles_count; ++i)
                x = arr[x];
            unsigned long long tick_finish = __rdtsc();
            if (x != 0)
                cout << "How did it happen?\n";
            unsigned long long dt = tick_finish - tick_start;
            if (min_ticks == 0 || dt < min_ticks)
                min_ticks = dt;
        }

        out << arr_size * 4 / 1024 << "\t";
        out << min_ticks / (arr_size * cycles_count) << "\n";
    }
    delete (arr);
    cout << "FINISHED\n";
    return 0;
}