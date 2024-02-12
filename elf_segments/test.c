#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// huge global variable initialize to zero in order to have a segment with p_filesz < p_memsz
char huge[1024*1024*16];

__attribute__((__section__(".foo1")))
__attribute__((__noinline__))
void foo1() {
    puts("foo1");
}


__attribute__((__section__(".foo2")))
__attribute__((__noinline__))
void foo2() {
    puts("foo2");
}

int main (int argc, char * argv[])
{
    puts("test");
    foo1();
    foo2();
    if (argc > 1) {
        sleep(atoi(argv[1]));
    }
    return 0;
}