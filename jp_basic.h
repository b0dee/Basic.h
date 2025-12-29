// basic.h
// All your basic needs imported at once
// * Assertions (imported from assert.h)
// * Type shorthands (imported from stdint.h and typedef'd)
// * Better printing! (See 'Better Printing API')
// * File IO @Incomplete (currently stdio.h)
// * Allocators @Incomplete (currently malloc.h)
// * Go-like strings @Incomplete
// * String formatting @Incomplete

// Key Info:
// Strings are slices until .owner property is non-null
// _Always_ call string_free - if it is not an owner it will just return
// Anything tagged @Memory allocates/manipulates owning buffer

#ifndef _BASIC_H
#define _BASIC_H

#include <assert.h>
#define panic(msg) assert(msg && 0)
#include <stdbool.h>
#include <stdint.h>
typedef int8_t  s8;
typedef int16_t s16;
typedef int8_t  s32;
typedef int8_t  s64;
#define S8_MAX  INT8_MAX
#define S16_MAX INT16_MAX
#define S32_MAX INT32_MAX
#define S64_MAX INT64_MAX

typedef uint8_t  byte;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#define U8_MAX  UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

#define dynarray(type) struct { \
    type *data;\
    size_t len;\
    size_t cap;\
    }

#define da_append(arr, item) \
    do { \
        if (arr.len >= arr.cap) { \
            if (arr.cap == 0) arr.cap = 256; \
            else arr.cap *= 2; \
            arr.data = realloc(arr.data, arr.cap*sizeof(*arr.data)); \
            assert(arr.data && "We requested more memory but the computer said \"No\"!"); \
        } \
        arr.data[arr.len++] = item; \
    } while (0);

// Strings
typedef struct string {
    bool  _owner;  // internal
    char  *data;
    size_t len;
    size_t _cap;    // internal
    bool next;
} string;

// String functions
string cstrlen(char *source);  // Creates slice over source

bool   jp_isspace(char c); // ISO compliant
bool   jp_isalpha(char c); // mask off bits 32 then check if it's between 65 + 90 (checks)
                           // this saves 2 comparisons as mask makes upper + lower the same bitwise
bool   jp_isnum(char c);

// String searching
bool   string_cmp(string a, const string b);
int    string_indexof(const string haystack, const string needle);
bool   string_contains(const string haystack, const string needle);

// Dest is pointer to reduce noise calling API
// pass NULL to allocate new string
// pass pointer to string if wanting to append
string string_copy(string *dest, const string source); // @Cleanup remove after string formatted writing is good....
string string_write(string *dest, const string data) ;

// String manipulation
// All pass by value and return a new string (slice)
string string_trim_whitespace(string source);
string string_trim_prefix(string source, char *prefix);
string string_trim_suffix(string source, char *suffix);
string string_trim_before(string source, char *target);
string string_trim_after(string source, char *target);
// As a result they can be chained i.e. 
// string after_arrow_not_space = string_trim_whitespace(string_trim_prefix(source, "->"));

// Except for this one, which works sort of like Go's SplitSeq
string string_split_iter(string *source, string delim);
// Which allows for the following style of code:
// (in the below example, source jp_is a preexisting string slice)
// ```
// string line;
// while ((line = string_split_iter(&source, "\n")).next) {
//     printf("Line: %.*s\n", (int)line.len, line.data)
// }
// ```

// StringBuilder functions
size_t        sb_write(string *sb, char *text);
void          sb_appendf(string *sb, char *fmt, ...);
void          sb_appends(string *sb, string slice);

// Better Printing
typedef enum {
    T_CHAR,
    T_SCHAR,
    T_UCHAR,

    T_SHORT,
    T_USHORT,

    T_INT,
    T_UINT,

    T_LONG,
    T_ULONG,

    T_LLONG,
    T_ULLONG,

    T_BOOL,

    T_FLOAT,
    T_DOUBLE,
    T_LDOUBLE,

    T_SIZE,
    T_PTRDIFF,

    T_WCHAR,

    T_STR,
    T_PTR
} tag_t;

typedef struct {
    tag_t tag;
    union {
    long long          i;   // signed integers
    unsigned long long u;   // unsigned integers
    double             d;   // floats/doubles
    long double        ld;  // long floats/doubles
    void              *p;   // pointers
    char              *s;   // strings
    bool               b;   // boolean
    };
} TypeInfo;

TypeInfo arg_char(char x)                     { return (TypeInfo){ T_CHAR,    .i   = x }; }
TypeInfo arg_schar(signed char x)             { return (TypeInfo){ T_SCHAR,   .i  = x }; }
TypeInfo arg_uchar(unsigned char x)           { return (TypeInfo){ T_UCHAR,   .u  = x }; }

TypeInfo arg_short(short x)                   { return (TypeInfo){ T_SHORT,   .i  = x }; }
TypeInfo arg_ushort(unsigned short x)         { return (TypeInfo){ T_USHORT,  .u = x }; }

