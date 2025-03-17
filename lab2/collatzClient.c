#include <stdio.h>

#define MAX_ITER 1000

#ifndef DYNAMIC_COLLATZ
    #include "./libcollatz/collatz.h"
#endif

#ifdef DYNAMIC_COLLATZ
    #include <dlfcn.h>
#endif

int main(){
    #ifdef DYNAMIC_COLLATZ
        void *handle = dlopen("./libcollatz/libcollatz.so", RTLD_LAZY);
        if(!handle){
            fprint("Error: %s\n", dlerror());
            return 1;
        }
        collatz_conjecture = dlsym(handle, "collatz_conjecture");
        test_collatz_convergence = dlsym(handle, "test_collatz_convergence");
    #endif
    
    printf("%d\n", test_collatz_convergence(27,MAX_ITER));
    return 0;
}