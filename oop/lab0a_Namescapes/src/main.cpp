#include "../includes/module1.h"
#include "../includes/module2.h"
#include <iostream>

int main(int argc, char **argv)
{
	std::cout << "Hello world!"
			  << "\n";

	std::cout << Module1::getMyName() << "\n";
	std::cout << Module2::getMyName() << "\n";

	using namespace Module1;
	std::cout << getMyName() << "\n"; // (A)
	std::cout << Module2::getMyName() << "\n";

	using namespace Module2;				   // (B)
	std::cout << Module1::getMyName() << "\n"; // COMPILATION ERROR (C)

	using Module2::getMyName;
	std::cout << getMyName() << "\n"; // (D)
}