TypeInfo arg_int(int x)                       { return (TypeInfo){ T_INT,     .i   = x }; }
TypeInfo arg_uint(unsigned int x)             { return (TypeInfo){ T_UINT,    .u  = x }; }

TypeInfo arg_long(long x)                     { return (TypeInfo){ T_LONG,    .i   = x }; }
TypeInfo arg_ulong(unsigned long x)           { return (TypeInfo){ T_ULONG,   .u  = x }; }

TypeInfo arg_llong(long long x)               { return (TypeInfo){ T_LLONG,   .i  = x }; }
TypeInfo arg_ullong(unsigned long long x)     { return (TypeInfo){ T_ULLONG,  .u = x }; }

TypeInfo arg_bool(bool x)                     { return (TypeInfo){ T_BOOL,    .b   = x }; }

TypeInfo arg_float(float x)                   { return (TypeInfo){ T_FLOAT,   .d   = x }; }
TypeInfo arg_double(double x)                 { return (TypeInfo){ T_DOUBLE,  .d   = x }; }
TypeInfo arg_ldouble(long double x)            { return (TypeInfo){ T_LDOUBLE, .ld  = x }; }

TypeInfo arg_str(char *x)                     { return (TypeInfo){ T_STR,     .s   = x }; }
TypeInfo arg_cstr(const char *x)              { return (TypeInfo){ T_STR,     .s   = (char *)x }; }

TypeInfo arg_ptr(void *x)                     { return (TypeInfo){ T_PTR,     .p   = x }; }
TypeInfo arg_cptr(const void *x)              { return (TypeInfo){ T_PTR,     .p   = (void *)x }; }

#define TypedArg(x) _Generic((x), \
    char:               arg_char, \
    signed char:        arg_schar, \
    unsigned char:      arg_uchar, \
    short:              arg_short, \
    unsigned short:     arg_ushort, \
    int:                arg_int, \
    unsigned int:       arg_uint, \
    long:               arg_ulong, \
    unsigned long:      arg_ulong, \
    long long:          arg_llong, \
    unsigned long long: arg_ullong, \
    bool:               arg_bool, \
    float:              arg_float, \
    double:             arg_double, \
    long double:        arg_ldouble, \
    char *:             arg_str, \
    const char *:       arg_cstr, \
    void *:             arg_ptr, \
    const void *:       arg_cptr \
)(x)

// Macro Helpers
// Yes it's ugly, but it works!
// If you are using more than 128 args, wtf.
// Feel fre to extend your own copy!
#define VA_COUNT(...) VA_COUNT_( \
     __VA_ARGS__, \
     128,127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101, \
     100,99,98,97,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64, \
     63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)

#define VA_COUNT_( \
     _1,_2,_3,_4,_5,_6,_7,_8,_9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
     _71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
     _81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
     _91,_92,_93,_94,_95,_96,_97,_98,_99,_100, \
     _101,_102,_103,_104,_105,_106,_107,_108,_109,_110, \
     _111,_112,_113,_114,_115,_116,_117,_118,_119,_120, \
     _121,_122,_123,_124,_125,_126,_127,_128, \
     N,...) N


