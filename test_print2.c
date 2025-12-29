#include "jp_basic.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

static void sep(const char *name)
{
    my_printfln("--- %s ---", name);
    printf("--- %s ---\n", name);
}

int main(void)
{
    bool testtrue  = true;
    bool testfalse = false;

    char ca = 'a';
    char cZ = 'Z';
    char c0 = '0';

    sep("my_print (no formatting)");

    my_println("hello", "world");
    printf("hello world\n");

    my_println("one");
    printf("one\n");

    my_println("a", "b", "c", "d");
    printf("a b c d\n");

    my_println("ints:", 1, -2, 3);
    printf("ints: 1 -2 3\n");

    my_println("bools:", testtrue, testfalse, testtrue);
    printf("bools: true false true\n");

    my_println("chars:", ca, cZ, c0);
    printf("chars: a Z 0\n");

    my_println("doubles:", 1.5, 2.0, -3.25);
    printf("doubles: 1.5 2 -3.25\n");

    my_println("floats:", 1.5f, 2.0f, -3.25f);
    printf("floats: 1.5 2 -3.25\n");

    sep("my_print percent handling");

    my_println("100% done");
    printf("100%% done\n");

    my_println("%", "%%", "%%%", "done");
    printf("%% %%%% %%%%%% done\n");

    my_println("value:", "%d", 42);
    printf("value: %%d 42\n");

    my_println("mixed", "%x", "%f", "%s");
    printf("mixed %%x %%f %%s\n");

    sep("my_printf (formatting)");

    my_printfln("hello world");
    printf("hello world\n");

    my_printfln("value = %d", 42);
    printf("value = 42\n");

    my_printfln("signed = %d", -123);
    printf("signed = -123\n");

    my_printfln("hex %x", 255);
    printf("hex ff\n");

    my_printfln("HEX %X", 255);
    printf("HEX FF\n");

    my_printfln("double %.2", 3.14159);
    printf("double 3.14\n");

    my_printfln("float %.2f", 3.14159f);
    printf("float 3.14\n");

    my_printfln("string %s", "hello");
    printf("string hello\n");

    my_printfln("char %c", ca);
    printf("char a\n");

    sep("my_printf percent handling");

    my_printfln("100%% done");
    printf("100%% done\n");

    my_printfln("%%");
    printf("%%\n");

    my_printfln("%%%d%%", 5);
    printf("%%5%%\n");

    my_printfln("percent %%%% percent");
    printf("percent %% percent\n");

    my_printfln("%d%% complete", 50);
    printf("50%% complete\n");

    my_printfln("mix %% %d %% %x %%", 10, 10);
    printf("mix %% 10 %% a %%\n");

    sep("inline char literals (explicitly supported)");

    my_println("inline chars:", 'a', 'Z', '0');
    printf("inline chars: a Z 0\n");

    my_printfln("inline %c %c %c", 'a', 'Z', '0');
    printf("inline a Z 0\n");

    sep("fixed-width integers");

    my_printfln("u8 %u", (uint8_t)255);
    printf("u8 255\n");

    my_printfln("i8 %d", (int8_t)-128);
    printf("i8 -128\n");

    my_printfln("u16 %u", (uint16_t)65535);
    printf("u16 65535\n");

    my_printfln("i16 %d", (int16_t)-32768);
    printf("i16 -32768\n");

    my_printfln("u32 %u", (uint32_t)4294967295u);
    printf("u32 4294967295\n");

    my_printfln("i32 %d", (int32_t)-2147483648);
    printf("i32 -2147483648\n");

    my_printfln("u64 %llu", (uint64_t)18446744073709551615ull);
    printf("u64 18446744073709551615\n");

    my_printfln("i64 %lld", (int64_t)-9223372036854775807ll);
    printf("i64 -9223372036854775807\n");

    sep("null handling");

    my_printfln("null = %s", (char *)0);
    printf("null = (null)\n");

    return 0;
}
