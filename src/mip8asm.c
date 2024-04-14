#include <ctype.h>
#include <stdlib.h>
#include <assert.h>

#include "core/util.h"


err_t process_word(char *word, int wl) {
    if (word[0] == '$') {
        CHECK_RETURN_VALUE(wl == 3 && isxdigit(word[1]) && isxdigit(word[2]), err_format("could not parse literal byte value"));
        static const u8 convertion_table[] = { [0] = 0, [1] = 1, [2] = 2, [3] = 3, [4] = 4, [5] = 5, [6] = 6, [7] = 7, [8] = 8, [9] = 9, [17] = 0xA, [18] = 0xB, [19] = 0xC, [20] = 0xD, [21] = 0xE, [22] = 0xF };
        u8 b = (convertion_table[word[2] - '0']) | (convertion_table[word[1] - '0']) << 4;
        LOG("\"%s\" -> 0x%X", word, b);
    } else if (word[wl-1] == ':') {
        // remember new identifier and its address
    } else if (word[0] == '.') {
        LOG("\"%s\" -> ...", word);
    } else if (word[0] == '#') {
        // parse identifier definition
    } else {
        // lookup identifier table
    }

    return err_success();
}

int main(int argc, char const *argv[])
{
    if (argc < 2) {
        LOG("Usage: mip8asm <src>");
        return 0;
    }

    char *text = NULL;
    u64 text_size = 0;
    CHECK_ERR_LOG_RETURN_VALUE(load_file(argv[1], &text, &text_size), 1);

    char word[64] = {'\0'};
    int wl = 0;
    for (char *c = text; *c != '\0'; c++) {
        ASSERT_LOG_RETURN_VALUE(wl < sizeof(word) - 1, 1, "word is too large");

        if (isspace(*c)) {
            if (wl == 0)
                continue;
            word[wl] = '\0';
            CHECK_ERR_LOG_RETURN_VALUE(process_word(word, wl), 1);
            wl = 0;
            continue;
        }

        if (*c == ';') {
            for (; *c != '\0' && *c != '\n'; c++);
            continue;
        }

        word[wl] = *c;
        wl++;
    }

    return 0;
}