#define FOREACH_1(func, _1) func(_1)
#define FOREACH_2(func, _1, _2) func(_1), func(_2)
#define FOREACH_3(func, _1, _2, _3) func(_1), func(_2), func(_3)
#define FOREACH_4(func, _1, _2, _3, _4) func(_1), func(_2), func(_3), func(_4)
#define FOREACH_5(func, _1, _2, _3, _4, _5) func(_1), func(_2), func(_3), func(_4), func(_5)
#define FOREACH_6(func, _1, _2, _3, _4, _5, _6) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6)
#define FOREACH_7(func, _1, _2, _3, _4, _5, _6, _7) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7)
#define FOREACH_8(func, _1, _2, _3, _4, _5, _6, _7, _8) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8)
#define FOREACH_9(func, _1, _2, _3, _4, _5, _6, _7, _8, _9) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9)
#define FOREACH_10(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10)
#define FOREACH_11(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11)
#define FOREACH_12(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12)
#define FOREACH_13(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13)
#define FOREACH_14(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14)
#define FOREACH_15(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15)
#define FOREACH_16(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16)
#define FOREACH_17(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17)
#define FOREACH_18(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18)
#define FOREACH_19(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19)
#define FOREACH_20(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20)
#define FOREACH_21(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21)
#define FOREACH_22(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22)
#define FOREACH_23(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23)
#define FOREACH_24(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24)
#define FOREACH_25(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25)
#define FOREACH_26(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26)
#define FOREACH_27(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27)
#define FOREACH_28(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28)
#define FOREACH_29(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29)
#define FOREACH_30(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30)
#define FOREACH_31(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31)
#define FOREACH_32(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32)
#define FOREACH_33(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33)
#define FOREACH_34(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34)
#define FOREACH_35(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35)
#define FOREACH_36(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36)
#define FOREACH_37(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37)
#define FOREACH_38(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38)
#define FOREACH_39(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39)
#define FOREACH_40(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40)
#define FOREACH_41(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41)
#define FOREACH_42(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42)
#define FOREACH_43(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43)
#define FOREACH_44(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44)
#define FOREACH_45(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45)
#define FOREACH_46(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46)
#define FOREACH_47(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47)
#define FOREACH_48(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48)
#define FOREACH_49(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49)
#define FOREACH_50(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50)
#define FOREACH_51(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51)
#define FOREACH_52(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52)
#define FOREACH_53(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53)
#define FOREACH_54(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54)
#define FOREACH_55(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55)
#define FOREACH_56(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56)
#define FOREACH_57(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57)
#define FOREACH_58(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58)
#define FOREACH_59(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59)
#define FOREACH_60(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60)
#define FOREACH_61(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61)
#define FOREACH_62(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62)
#define FOREACH_63(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63)
#define FOREACH_64(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64)
#define FOREACH_65(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65)
#define FOREACH_66(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66)
#define FOREACH_67(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67)
#define FOREACH_68(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68)
#define FOREACH_69(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69)
#define FOREACH_70(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70)
#define FOREACH_71(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71)
#define FOREACH_72(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72)
#define FOREACH_73(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73)
#define FOREACH_74(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74)
#define FOREACH_75(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75)
#define FOREACH_76(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76)
#define FOREACH_77(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77)
#define FOREACH_78(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78)
#define FOREACH_79(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79)
#define FOREACH_80(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80)
#define FOREACH_81(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81)
#define FOREACH_82(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82)
#define FOREACH_83(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83)
#define FOREACH_84(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84)
#define FOREACH_85(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85)
#define FOREACH_86(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86)
#define FOREACH_87(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87)
#define FOREACH_88(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88)
#define FOREACH_89(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89)
#define FOREACH_90(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90)
#define FOREACH_91(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91)
#define FOREACH_92(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92)
#define FOREACH_93(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93)
#define FOREACH_94(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94)
#define FOREACH_95(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95)
#define FOREACH_96(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96)
#define FOREACH_97(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97)
#define FOREACH_98(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98)
#define FOREACH_99(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99)
#define FOREACH_100(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100)
#define FOREACH_101(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101)
#define FOREACH_102(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102)
#define FOREACH_103(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103)
#define FOREACH_104(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104)
#define FOREACH_105(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105)
#define FOREACH_106(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106)
#define FOREACH_107(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107)
#define FOREACH_108(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108)
#define FOREACH_109(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109)
#define FOREACH_110(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110)
#define FOREACH_111(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111)
#define FOREACH_112(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112)
#define FOREACH_113(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113)
#define FOREACH_114(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114)
#define FOREACH_115(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115)
#define FOREACH_116(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116)
#define FOREACH_117(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117)
#define FOREACH_118(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118)
#define FOREACH_119(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119)
#define FOREACH_120(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120)
#define FOREACH_121(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121)
#define FOREACH_122(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122)
#define FOREACH_123(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122), func(_123)
#define FOREACH_124(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122), func(_123), func(_124)
#define FOREACH_125(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122), func(_123), func(_124), func(_125)
#define FOREACH_126(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122), func(_123), func(_124), func(_125), func(_126)
#define FOREACH_127(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122), func(_123), func(_124), func(_125), func(_126), func(_127)
#define FOREACH_128(func, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, _128) func(_1), func(_2), func(_3), func(_4), func(_5), func(_6), func(_7), func(_8), func(_9), func(_10), func(_11), func(_12), func(_13), func(_14), func(_15), func(_16), func(_17), func(_18), func(_19), func(_20), func(_21), func(_22), func(_23), func(_24), func(_25), func(_26), func(_27), func(_28), func(_29), func(_30), func(_31), func(_32), func(_33), func(_34), func(_35), func(_36), func(_37), func(_38), func(_39), func(_40), func(_41), func(_42), func(_43), func(_44), func(_45), func(_46), func(_47), func(_48), func(_49), func(_50), func(_51), func(_52), func(_53), func(_54), func(_55), func(_56), func(_57), func(_58), func(_59), func(_60), func(_61), func(_62), func(_63), func(_64), func(_65), func(_66), func(_67), func(_68), func(_69), func(_70), func(_71), func(_72), func(_73), func(_74), func(_75), func(_76), func(_77), func(_78), func(_79), func(_80), func(_81), func(_82), func(_83), func(_84), func(_85), func(_86), func(_87), func(_88), func(_89), func(_90), func(_91), func(_92), func(_93), func(_94), func(_95), func(_96), func(_97), func(_98), func(_99), func(_100), func(_101), func(_102), func(_103), func(_104), func(_105), func(_106), func(_107), func(_108), func(_109), func(_110), func(_111), func(_112), func(_113), func(_114), func(_115), func(_116), func(_117), func(_118), func(_119), func(_120), func(_121), func(_122), func(_123), func(_124), func(_125), func(_126), func(_127), func(_128)

