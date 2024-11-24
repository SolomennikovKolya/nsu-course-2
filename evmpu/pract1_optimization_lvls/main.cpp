#include <iostream>
#include <math.h>

double mySin(double x, long long n)
{
    x = fmod(x, 2 * M_PI);
    double ans = x;
    double d = x;
    for (long long i = 2; i <= n; i++)
    {
        d = -d * x * x / (2 * i - 1) / (2 * i - 2);
        ans += d;
    }
    return ans;
}

int main(int args, char *argv[])
{
    long long n = atoll(argv[1]);
    double angle = 1.57; // for example

    double res = mySin(angle, n);
    std::cout << res << std::endl;
    return 0;
}
