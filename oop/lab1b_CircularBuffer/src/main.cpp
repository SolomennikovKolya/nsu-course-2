#include "circular_buffer.hpp"

#include <iostream>

using namespace std;

int main()
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_back(4);
        a.push_back(5);
        a.push_back(6);
        a.push_back(7);
        a.push_back(8);
        a.push_back(9);
        a.push_back(10);
        a.push_back(11);
        a.push_back(12);
        a.pop_front();
        a.pop_front();
        a.print();               // [ 5 6 7 8 9 10 11 12 ] size = 8 capacity = 10
        a.printBufferInMemory(); // [ 11 12 _ _ 5 6 7 8 9 10 ]
        cout << "[ ";
        for (auto i : a)
            cout << i << " ";
        cout << "]\n"; // [ 5 6 7 8 9 10 11 12 ]
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    cout << "Program finished\n";
    return 0;
}