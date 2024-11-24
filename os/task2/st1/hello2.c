
#include <syscall.h>
#include <unistd.h>

void myWrite(int descriptor, const void *buffer, size_t len)
{
	syscall(SYS_write, descriptor, buffer, len);
}

int main()
{
	myWrite(1, "Hello World\n", 12);
	return 0;
}
