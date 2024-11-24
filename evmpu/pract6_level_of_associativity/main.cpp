
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <new>
#include <vector>

constexpr int CacheL1Size = 48 * 1024;       // L1 - 48KB
constexpr int CacheL2Size = 1280 * 1024;     // L2 - 1.25MB
constexpr int CacheL3Size = 8 * 1024 * 1024; // L3 - 8MB
const int Loops = 10;                        // Для большей точности

using namespace std;

int loadCPU()
{
    const int n = 500;
    vector<vector<int>> A(n, vector<int>(n, 10));
    vector<vector<int>> B(n, vector<int>(n, 10));
    vector<vector<int>> C(n, vector<int>(n, 10));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < n; ++k)
                C[i][k] += A[i][j] * B[j][k];
    return C[0][0];
}

void fiilInArr(int *arr, const int fragments, const int fragmentSize, const int offset)
{
    for (int i = 0; i < fragmentSize; ++i)
    {
        for (int j = 0; j < fragments - 1; ++j)
        {
            arr[j * offset + i] = (j + 1) * offset + i;
        }
        arr[(fragments - 1) * offset + i] = i + 1;
    }
    arr[(fragments - 1) * offset + fragmentSize - 1] = 0;
}

int calculateOffset(int size, int fragments)
{
    int limit = size / fragments;
    int offset = 1;
    while (offset < limit)
        offset *= 2;
    return offset / 2;
}

int main()
{
    ofstream out("results.txt"); // Для записи результатов
    loadCPU();

    for (int fragments = 1; fragments <= 32; ++fragments)
    {
        cout << fragments << " ";
        const int size = CacheL3Size;
        int *arr = new int[size];
        int offset = calculateOffset(size, fragments);
        int fragmentSize = offset;
        fiilInArr(arr, fragments, fragmentSize, offset);

        int pos = 0;
        unsigned long long tick_start = __rdtsc();
        for (int i = 0; i < fragmentSize * fragments * Loops; ++i)
            pos = arr[pos];
        unsigned long long tick_finish = __rdtsc();
        if (pos != 0)
            cout << "How did it happen?\n";
        unsigned long long dt = tick_finish - tick_start;
        out << dt / (fragmentSize * fragments * Loops) << "\n";

        delete[] arr;
    }
    return 0;
}
