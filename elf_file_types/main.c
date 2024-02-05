#include <unistd.h>
#include <dlfcn.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
  puts("hello");
  if (argc > 1) {
	  void * h = dlopen(argv[1], RTLD_LAZY);
	  if (!h) {
		  printf("dlopen failed: %s\n", dlerror());
	  }
  }
  
  sleep(10);
}