#define FOREACH_N(n,m,...) FOREACH_##n(m,__VA_ARGS__)
#define FOREACH_DO(n, m, ... ) FOREACH_N(n, m, __VA_ARGS__)
#define FOREACH(m, ...) FOREACH_DO(VA_COUNT(__VA_ARGS__), m, __VA_ARGS__)

void printf_impl(size_t n, TypeInfo *args, bool isf);
// void printf_impl(char *fmt, size_t n, const TypeInfo *args);
void writef_impl(char *fmt, size_t n, const TypeInfo *args, bool isf);

// Better Printing API
#define my_print(...) \
    do { \
        TypeInfo _args[] = { FOREACH(TypedArg, __VA_ARGS__) }; \
        printf_impl(sizeof(_args)/sizeof(_args[0]), _args, false); \
    } while(0)

#define my_printf(...) \
    do { \
        TypeInfo _args[] = { FOREACH(TypedArg, __VA_ARGS__) }; \
        printf_impl(sizeof(_args)/sizeof(_args[0]), _args, true); \
    } while(0)

#define my_println(...) my_print(__VA_ARGS__, "\n")
#define my_printfln(...) my_printf(__VA_ARGS__, "\n")

// @Incomplete I want thjp_is _Generic write(<type>) and firing off to write_string, write_file, write_output
#define write_string(dst, ...) \
    do { \
        TypeInfo _args[] = { FOREACH(TypedArg, __VA_ARGS__) }; \
        writef_string_impl(dst, sizeof(_args)/sizeof(_args[0]), _args, false); \
    } while(0)

#define writef_string(dst, ...) \
    do { \
        TypeInfo _args[] = { FOREACH(TypedArg, __VA_ARGS__) }; \
        writef_string_impl(dst, sizeof(_args)/sizeof(_args[0]), _args, true); \
    } while(0)

// IO 
#define IO_FILE    1
#define IO_DIR     2
#define IO_SYMLINK 3

typedef struct {
    bool use_relative; // Don't include PWD if searching inside it
} readdir_opts;

typedef dynarray(string) StringList;
StringList read_dir_cstr(string path, readdir_opts opts); // @Memory
StringList read_dir_string(string path, readdir_opts opts); // @Memory

