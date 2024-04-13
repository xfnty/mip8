#include "asm.h"

#include <stdlib.h>
#include <assert.h>


err_t asm_compile(const char *text, u8 **out_buffer, u64 *out_buffer_size) {
    assert(text != NULL);
    assert(out_buffer != NULL);
    assert(out_buffer_size != NULL);

    /*

    labels: (name, addr)[]
    unidentified_labels: (name, pos)[]
    ids: (name, byte)[]
    
    for word in text:
        if word.startswith('$'):
            buffer.push(parse_byte(word[1:]))
        elif word.startswith('.'):
            // handle missing labels
            buffer.push(labels[word[1:]].addr)
        elif word.endswith(':'):
            labels.set_or_default(word[:-1], buffer.position)
        elif word.startswith('#'):
            ids.set_or_default(word[1:], parse_byte(next_word))
    */

    trie_t(char, u8) labels;
    trie_t(char, struct {u8 addr; const char* src;}) undefined_labels;

    u64 out_buffer_capacity = 0;

    const char *word;
    while (word = get_next_word(text)) {
        if (word_starts_with(word, '$')) {
            u8 byte;
            CHECK_RETURN_VALUE(parse_byte(word + 1, &byte), err_format("could not parse byte value"));
            buffer_push(out_buffer, out_buffer_size, &out_buffer_capacity, byte);
        } else if (word_starts_with(word, '.')) {
            u8 addr = 0;
            if (trie_find(labels, word + 1, &addr)) {
                buffer_push(out_buffer, out_buffer_size, &out_buffer_capacity, addr);
                continue;
            }
            trie_add(undefined_labels, word, word.length - 1, {out_buffer.position, word});
        } else if (word_ends_with(word, ':')) {
            trie_add(labels, word, word.length - 1, out_buffer.position);
        } else if (word_starts_with(word, '#')) {
            trie_add(ids, word+1, word.length - 1, get_next_word(text));
        } else if (is_id(word)) {
            u8 byte = 0;
            CHECK(trie_find(ids, word, &byte), "unknown identifier");
            buffer_push(out_buffer, out_buffer_size, &out_buffer_capacity, byte);
        } else {
            return error("invalid expression `%s`", word);
        }
    }

    return err_success();
}

err_t asm_decompile(const u8 *binary, u64 binary_size, u8 **out_buffer, u64 *out_buffer_size) {
    assert(binary != NULL);
    assert(out_buffer != NULL);
    assert(out_buffer_size != NULL);

    return err_success();
}
