#include "jp_basic.h"
#include <stdio.h>



int main() 
{

    printf("hello\n");
    bool test = true;
    my_println("foo", "bar", "baz", test);
    return 0;

    string result = {0};
    // strings
    my_printfln("Testing basic strings....");
    printf("expected:\nfoo bar baz bizz bazzbuzz\n");
    my_printfln("foo ", "bar ", "baz", " bizz ", "bazz", "buzz");

    my_printfln("---------------");

    // numbers
    my_printfln("Testing integers....");
    my_printfln("expected:\n1 2 3 4 5 6 7 8 9 0");
    my_printfln(1, 2, 3, 4, 5, 6, 7, 8, 9, 0);

    my_printfln("---------------");

    my_printfln("Testing basic format expansion...");
    printf("expected:\nresult goes -> 69 <- text carries on... even with more!\n");
    my_printfln("result goes -> % <- text carries on...", 69, " even with more!");
    my_printfln("---------------");

    my_printfln("Testing huge (not really but bigger than typical) message printing!!");
    char huge[8192];
    for (size_t i = 0; i <  8192; i++) {
        huge[i] = 'a' + i % 26;
    }


    printf("expected:\n%s\n", huge);
    my_printfln("---------------");
    my_printfln(huge);
    my_printfln("---------------");

    result = (string){0};
    writef_string(&result, "foo ", 69, 420, "reslut goes -> % <- and text keeps going!!!!", 69);
    my_printfln(result.data);
    my_printfln("---------------");

    /* my_printfln(result); */
    result.len = 0;
    writef_string(&result, huge);
    string huge_str = cstrlen(huge);
    assert(string_cmp(huge_str, result));
    my_printfln("---------------");
    my_printfln("Strings match? ", string_cmp(huge_str, result), "huge_str len: ", huge_str.len, "result len: ", result.len);
    my_printfln("---------------");


    // not implemented!
    /* printf("expected:\n420.69 80\n"); */
    /* my_printfln(420.69f, 80l); */
    return 0;
}

