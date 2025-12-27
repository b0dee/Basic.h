// jp_basic.h
// All your basic needs imported at once
// * Assertions (imported from assert.h)
// * Type shorthands (imported from stdint.h and typedef'd)
// * File IO @Incomplete (currently stdio.h)
// * Allocators @Incomplete (currently malloc.h)
// * Go-like strings @Incomplete
// * String formatting @Incomplete

// Key Info:
// Strings are slices until .owner property is non-null
// _Always_ call string_free - if it is not an owner it will just return
// Anything tagged @Memory allocates/manipulates owning buffer

#ifndef _JP_BASIC_H
#define _JP_BASIC_H

#include <assert.h>
#define panic(msg) assert(0 && msg)
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
typedef struct {
    char  *_owner;  // internal
    char  *data;
    size_t len;
    size_t _cap;    // internal
} string;

// libc string.h replacements
string cstrlen(char source[static 1]);  // Creates slice over source
string cstrcpy(char dest[static 1], const char source[static 1], size_t size); // If dest is null @Memory
bool   cstrcmp(const char a[static 1], const char b[static 1]);
int    cstrcontains(char haystack[static 1], char needle[static 1]);
bool   jp_isspace(char c);

// String functions
string string_from(char src[static 1]);

// String searching
bool   string_cmp(string a, const string b);
int    string_indexof(const string haystack, const string needle);
bool   string_contains(const string haystack, const string needle);

// String manipulation
// All pass by value and return a new string (slice)
string string_trim_whitespace(string source);
string string_trim_prefix(string source, char prefix[static 1]);
string string_trim_suffix(string source, char suffix[static 1]);
string string_trim_before(string source, char target[static 1]);
string string_trim_after(string source, char target[static 1]);
// As a result they can be chained i.e. 
// string after_arrow_not_space = string_trim_whitespace(string_trim_prefix(source, "->"));

// Except for this one, which works sort of like Go's SplitSeq
string string_split_iter(string source[static 1], char raw[static 1]);
// Which allows for the following style of code:
// (in the below example, source is a preexisting string slice)
// ```
// string line;
// while ((line = string_split_seq(&source, "\n")).next) {
//     printf("Line: %.*s\n", (int)line.len, line.data)
// }
// ```

// StringBuilder functions
size_t        sb_write(string *sb, char *text);
void          sb_appendf(string *sb, char *fmt, ...);
void          sb_appends(string *sb, string slice);


// IO 
#define IO_FILE    1
#define IO_DIR     2
#define IO_SYMLINK 3

typedef struct {
    bool use_relative; // Don't include PWD if searching inside it
} readdir_opts;

dynarray(string) read_dir_cstr(string path, readdir_opts opts); // @Memory
dynarray(string) read_dir_string(string path, readdir_opts opts); // @Memory

