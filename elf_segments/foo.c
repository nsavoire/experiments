#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

__attribute__((aligned(0x200000))) int foo = 1;

int __attribute__ ((noinline))
is_aligned (void *p, int align)
{
  return (((uintptr_t) p) & (align - 1)) != 0;
}

void do_load_test()
{
  printf ("foo: %p\n", &foo);
  if (is_aligned (&foo, 0x200000)) {
    abort ();
  }
}
