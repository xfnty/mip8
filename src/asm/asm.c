#include "asm.h"

#include <ctype.h>

#include "core/cpu.h"


typedef arr_t(char) token_t;

struct label_t {
    token_t str;
    u32 hash;
    u8 addr;
};

struct alias_t {
    token_t str;
    u32 hash;
    u8 value;
};

struct parser_context_t {
    u64             cursor_position;
    const buffer_t  *source_buffer;
    buffer_t        *out_buffer;
    token_t         current_token;
    arr_t(struct label_t) labels;
    arr_t(struct label_t) unresolved_label_address_placeholders;
};

static arr_t(struct alias_t) s_builtin_aliases;

static void precompute_alias_hashes();
static void skip_space_and_comments(struct parser_context_t *ctx);
static bool parse_next_token(struct parser_context_t *ctx);
static err_t resolve_label_references(struct parser_context_t *ctx);
static err_t try_process_token(struct parser_context_t *ctx, token_t *token);

err_t asm_compile(const buffer_t *source_buffer, buffer_t *out_buffer) {
    assert(source_buffer);
    assert(out_buffer);

    struct parser_context_t ctx = {0};
    ctx.source_buffer = source_buffer;
    ctx.out_buffer = out_buffer;

    if (s_builtin_aliases.size == 0)
        precompute_alias_hashes();

    while (parse_next_token(&ctx))
        CHECK_ERR_PROPAGATE(try_process_token(&ctx, &ctx.current_token));

    CHECK_ERR_PROPAGATE(resolve_label_references(&ctx));

    return err_success();
}

void precompute_alias_hashes() {
    for (int i = 0; i < LENGTH(g_cpu_opcode_table); i++) {
        if (g_cpu_opcode_table[i].mnemonic == NULL)
            continue;

        struct alias_t a = {0};
        arr_assign(a.str, strdup(g_cpu_opcode_table[i].mnemonic), strlen(g_cpu_opcode_table[i].mnemonic));
        arr_push(a.str, '\0');
        a.value = i;
        a.hash = hash(a.str.data, a.str.size - 1);

        arr_push(s_builtin_aliases, a);
    }
}

bool parse_next_token(struct parser_context_t *ctx) {
    assert(ctx);

    arr_clear(ctx->current_token);

    skip_space_and_comments(ctx);

    if (ctx->cursor_position >= ctx->source_buffer->size)
        return false;

    while (ctx->cursor_position < ctx->source_buffer->size && !isspace(ctx->source_buffer->data[ctx->cursor_position]) && ctx->source_buffer->data[ctx->cursor_position] != '\0') {
        arr_push(ctx->current_token, ctx->source_buffer->data[ctx->cursor_position]);
        ctx->cursor_position++;
    }

    return ctx->current_token.size > 0;
}

void skip_space_and_comments(struct parser_context_t *ctx) {
    assert(ctx);

    char c = ctx->source_buffer->data[ctx->cursor_position];
    bool skipping_comment = c == ';';
    
    while (ctx->cursor_position < ctx->source_buffer->size && (skipping_comment || isspace(c))) {
        ctx->cursor_position++;
        c = ctx->source_buffer->data[ctx->cursor_position];
        skipping_comment = (skipping_comment) ? (c != '\n') : (c == ';');
    }
}

err_t resolve_label_references(struct parser_context_t *ctx) {
    static arr_t(int) unresolved_label_indexes = {0};

    assert(ctx);

    for (int i = 0; i < ctx->unresolved_label_address_placeholders.size; i++) {
        bool found_match = false;

        for (int j = 0; j < ctx->labels.size; j++) {
            if (ctx->unresolved_label_address_placeholders.data[i].hash == ctx->labels.data[j].hash) {
                ctx->out_buffer->data[ctx->unresolved_label_address_placeholders.data[i].addr] = ctx->labels.data[j].addr;
                found_match = true;
                break;
            }
        }

        if (!found_match)
            arr_push(unresolved_label_indexes, i);
    }

    if (unresolved_label_indexes.size > 0) {
        char b[512] = {'\0'};
        snprintf(b, sizeof(b), "could not resolve following label references: ");
        for (int i = 0; i < unresolved_label_indexes.size; i++) {
            struct label_t *l = &ctx->unresolved_label_address_placeholders.data[unresolved_label_indexes.data[i]];
            arr_push(l->str, '\0');
            snprintf(b, sizeof(b), "%s\"%s\"%s", b, l->str.data, (i < unresolved_label_indexes.size - 1) ? (", ") : (""));
            arr_pop(l->str);
        }
        arr_clear(unresolved_label_indexes);
        return err_format(b);
    }

    return err_success();
}

err_t try_process_token(struct parser_context_t *ctx, token_t *token) {
    assert(ctx);
    assert(token);

    if (token->data[0] == '$') {
        if (token->size != 3 || !isxdigit(token->data[1]) || !isxdigit(token->data[2])) {
            arr_push(*token, '\0');
            return err_format("invalid literal byte value \"%s\"", token->data);
        }

        static const u8 convertion_table[] = { [0] = 0, [1] = 1, [2] = 2, [3] = 3, [4] = 4, [5] = 5, [6] = 6, [7] = 7, [8] = 8, [9] = 9, [17] = 0xA, [18] = 0xB, [19] = 0xC, [20] = 0xD, [21] = 0xE, [22] = 0xF };
        arr_push(*(ctx->out_buffer), (convertion_table[token->data[2] - '0']) | (convertion_table[token->data[1] - '0']) << 4);
        
        return err_success();
    } else if (token->data[0] == '.') {
        u32 h = hash(token->data + 1, token->size - 1);
        for (u64 i = 0; i < ctx->labels.size; i++) {
            if (ctx->labels.data[i].hash == h) {
                arr_push(*(ctx->out_buffer), ctx->labels.data[i].addr);
                return err_success();
            }
        }
        
        token_t s = {0};
        arr_copy(s, &token->data[1], token->size - 1);
        struct label_t l = (struct label_t){ .str = s, .hash = h, .addr = ctx->out_buffer->size };
        arr_push(ctx->unresolved_label_address_placeholders, l);
        arr_push(*(ctx->out_buffer), 0xAA);
        
        return err_success();
    } else if (token->data[token->size - 1] == ':') {
        u32 h = hash(token->data, token->size - 1);

        for (u64 i = 0; i < ctx->labels.size; i++)
            if (ctx->labels.data[i].hash == h) {
                token->data[token->size - 1] = '\0';
                return err_format("redifinition of a label \"%s\"", token->data);
            }
        
        token_t s = {0};
        arr_copy(s, token->data, token->size - 1);
        struct label_t l = (struct label_t){ .str = s, .hash = h, .addr = ctx->out_buffer->size };
        arr_push(ctx->labels, l);

        return err_success();
    } else {
        u32 h = hash(token->data, token->size);
        for (int i = 0; i < s_builtin_aliases.size; i++) {
            if (s_builtin_aliases.data[i].hash == h) {
                arr_push(*(ctx->out_buffer), s_builtin_aliases.data[i].value);
                return err_success();
            }
        }
    }

    arr_push(*token, '\0');
    return err_format("could not parse token \"%s\"", token->data);
}