#define read_dir(path, ...) _Generic((path), \
        char* read_dir_cstr(path); \
        string: read_dir_string(path, #__VA_ARGS__); 


string read_entire_file(string filepath); // @Memory

#endif // _JP_BASIC_H

// ----------------------------------
#ifdef JP_BASIC_IMPL
// 
// BEGIN IMPLEMENTATION
//

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

//
// libc string.h replacements
//

string cstrlen(char source[static 1])
{
    string result = { .data = source };
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail

    for (result.len = 0; source[result.len] != '\0' && result.len < SIZE_MAX; result.len++);
    return result;
}

// We assume you have reserved at least <size> bytes in dest
// Or are passing dest as NULL to have the function allocate for you @Memory
string cstrcpy(char dest[static 1], const char source[static 1], size_t size) 
{
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail
    string copied;
    if (!dest) {
        copied.data = (char*)malloc(size * sizeof(char));
        assert(copied.data && "Failed to allocate memory for string");
        copied._owner = copied.data;
    }
    for (copied.len = 0; source[copied.len] != '\0' && copied.len <= size-1; copied.len++) dest[copied.len] = source[copied.len];
    dest[copied.len] = '\0';
    return copied;
}

bool cstrcmp(const char a[static 1], const char b[static 1])
{
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail
    size_t i;
    for (i = 0; a[i] != '\0' && b[i] != '\0'; i++) {
        if (a[i] != b[i]) return false;
    }
    return true; 
}


int cstrcontains(char haystack[static 1], char needle[static 1])
{
    string n = cstrlen(needle);
    string h = cstrlen(haystack);
    if (n.len > h.len) return -1;
    for (int i = 0; haystack[n.len + i] == '\0'; i++) {
        if (haystack[i] != *needle) continue;
        if (cstrcmp(&haystack[i], needle)) return i;
    }
    return -1;
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

//
// string Implementation start
//

string string_from(char cstr[static 1]) 
{
    return cstrlen(cstr); 
}

// String conditionals
bool string_cmp(const string source, const string test)
{ 
    if (source.len < test.len) return false;
    // @Incomplete - compiler does not optimise this out 
    // Rewrite using SIMD instructions
    // These should be gaurded and default to this if not avail
    size_t i;
    for (i = 0; i < test.len; i++) {
        if (source.data[i] != test.data[i]) return false;
    }
    return true;
}

int string_indexof(const string haystack, const string needle)
{
    if (haystack.len < needle.len) return -1;
    for (size_t i = 0; i < haystack.len - needle.len; i++) {
        if (haystack.data[i] != *needle.data) continue;
        if (cstrcmp(&haystack.data[i], needle.data)) return i;
    }
    return -1;
}

bool string_contains(const string haystack, const string needle)
{
    return string_indexof(haystack, needle) >= 0;
}

// Slice manipulation (not owner)
string string_trim_whitespace(string source)
{
    size_t trimstart, trimend;
    for (trimstart = 0; trimstart < source.len && jp_isspace(source.data[trimstart]); trimstart++);
    for (trimend = source.len; trimend > trimstart && jp_isspace(source.data[trimend-1]); trimend--);
    
    return (string){
        .data = &source.data[trimstart],
        .len = trimend - trimstart,
    };
}

string string_trim_prefix(string source, char prefix[static 1])
{
    string needle = string_from(prefix);
    if (!string_cmp(source, needle)) return source;

    return (string){
        .data = &source.data[needle.len],
        .len = source.len - needle.len,
    };
}

string string_trim_suffix(string source, char suffix[static 1])
{
    string needle = string_from(suffix);
    if (source.len < needle.len) return source;

    string test   = {
        .data = &source.data[source.len - needle.len],
        .len = needle.len,
    };
    if (!string_cmp(test, needle)) return source;

    source.len = source.len - needle.len - 1;
    return source;
}

string string_split_iter(string source[static 1], char raw[static 1])
{
    if (source->len == 0) return (string){0};
    string delim = string_from(raw);

    // size_t maxlen = source->len - delim.len;
     for(size_t index = 0; index < source->len; index++) {
        if (cstrcmp(&source->data[index], delim.data)) {
            index++;
            string before = {
                .data = source->data,
                .len =  index - 1,
            };
            source->data = &source->data[index];
            source->len = source->len - index;
            return before;
        }
    }
    return (string){0};
}

string string_trim_before(string source, char target[static 1])
{
    string needle = string_from(target);
    size_t index = string_indexof(source, needle);
    return (string){
        .data = &source.data[index],
        .len = source.len - index,
    };
}
string string_trim_after(string source, char target[static 1])
{
    string needle = string_from(target);
    size_t index = string_indexof(source, needle);
    source.len = index;
    return source;
}

// String manipulation (owning)
size_t string_write(string *sb, char *text) 
{
    assert(sb->_owner);
    string t = cstrlen(text);
    if (sb->len + t.len > sb->_cap) {
        if (sb->_cap == 0) sb->_cap = t.len;
        sb->_cap *= 2;
        sb->data = realloc(sb->data, sizeof(char)*sb->_cap);
    }
    size_t copied = cstrcpy(&sb->data[sb->len++], text, t.len).len;
    assert(copied == t.len && "Failure in cstrcpy");
    sb->len += t.len;
    return copied;
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

void sb_appends(string *sb, string slice)
{
    assert(sb->_owner);
    if (sb->len >= sb->_cap) {
        if (sb->_cap == 0) sb->_cap = slice.len;
        sb->_cap *= 2;
        sb->data = realloc(sb->data, sb->_cap*sizeof(char));
        assert(sb->data && "We requested more memory but the computer said \"No\"!");
    }
    cstrcpy(&sb->data[sb->len], slice.data, slice.len);
}

// IO Implementation

#undef    JP_BASIC_IMPL
#endif // JP_BASIC_IMPL


/* -- Prefix macro 
 * Commented and removed prefix calls but keeping in case we need to bring back...
 * Pollutes codebase for benefit of user (mostly me no doubt) 
 * and doubt we'll be brought into any new large codebases...
#ifndef JP_CONCAT2
#define JP_CONCAT2(a, b, c) a##b##c
#endif // JP_CONCAT2

// Let user of lib specify optional prefix
#ifdef JP_BASIC_PREFIX

// another of our headers was used 
// but maybe user didn't give both a prefix...
// define using prefix here as we have been told to
#undef JP_CONCAT
#define JP_CONCAT(a, b) JP_CONCAT2(a, _, b)

#else  // JP_BASIC_PREFIX 
#define JP_BASIC_PREFIX

// another of our headers was used 
// but maybe user didn't give both a prefix...
// define without prefix here as we have been told to
#undef JP_CONCAT
#define JP_CONCAT(a, b) b

#endif // JP_BASIC_PREFIX

#ifndef JP_PREFIX
#define JP_PREFIX(x) JP_CONCAT(JP_BASIC_PREFIX, x)
#endif // JP_PREFIX
*/

