
#include <dlfcn.h>

int main()
{
	void *handle = dlopen("dyn_runtime.so");
	if (!handle)
	{
		fputs(dlerror(), stderr);
		return 0;
	}

	void (*hello)(void);
	char *error;
	hello = dlsym(handle, "hello_from_dyn_runtime_lib");
	if ((error = dlerror()) != NULL)  {
   		fputs(error, stderr);
    		exit(1);
  	}

	*hello();

	lclose(handle);
	return 0;
}

