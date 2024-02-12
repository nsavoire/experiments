#include <stdio.h>
#include <stdlib.h>

static __attribute__((constructor)) void init() {
	puts("libfoo:init");
}

#ifdef PT_INTERP
const char interp[] __attribute__ ((section(".interp"))) = PT_INTERP;

int main() {
	puts("libfoo:main");
	exit(0);
}

#endif