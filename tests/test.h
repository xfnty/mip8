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

#define FAIL(fmt, ...) do { _enable_ansi_sequences(); printf("\e[1;97m-- %s \e[1;91mfailed\e[1;97m:\e[0m " fmt "\e[0m (line %d)\n", FILENAME, ##__VA_ARGS__, __LINE__); abort(); } while(0)
#define SUCCEED() do { _enable_ansi_sequences(); printf("\e[1;97m-- %s \e[1;92msucceded\e[0m\n", FILENAME); exit(0); } while(0)

#define ASSERT_EQ(a, b) do { if ((a) != (b)) FAIL(#a " != " #b); } while(0)
#define ASSERT_NEQ(a, b) do { if ((a) == (b)) FAIL(#a " == " #b); } while(0)
#define ASSERT_STREQ(a, b) do { if (strcmp((a), (b)) != 0) FAIL(#a " != " #b); } while(0)

#endif
