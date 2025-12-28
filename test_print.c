#include "jp_basic.h"
#include <stdio.h>



int main() 
{
    // strings
    printf("expected:\nfoo bar baz bizz bazzbuzz\n");
    my_println("foo ", "bar ", "baz", " bizz ", "bazz", "buzz");

    my_println("---------------");

    // numbers
    printf("expected:\n1 2 3 4 5 6 7 8 9 0\n");
    my_println(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    my_println("---------------");

    printf("expected:\nresult goes -> 69 <- text carries on... even with more!\n");
    my_println("result goes -> % <- text carries on...", 69, " even with more!");

    my_println("---------------");
    char huge[8192];
    for (size_t i = 0; i <  8192; i++) {
        huge[i] = 'a' + i % 26;
    }

    printf("expected:\n%s\n", huge);
    my_println("---------------");
    my_println(huge);
    my_println("---------------");

    string result = {0};
    write_string(&result, "foo", 69, 420, "reslut goes -> % <- and text keeps going!!!!", 69);
    my_println(result.data);
    result.len = 0;
    write_string(&result, huge);
    assert("Check write_string for ok-ish-large char * variable" && huge == result.data);
    my_println(result.data);

    // not implemented!
    /* printf("expected:\n420.69 80\n"); */
    /* my_println(420.69f, 80l); */
    return 0;
}

