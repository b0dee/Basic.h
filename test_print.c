#include "jp_basic.h"
#include <stdio.h>



int main() 
{

    string result = {0};
    // strings
    my_println("Testing basic strings....");
    printf("expected:\nfoo bar baz bizz bazzbuzz\n");
    my_println("foo ", "bar ", "baz", " bizz ", "bazz", "buzz");

    my_println("---------------");

    // numbers
    my_println("Testing integers....");
    my_println("expected:\n1 2 3 4 5 6 7 8 9 0");
    my_println(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    my_println("---------------");

    my_println("Testing basic format expansion...");
    printf("expected:\nresult goes -> 69 <- text carries on... even with more!\n");
    my_println("result goes -> % <- text carries on...", 69, " even with more!");
    my_println("---------------");

    my_println("Testing huge (not really but bigger than typical) message printing!!");
    char huge[8192];
    for (size_t i = 0; i <  8192; i++) {
        huge[i] = 'a' + i % 26;
    }


    printf("expected:\n%s\n", huge);
    my_println("---------------");
    my_println(huge);
    my_println("---------------");

    result = (string){0};
    write_string(&result, "foo ", 69, 420, "reslut goes -> % <- and text keeps going!!!!", 69);
    my_println(result.data);
    my_println("---------------");

    result.len = 0;
    write_string(&result, huge);
    string huge_str = cstrlen(huge);
    assert(string_cmp(huge_str, result));
    my_println("---------------");
    my_println("Strings match? ", string_cmp(huge_str, result), "huge_str len: ", huge_str.len, "result len: ", result.len);
    my_println("---------------");


    // not implemented!
    /* printf("expected:\n420.69 80\n"); */
    /* my_println(420.69f, 80l); */
    return 0;
}

