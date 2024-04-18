#ifndef TEST_H_
#define TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdbool.h>

static void _enable_ansi_sequences() {
    static bool enabled = false;
    if (enabled)
        return;

    HANDLE handleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode;
    GetConsoleMode( handleOut , &consoleMode);
    consoleMode |= 0x0004;
    consoleMode |= 0x0008;            
    SetConsoleMode( handleOut , consoleMode );

    enabled = true;
}

#define FILENAME ( (strrchr(__FILE__, '\\')) ? (strrchr(__FILE__, '\\') + 1) : ( strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : (__FILE__) ) )

#define TEST_LOG(fmt, ...) do { _enable_ansi_sequences(); printf("\e[1;97m-- %s:\e[0m " fmt "\e[0m\n", FILENAME, ##__VA_ARGS__); } while(0)
#define TEST_FAIL(fmt, ...) do { _enable_ansi_sequences(); printf("\e[1;97m-- %s \e[1;91mfailed\e[1;97m:\e[0m " fmt "\e[0m (line %d)\n", FILENAME, ##__VA_ARGS__, __LINE__); abort(); } while(0)
#define TEST_SUCCEED() do { _enable_ansi_sequences(); printf("\e[1;97m-- %s \e[1;92msucceded\e[0m\n", FILENAME); exit(0); } while(0)
#define TEST_IGNORE() do { _enable_ansi_sequences(); printf("\e[1;97m-- %s \e[0mignored\e[0m\n", FILENAME); exit(0); } while(0)

#define TEST_ASSERT_LOG(a, fmt, ...) do { if (!(a)) TEST_FAIL(fmt, ##__VA_ARGS__); } while(0)
#define TEST_ASSERT_EQ(a, b) do { if ((a) != (b)) TEST_FAIL(#a " != " #b); } while(0)
#define TEST_ASSERT_LEQ(a, b) do { if ((a) > (b)) TEST_FAIL(#a " > " #b); } while(0)
#define TEST_ASSERT_GEQ(a, b) do { if ((a) < (b)) TEST_FAIL(#a " < " #b); } while(0)
#define TEST_ASSERT_NEQ(a, b) do { if ((a) == (b)) TEST_FAIL(#a " == " #b); } while(0)
#define TEST_ASSERT_STREQ(a, b) do { if (strcmp((a), (b)) != 0) TEST_FAIL(#a " != " #b); } while(0)

#endif
