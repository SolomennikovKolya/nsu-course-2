#include "circular_buffer.hpp"
#include "gtest/gtest.h"

TEST(CircularBeffer, init_without_args) // (набор тестов, тест)
{
    try
    {
        // Arrange
        const int defaultCapacity = 16; // Эта костанта захардкожена в библиотеке буфера
        // Act
        CircularBuffer<int> a;
        // Assert
        ASSERT_EQ(a.capacity(), defaultCapacity);
        ASSERT_EQ(a.size(), 0);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, init_with_capacity)
{
    try
    {
        const int testCapacity = 10;

        CircularBuffer<char> a(testCapacity);

        ASSERT_EQ(a.capacity(), testCapacity);
        ASSERT_EQ(a.size(), 0);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    ASSERT_THROW(CircularBuffer<char> a(-1), CircularBuffer<char>::exception);
}

TEST(CircularBeffer, init_with_capacity_and_elem)
{
    try
    {
        const int testCapacity = 10;
        const int testElem = 3;

        CircularBuffer<int> a(testCapacity, testElem);

        ASSERT_EQ(a.capacity(), testCapacity);
        ASSERT_EQ(a.size(), testCapacity);
        for (int i = 0; i < testCapacity; i++)
            ASSERT_EQ(a[i], testElem);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    ASSERT_THROW(CircularBuffer<int> a(-1, 3), CircularBuffer<int>::exception);
}

TEST(CircularBeffer, init_with_other_buffer)
{
    try
    {
        CircularBuffer<int> a(10, 3);

        CircularBuffer<int> b(a); // Тестируется этот буфер

        ASSERT_EQ(b.capacity(), a.capacity());
        ASSERT_EQ(b.size(), a.size());
        for (int i = 0; i < a.size(); i++)
            ASSERT_EQ(b[i], a[i]);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

// TEST(CircularBeffer, destructor)
// {
//     {
//         CircularBuffer<int> a(10, 3);
//     }
//     ASSERT_THROW(int s = a.size());
// }

TEST(CircularBeffer, size)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(4);
        a.push_front(6);
        a.push_front(1);

        ASSERT_EQ(a.size(), 3);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, capacity)
{
    try
    {
        const int testCapacity = 10;
        CircularBuffer<int> a(testCapacity, 3);

        ASSERT_EQ(a.capacity(), testCapacity);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, empty)
{
    try
    {
        CircularBuffer<int> a(10);
        CircularBuffer<int> b(10, 3);

        ASSERT_TRUE(a.empty());
        ASSERT_FALSE(b.empty());
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, full)
{
    try
    {
        CircularBuffer<int> a(10);
        CircularBuffer<int> b(10, 3);

        ASSERT_FALSE(a.full());
        ASSERT_TRUE(b.full());
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, reserve)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(1);
        a.push_back(1);

        ASSERT_EQ(a.reserve(), 7);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, item_by_index)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]
        CircularBuffer<int> b(10);

        ASSERT_EQ(a[0], 0);
        ASSERT_EQ(a[1], 1);
        ASSERT_EQ(a[2], 2);
        ASSERT_EQ(a[3], 3);
        ASSERT_EQ(a[4], 0);
        ASSERT_EQ(a[-1], 3);
        int a0_const = a[0];
        a0_const = 100;
        ASSERT_EQ(a[0], 0);
        int &a0 = a[0];
        a0 = 100;
        ASSERT_EQ(a[0], 100);
        ASSERT_THROW(b[0], CircularBuffer<int>::exception);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, at)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]

        ASSERT_EQ(a.at(0), 0);
        ASSERT_EQ(a.at(1), 1);
        ASSERT_EQ(a.at(2), 2);
        ASSERT_EQ(a.at(3), 3);
        ASSERT_THROW(a.at(4), CircularBuffer<int>::exception);
        ASSERT_THROW(a.at(-1), CircularBuffer<int>::exception);
        int a0_const = a.at(0);
        a0_const = 100;
        ASSERT_EQ(a.at(0), 0);
        int &a0 = a.at(0);
        a0 = 100;
        ASSERT_EQ(a.at(0), 100);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, front)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]
        CircularBuffer<int> b(10);

        int front_const = a.front();
        front_const = 100;
        ASSERT_EQ(a[0], 0);
        int &front = a.front();
        ASSERT_EQ(front, 0);
        front = 100;
        ASSERT_EQ(a[0], 100);
        ASSERT_THROW(b.front(), CircularBuffer<int>::exception);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, back)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]
        CircularBuffer<int> b(10);

        int back_const = a.back();
        back_const = 100;
        ASSERT_EQ(a[3], 3);
        int &back = a.back();
        ASSERT_EQ(back, 3);
        back = 100;
        ASSERT_EQ(a[3], 100);
        ASSERT_THROW(b.back(), CircularBuffer<int>::exception);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, is_linearized)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]
        CircularBuffer<int> b(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        // b = [ 1 2 3 ]

        ASSERT_FALSE(a.is_linearized());
        ASSERT_TRUE(b.is_linearized());
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, linearize)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]

        ASSERT_FALSE(a.is_linearized());
        a.linearize();
        ASSERT_TRUE(a.is_linearized());
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, rotate)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]

        ASSERT_EQ(a[0], 0);
        ASSERT_FALSE(a.is_linearized());
        a.rotate(2); // a = [ 2 3 0 1 ]
        ASSERT_EQ(a[0], 2);
        ASSERT_TRUE(a.is_linearized());
        a.rotate(-1); // a = [ 1 2 3 0 ]
        ASSERT_EQ(a[0], 1);
        ASSERT_TRUE(a.is_linearized());
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, set_capacity)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]

        ASSERT_THROW(a.set_capacity(-1), CircularBuffer<int>::exception);
        ASSERT_THROW(a.set_capacity(0), CircularBuffer<int>::exception);
        ASSERT_FALSE(a.is_linearized());
        a.set_capacity(100);
        ASSERT_TRUE(a.is_linearized());
        a.set_capacity(2); // a = [ 0 1 ]
        ASSERT_EQ(a.size(), 2);
        a.push_back(2); // a = [ 1 2 ]
        a.set_capacity(2);
        ASSERT_TRUE(a.is_linearized());
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, resize)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]

        ASSERT_THROW(a.resize(-1, 1), CircularBuffer<int>::exception);
        ASSERT_EQ(a.size(), 4);
        a.resize(6, 7); // a = [ 0 1 2 3 7 7 ]
        ASSERT_EQ(a.size(), 6);
        ASSERT_EQ(a[4], 7);
        ASSERT_EQ(a[5], 7);
        a.resize(3, 7);
        ASSERT_EQ(a.size(), 3);
        a.resize(3, 7);
        ASSERT_EQ(a.size(), 3);
        a.resize(0, 7);
        ASSERT_EQ(a.size(), 0);
        a.resize(11, 7);
        ASSERT_EQ(a.size(), 11);
        ASSERT_EQ(a.capacity(), 11);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, assignment)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]
        CircularBuffer<int> b(7);

        b = a;

        ASSERT_EQ(a.capacity(), b.capacity());
        ASSERT_EQ(a.size(), b.size());
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, swap)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_front(0);
        // a = [ 0 1 2 3 ]
        CircularBuffer<int> last_a(a);
        CircularBuffer<int> b(7);
        CircularBuffer<int> last_b(b);

        a.swap(b);

        ASSERT_TRUE(a == last_b);
        ASSERT_TRUE(b == last_a);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, push_back)
{
    try
    {
        CircularBuffer<int> a(4); // 4 1 2 3
        a.push_back(0);
        ASSERT_EQ(a[0], 0);
        a.push_back(1);
        ASSERT_EQ(a[1], 1);
        a.push_back(2);
        ASSERT_EQ(a[2], 2);
        a.push_back(3);
        ASSERT_EQ(a[3], 3);
        a.push_back(4);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[-1], 4);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, push_front)
{
    try
    {
        CircularBuffer<int> a(4);
        a.push_front(0);
        ASSERT_EQ(a[0], 0);
        a.push_front(1);
        ASSERT_EQ(a[0], 1);
        a.push_front(2);
        ASSERT_EQ(a[0], 2);
        a.push_front(3);
        ASSERT_EQ(a[0], 3);
        a.push_front(4);
        ASSERT_EQ(a[0], 4);
        ASSERT_EQ(a[-1], 1);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, pop_back)
{
    try
    {
        CircularBuffer<int> a(4, 0);
        a.pop_back();
        ASSERT_EQ(a.size(), 3);
        a.pop_back();
        ASSERT_EQ(a.size(), 2);
        a.pop_back();
        ASSERT_EQ(a.size(), 1);
        a.pop_back();
        ASSERT_EQ(a.size(), 0);
        ASSERT_NO_THROW(a.pop_back());
        ASSERT_EQ(a.size(), 0);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, pop_front)
{
    try
    {
        CircularBuffer<int> a(4, 0);
        a.pop_front();
        ASSERT_EQ(a.size(), 3);
        a.pop_front();
        ASSERT_EQ(a.size(), 2);
        a.pop_front();
        ASSERT_EQ(a.size(), 1);
        a.pop_front();
        ASSERT_EQ(a.size(), 0);
        ASSERT_NO_THROW(a.pop_front());
        ASSERT_EQ(a.size(), 0);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, insert)
{
    try
    {
        CircularBuffer<int> a(4);
        a.push_back(1);
        a.push_back(3);
        a.insert(1, 2);
        ASSERT_EQ(a.size(), 3);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 2);
        ASSERT_EQ(a[2], 3);
        a.insert(3, 4);
        ASSERT_EQ(a.size(), 4);
        ASSERT_EQ(a[3], 4);
        ASSERT_THROW(a.insert(0, 0), CircularBuffer<int>::exception);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, erase)
{
    try
    {
        CircularBuffer<int> a(4, 0);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_back(4);
        a.erase(1, 3);
        ASSERT_EQ(a.size(), 2);
        ASSERT_EQ(a[0], 1);
        ASSERT_EQ(a[1], 4);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBeffer, clear)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        a.push_back(4);
        a.clear();
        ASSERT_EQ(a.size(), 0);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

// TEST(CircularBeffer, print)
// {
//     try
//     {
//         CircularBuffer<int> a(10);
//         a.push_back(1);
//         a.push_back(2);
//         a.push_back(3);
//         a.push_back(4);
//         ASSERT_NO_THROW(a.print());
//     }
//     catch (const CircularBuffer<int>::exception &e)
//     {
//         std::cerr << e.what() << '\n';
//     }
// }

// TEST(CircularBeffer, printBufferInMemory)
// {
//     try
//     {
//         CircularBuffer<int> a(10);
//         a.push_back(1);
//         a.push_back(2);
//         a.push_back(3);
//         a.push_back(4);
//         ASSERT_NO_THROW(a.printBufferInMemory());
//     }
//     catch (const CircularBuffer<int>::exception &e)
//     {
//         std::cerr << e.what() << '\n';
//     }
// }

TEST(CircularBefferIterator, init)
{
    try
    {
        CircularBuffer<int> a(10);
        ASSERT_NO_THROW(CircularBuffer<int>::iterator it(a, 0));
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, dereference)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        CircularBuffer<int>::iterator it(a, 0);
        ASSERT_EQ(*it, a[0]);
        a.pop_back();
        ASSERT_THROW(*it, CircularBuffer<int>::exception);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, increase_by_1)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(0);
        a.push_back(1);
        a.push_back(2);

        CircularBuffer<int>::iterator it(a, 0);
        ASSERT_EQ(*it, a[0]);

        CircularBuffer<int>::iterator last_it = it++;
        ASSERT_EQ(*last_it, a[0]);
        ASSERT_EQ(*it, a[1]);

        last_it = ++it;
        ASSERT_EQ(*last_it, a[2]);
        ASSERT_EQ(*it, a[2]);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, decrease_by_1)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(0);
        a.push_back(1);
        a.push_back(2);

        CircularBuffer<int>::iterator it(a, 2);
        ASSERT_EQ(*it, a[2]);

        CircularBuffer<int>::iterator last_it = it--;
        ASSERT_EQ(*last_it, a[2]);
        ASSERT_EQ(*it, a[1]);

        last_it = --it;
        ASSERT_EQ(*last_it, a[0]);
        ASSERT_EQ(*it, a[0]);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, increase_by_shift)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        CircularBuffer<int>::iterator it(a, 0);
        ASSERT_EQ(*it, a[0]);
        it = it + 1;
        it += 1;
        ASSERT_EQ(*it, a[2]);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, decrease_by_shift)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        CircularBuffer<int>::iterator it(a, 2);
        ASSERT_EQ(*it, a[2]);
        it = it - 1;
        it -= 1;
        ASSERT_EQ(*it, a[0]);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, equal)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        CircularBuffer<int>::iterator it1(a, 1);
        CircularBuffer<int>::iterator it2(a, 0);
        it2++;
        ASSERT_TRUE(it1 == it2);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, not_equal)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        CircularBuffer<int>::iterator it1(a, 1);
        CircularBuffer<int>::iterator it2(a, 0);
        it2++;
        ASSERT_FALSE(it1 != it2);
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

TEST(CircularBefferIterator, for_auto)
{
    try
    {
        CircularBuffer<int> a(10);
        a.push_back(1);
        a.push_back(2);
        a.push_back(3);
        int id = 0;
        for (auto i : a)
        {
            ASSERT_EQ(i, a[id]);
            id++;
        }
    }
    catch (const CircularBuffer<int>::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}