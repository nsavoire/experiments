#include <chrono>
#include <cstdio>
#include <cstdlib>

extern "C" {
__attribute__((noinline)) void foo(int n) {
    static int x = 0;
    for(int i=0; i<n; i++) {
        x+=i*i;
    }
}
}

int main(int argc, char* argv[]) {
    int inner_loop = argc > 1 ? atoi(argv[1]) : 30000;
    int outer_loop = argc > 2 ? atoi(argv[2]) : 50000;
    int outer_loop2 = argc > 3 ? atoi(argv[3]) : 5;

    for(int i=0; i<outer_loop2; ++i) {
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        for(int i=0; i<outer_loop; i++) {
            foo(inner_loop);
        }
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        printf("foo takes %ld ns\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count()/outer_loop);
    }
}