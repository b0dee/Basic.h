#include <stdio.h>

int main(void) {
    for(size_t i = 1; i < 129; ++i) {
        printf("#define FOREACH_%lld(func", i);
        for (size_t j = 1; j <= i; ++j) { 
            printf(", _%lld", j);
        }
        printf(")");
        for (size_t j = 1; j <= i; ++j) { 
            if (j > 1) printf(",");
            printf(" func(_%lld)", j);
        }
        printf("\n");
    }
}