#define read_dir(path, ...) _Generic((path), \
        char* read_dir_cstr, \
        string: read_dir_string \
        )(path, #__VA_ARGS__)


string read_entire_file(string filepath); // @Memory


// 
// BEGIN IMPLEMENTATION
//

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

//
// libc string.h replacements
//

// Returns string with len **NOT** including null terminator
string cstrlen(char *source)
{
    string result = { .data = source };
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail

    for (result.len = 0; source[result.len] != '\0' && result.len < SIZE_MAX; result.len++);
    return result;
}

bool jp_isspace(char c)
{
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail
    return c == ' '  || 
           c == '\t' || 
           c == '\n' || 
           c == '\r' ||
           c == '\v' ||
           c == '\f';
}

string string_copy(string *dest, const string source) 
{
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail
    if (!dest) {
        *dest = (string){
            ._owner = true,
            .len = source.len + 1,
        };
    }
    assert(dest->_owner);
    // extra 1 for null terminator though we do not include it in length
    // safety measure to prevent us having issues with other C code (maybe disable later?)
    if (source.len + 1 + dest->len > dest->_cap) {
        printf("DEBUG: allocating memory for string!");
        dest->data = (char *)realloc(dest->data, (dest->len + 1 + source.len) * sizeof(char));
        assert(dest->data && "Failed to allocate memory for string");
        dest->_cap = dest->len + source.len;
    }
    for (size_t i = 0; source.len > i; i++) dest->data[dest->len + i] = source.data[i];
    dest->len += source.len;
    dest->data[dest->len + 1] = '\0';
    return *dest;
}


//
// string Implementation start
//
bool _string_cmp_unsafe(const char *haystack, const string needle); // internal only

// String conditionals
inline bool string_cmp(const string haystack, const string needle)
{ 
    if (haystack.len < needle.len) return false;
    return _string_cmp_unsafe(haystack.data, needle);
}

// Internal only!!
// Does not bound check haystack for null terminator
// Assumes you have checked haystack >= needle.len
bool _string_cmp_unsafe(const char *haystack, const string needle)
{
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail
    for (size_t i = 0; i < needle.len; i++) {
        if (haystack[i] != needle.data[i]) return false;
    }
    return true;
}

int string_indexof(const string haystack, const string needle)
{
    if (haystack.len < needle.len) return -1;
    for (size_t i = 0; i < haystack.len - needle.len; i++) {
        if (haystack.data[i] != *needle.data) continue;
        if (_string_cmp_unsafe(&haystack.data[i], needle)) return i;
    }
    return -1;
}

inline bool string_contains(const string haystack, const string needle)
{
    return string_indexof(haystack, needle) >= 0;
}

string string_trim_whitespace(string source)
{
    if (source.len == 0) return source;

    size_t trimstart, trimend;
    for (trimstart = 0; trimstart < source.len && jp_isspace(source.data[trimstart]); trimstart++);
    for (trimend = source.len; trimend > trimstart && jp_isspace(source.data[trimend-1]); trimend--);
    
    return (string){
        .data = &source.data[trimstart],
        .len = trimend - trimstart,
    };
}

// @TODO Wrap this in generic for 2nd arg string/char*
string string_trim_prefix(string source, char *prefix)
{
    string needle = cstrlen(prefix);
    if (!string_cmp(source, needle)) return source;

    return (string){
        .data = &source.data[needle.len],
        .len = source.len - needle.len,
    };
}

string string_trim_suffix(string source, char *suffix)
{
    string needle = cstrlen(suffix);
    if (source.len < needle.len) return source;

    if (!_string_cmp_unsafe(&source.data[source.len-needle.len], needle)) return source;
    source.len = source.len - needle.len - 1;
    return source;
}

string string_trim_before(string source, char *target)
{
    string needle = cstrlen(target);
    size_t index = string_indexof(source, needle);
    return (string){
        .data = &source.data[index],
        .len = source.len - index,
    };
}
string string_trim_after(string source, char *target)
{
    string needle = cstrlen(target);
    size_t index = string_indexof(source, needle);
    source.len = index;
    return source;
}

// String manipulation (owning)
// Dest is pointer to reduce noise calling API
// pass NULL to allocate new string
// pass pointer to string if wanting to append
string string_write(string *dest, const string source) 
{
    if (dest == NULL) { *dest = (string){ ._owner = true }; }
    assert(dest->_owner); 
    if (dest->len + source.len >= dest->_cap) {
        if (dest->_cap == 0) dest->_cap = source.len;
        dest->_cap *= 2;
        dest->data = (char *)realloc(dest->data, sizeof(char)*dest->_cap);
    }
    for (size_t i = 0; i < source.len; i++) dest->data[dest->len+i] = source.data[i];
    dest->data[dest->len+1] = '\0'; // In case we ever deal with C apis...
    dest->len += source.len;
    return *dest;
}

/*
void sb_appendf(StringBuilder *sb, const char *fmt, ...)
{
    char *cursor = fmt;
    for (; *cursor; cursor++) {
        if (*cursor == '%' && *cursor+1 != '%') 

    }
    if (arr.len >= arr.cap) { \
        if (arr.cap == 0) arr.cap = 256; \
        else arr.cap *= 2; \
            arr.data = realloc(arr.data, arr.cap*sizeof(*arr.data)); \
                assert(arr.data && "We requested more memory but the computer said \"No\"!"); \
    } \
    arr.data[arr.len++] = item; \

   (void)sb;
   (void)fmt;
}
*/

// void sb_appends(string *sb, string slice)
// {
//     assert(sb->_owner);
//     if (sb->len >= sb->_cap) {
//         if (sb->_cap == 0) sb->_cap = slice.len;
//         sb->_cap *= 2;
//         sb->data = realloc(sb->data, sb->_cap*sizeof(char));
//         assert(sb->data && "We requested more memory but the computer said \"No\"!");
//     }
//     cstrcpy(&sb->data[sb->len], slice.data, slice.len);
// }

// @Warning manipulates source string!!
string string_split_iter(string *source, string delim)
{
    assert(delim.len > 0 && "delimiter provided jp_is empty!");
    if (source->len == 0) return (string){0};

     for(size_t index = 0; index < source->len; index++) {
        if (_string_cmp_unsafe(&source->data[index], delim)) {
            index++;
            string before = {
                .data = source->data,
                .len =  index - 1,
                .next = true,
            };
            source->data = &source->data[index];
            source->len = source->len - index;
            return before;
        }
    }
    return (string){0};
}

// Better Printing!
// @Incomplete - basic implementation only atm, we should be writing to buffers to be better practice
string pct = {
    .data = "%",
    .len  = 1,
};

/*
 * Syntax to add (after % for customising)
    -  = Left-align
    +  = Always show sign (ignored for strings)
   ' ' = Space if positive (ignored for strings)
    0  = Zero-pad (ignored for strings)
    #  = Alternate form
    U  = Uppercase (ignored for non-strings)
    p  = pointer
    width:
        Minimum field width.
            printf("%5d", 42);   // "   42"
            printf("%-5d", 42);  // "42   "
        Dynamic width:
            printf("%*d", 5, 42);
    precision:
        For integers
            Minimum number of digits:
            printf("%.5d", 42);  // 00042
        For floats
            Digits after decimal:
            printf("%.3f", 3.14159); // 3.142
        For strings
            Maximum characters:
            printf("%.4s", "abcdef"); // abcd
        Dynamic precision:
            printf("%.*f", 2, 3.14159);
    .  = start size logic
        *  = dynamic width
        \d = (digit) width
*/

#if defined(_MSC_VER)
#  define DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#  define DLL_IMPORT __attribute__((dllimport))
#else
#  define DLL_IMPORT
#endif

DLL_IMPORT void* __stdcall GetStdHandle(unsigned long);
DLL_IMPORT int   __stdcall WriteFile(
    void* h,
    const void* buf,
    unsigned long len,
    unsigned long* written,
    void* overlapped
);


#ifdef _WIN32
#define STDIN  0
#define STDOUT 0
#define STDERR 0
#define WIN_STDIN  ((unsigned long)-10)
#define WIN_STDOUT ((unsigned long)-11)
#define WIN_STDERR ((unsigned long)-12)
void *stdio_handles[3] = {0}; // index into to get handle then cast depending on system
__attribute__((dllimport)) void* __stdcall GetStdHandle(unsigned long);
__attribute__((dllimport)) int   __stdcall WriteFile(void *, const void*, unsigned long, unsigned long*, void*);
#elif __linux__
#endif // _WIN32

size_t __write(void *dest, char *data, size_t len)
{
#ifdef _WIN32
    unsigned long written;
    WriteFile(dest, data, len, &written, NULL);
#else
#error Unsupported OS TODO!!!
#endif
    return (size_t)written;
}

string __write_string(void *dest, string *source)
{ 
    __write(dest, source->data, source->len);
    source->len = 0;
    return *source;
}

size_t __print(char *data, size_t len) {
#ifdef _WIN32
    if (!stdio_handles[STDOUT]) {
        stdio_handles[STDOUT] = GetStdHandle(WIN_STDOUT);
    }
#endif
    return __write(stdio_handles[STDOUT], data, len);
}

//#define ALWAYS_SHOW_SIGN 1 << 0
void format_u64(string *buf, unsigned long long value, u8 opts) 
{
    (void)opts;
    char reversed[20];   // max for 64-bit decimal
    int n = 0;

    // @Speed can we improve this?
    do {
        reversed[n++] = '0' + value % 10;
        value /= 10;
    } while (value);
    while (n--) {
        buf->data[buf->len++] = reversed[n];
    }
}

void format_s64(string *buf, long long value, u8 opts)
{ 
    if (value <  0) {
        buf->data[buf->len++] = '-';
        value = -value;
    } 
    format_u64(buf, (unsigned long long)value, opts);
}

#define MAX_DBL_DP 17
// @Incomplete this is only the case on x64 on 128 bit we need to do 36 for long double
#define MAX_LDBL_DP 21

void format_f64(string *buf, double value, int precision)
{
    if (value <  0) {
        buf->data[buf->len++] = '-';
        value = -value;
    } 

    int digit;
    long long intpart = (long long)value;
    double frac       = value - (double)intpart;
    format_u64(buf, intpart, 0);
    if (precision < 0) precision = MAX_DBL_DP;
    buf->data[buf->len] = '.'; 
    size_t lastnon0 = 1;
    for (size_t i = 0; i < (size_t)precision; i++) { 
        frac *= 10.0;
        if (frac >= 1) lastnon0 = i + 3; // +3 so we can -= 1 after and it will remove the '.' if no decimal result
        digit = (int)frac;
        buf->data[buf->len+1+i] = '0' + digit;
        frac -= digit;
    }
    buf->len += lastnon0 - 1;
}

void format_ldbl(string *buf, long double value, int precision)
{
    if (value <  0) {
        buf->data[buf->len++] = '-';
        value = -value;
    } 

    int digit;
    long long intpart = (long long)value;
    long double frac  = value - (long double)intpart;
    format_u64(buf, intpart, 0);
    if (precision < 0) precision = MAX_DBL_DP;
    buf->data[buf->len] = '.'; 
    size_t lastnon0 = 1;
    for (size_t i = 0; i < (size_t)precision; i++) { 
        frac *= 10.0;
        if (frac >= 1) lastnon0 = i + 3; // +3 so we can -= 1 after and it will remove the '.' if no decimal result
        digit = (int)frac;
        buf->data[buf->len+1+i] = '0' + digit;
        frac -= digit;
    }
    buf->len += lastnon0 - 1;
}

string boolstr[] = {
    {.data = "false", .len = 5, ._cap = 5},
    {.data = "true" , .len = 4, ._cap = 4},
}; 

size_t write_string_upto_cap(string *buf, string source)
{
    size_t advanceby;
    // copy into buffer until either we finish the string or we fill up the buffer
    for (advanceby = 0;buf->len < buf->_cap && source.len > 0; advanceby++) {
        buf->data[buf->len++] = source.data[advanceby];
        source.len--;
    }
    return advanceby;
}


// @Incomplete I want to replace char * here with string and wrap any char* in cstrlen at time of call
bool format_string_arg_into_buffer_iter(string *buf, size_t *argc, TypeInfo **args, char *source, bool isf)
{
    if (!source) {
        write_string_upto_cap(buf, (string){.data="(null)", .len=6});
        return false;
    }
    string working, result, line, subline;
    working = cstrlen(source);
    u8 opts = 0;
    size_t advanceby;
    if (!isf) {
        string newline = {.data = "\n", .len = 1, ._cap = 1 };
        bool isnewline = string_cmp(working, newline);
        advanceby = write_string_upto_cap(buf, working);
        (*args)[0].s = &(*args)[0].s[advanceby]; // no need to preserve the '%' we are not formatting
        if (buf->len + (isnewline ? 1 : 0) > buf->_cap) return true;
        if (!isnewline) buf->data[buf->len++] = ' ';
        return false;
    }
    while ((line = string_split_iter(&working, pct)).next) {
        advanceby = write_string_upto_cap(buf, line);

        // advance raw source string in case we run out of space and are called again
        // this gives the caller the same view as 'working' (and thus our next call if any)

        if (line.len > advanceby) {
            // We filled up the buffer before we finished writing the string
            return true;
        }

        // we had enough space to write the string UP TO the % we are formatting...

        // check we can fit at least a double...
        if (buf->len + 41 >= buf->_cap) return true;
        // saves doing an if on each number branch...

        if (working.len > 0) {
            // check if we had an escaped % OR no more args (print the %)
            if (working.data[0] == '%' || *argc == 1) {
                working.data++;
                working.len--;
                buf->data[buf->len++] = '%';
                continue;
            }
            // handle_printf_format_opts()
        }

        TypeInfo next = (*args)[1];
        // We've already checked we know we have at least 1 or more arg so have to format
        switch(next.tag) {
            case T_CHAR:
                buf->data[buf->len++] = (char)next.i;
                break;
            case T_SCHAR:
            case T_SHORT:
            case T_INT:  
            case T_LONG:
            case T_LLONG:
                format_s64(buf, next.i, opts);
                break;
            case T_UCHAR:
            case T_USHORT:
            case T_UINT:  
            case T_ULONG:
            case T_ULLONG: 
                format_u64(buf, next.u, opts);
                break;
            case T_FLOAT:
            case T_DOUBLE:
                format_f64(buf, next.d, opts);
                break;
            case T_LDOUBLE:
                format_ldbl(buf, next.ld, opts);
                break;
            case T_BOOL:  
                // printf("DEBUG: handling formatted T_BOOL\n");
                result = boolstr[next.b ? 1 : 0];
                for (size_t i = 0; i < result.len; i++) {
                    buf->data[buf->len++] = result.data[i];
                }
                break;
            case T_STR:
                if (!next.s) {
                    write_string_upto_cap(buf, (string){.data="(null)", .len=6});
                    break;
                }
                subline = cstrlen(next.s);
                // @CopyPasta from a few lines above
                advanceby = write_string_upto_cap(buf, subline);
                // advance raw source string in case we run out of space and are called again
                // this gives the caller the same view as 'working' (and thus our next call if any)
                (*args)[1].s = &(*args)[1].s[advanceby]; 

                if (subline.len > advanceby) return true;
                break;

            default:
                // Once everything is implemented this will only fire for
                // custom types... So we can handle custom types here?
                panic("Unhandled type!");
        }
        // Replace the arg we consumed with the string we're formatting
        (*args)[1] = (*args)[0];
        // Increment args to remove consumed from total
        (*args) = &(*args)[1];
        (*argc)--;
    }
    advanceby = write_string_upto_cap(buf, working);
    // advance raw source string in case we run out of space and are called again
    // this gives the caller the same view as 'working' (and thus our next call if any)
    (*args)[0].s = &(*args)[0].s[advanceby]; // no need to preserve the '%' we have handled them all in the loop above

    if (working.len > advanceby) return true;
    return false;

}

// TODO
// - Remaining basic types formatting
// - Decide handling of each type for printf formatting semantics and implement
// - Optimise handling and processing...
// - Figure out how to do custom end user types...
// - Tests for each of the features 
//
// IMPORTANT! Updates *args pointing to next arg on each loop,
//            if you need to keep access to the start of the list 
//            copy the address!
//
// Returns true if more args to process
bool format_args_into_iter(string *buf, size_t *argc, TypeInfo **args, bool isf)
{
    if (*argc == 0) return false; // nothing to do 
    buf->next = true;
    TypeInfo current;
    string result;
    while (*argc > 0) {
        current = *args[0];
        // Check we can fit the largest possible numerical type when represented as string
        if (buf->len + 41 >= buf->_cap) return true;
        // anything else (bool) will fit in this, strings are handled separately...

        switch(current.tag) {
            // Numbers are all handled the same, could probably do this during the tag and just do l and d (long and double all types are representable by)
            // But then it'd just push this switch statement into another function, so meh.
            // @Incomplete @cleanup we wouldn't have to switch twice we could just if T_STR do string else do number which does this!!!
            //
            // Numbers not in format string have a space (' ') appended
            case T_CHAR:
                buf->data[buf->len++] = (char)current.i;
                buf->data[buf->len++] = ' ';
                break;
            case T_SCHAR:
            case T_SHORT:
            case T_INT:  
            case T_LONG:
            case T_LLONG:
                format_s64(buf, current.i, 0);
                buf->data[buf->len++] = ' ';
                break;
            case T_UCHAR:
            case T_USHORT:
            case T_UINT:  
            case T_ULONG:
            case T_ULLONG:
                format_u64(buf, current.u, 0);
                buf->data[buf->len++] = ' ';
                break;
            case T_FLOAT:
            case T_DOUBLE:
                format_f64(buf, current.d, -1);
                buf->data[buf->len++] = ' ';
                break;
            case T_LDOUBLE:
                format_ldbl(buf, current.ld, -1);
                break;
            case T_BOOL:  
                result = boolstr[current.b ? 1 : 0];
                for (size_t i = 0; i < result.len; i++) {
                    buf->data[buf->len++] = result.data[i];
                }
                buf->data[buf->len++] = ' ';
                break;
            case T_STR:  
                if (format_string_arg_into_buffer_iter(buf, argc, args, current.s, isf)) return true;
                break;
            default:
                // Once everything is implemented this will only fire for
                // custom types... So we can handle custom types here?
                panic("Unhandled type!");
        }
        (*args) = &(*args)[1]; // Update args base address
        (*argc)--;
    }
    return false;
}

#define PRINT_BUF_SIZE 4096
void printf_impl(size_t argc, TypeInfo *args, bool isf) 
{
    if (argc == 0) return; // nothing to do 
#ifdef _WIN32
    if (!stdio_handles[STDOUT]) {
        stdio_handles[STDOUT] = GetStdHandle(WIN_STDOUT);
    }
#endif
    // shortcut logic if we have 1 string arg to print
    // send it straight to write
    if (argc == 1 && args[0].tag == T_STR) {
        my_println("we're skipping the format logic");
        string towrite = cstrlen(args[0].s);
        __write_string(stdio_handles[STDOUT], &towrite);
        return;
    }

    char _buf[PRINT_BUF_SIZE];
    string buf = {
        ._owner = true,
        .data = _buf,
        .len = 0,
        ._cap = PRINT_BUF_SIZE,
    };
    while (format_args_into_iter(&buf, &argc, &args, isf)) {
        __write_string(stdio_handles[STDOUT], &buf);
    }
    if (buf.len > 0) __write_string(stdio_handles[STDOUT], &buf);
}

// TODO - ability to write raw bytes not converted to human format

// like sprintf except we know the types and can grow the buffer
void writef_string_impl(string *dest, size_t argc, TypeInfo *args, bool isf)
{
    assert("Passed NULL to write_string" && dest);
    if (!dest->data) {
        dest->_owner = true;
        dest->_cap   = 256;
        dest->data   = (char *)malloc(dest->_cap * sizeof(char));
    }
    assert(dest->_owner); // @Incomplete this lib should make a copy of and make an owner
    if (argc == 1 && args[0].tag == T_STR) {
        my_println("we're skipping the format logic");
        string towrite = cstrlen(args[0].s);
        string_copy(dest, towrite);
        return;
    }
    while (format_args_into_iter(dest, &argc, &args, isf)) {
        dest->data = (char*)realloc(dest->data, dest->_cap * 2 * sizeof(char));
        assert("Failed to reallocate string buffer!" && dest->data);
        dest->_cap = dest->_cap * 2;
    }
}

// IO Implementation

/* -- Prefix macro 
 * Commented and removed prefix calls but keeping in case we need to bring back...
 * Pollutes codebase for benefit of user (mostly me no doubt) 
 * and doubt we'll be brought into any new large codebases...
#ifndef CONCAT2
#define CONCAT2(a, b, c) a##b##c
#endif // CONCAT2

// Let user of lib specify optional prefix
#ifdef BASIC_PREFIX

// another of our headers was used 
// but maybe user didn't give both a prefix...
// define using prefix here as we have been told to
#undef CONCAT
#define CONCAT(a, b) JP_CONCAT2(a, _, b)

#else  // BASIC_PREFIX 
#define BASIC_PREFIX

// another of our headers was used 
// but maybe user didn't give both a prefix...
// define without prefix here as we have been told to
#undef CONCAT
#define CONCAT(a, b) b

#endif // BASIC_PREFIX

#ifndef PREFIX
#define PREFIX(x) JP_CONCAT(JP_BASIC_PREFIX, x)
#endif // PREFIX
*/
#endif // _BASIC_H
