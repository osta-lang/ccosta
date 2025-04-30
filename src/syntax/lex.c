#include "lex.h"

#include <stdio.h>
#include <stdlib.h>

#include "../utils/da.h"

typedef struct {
    DA(slice_t) slices;
    DA(size_t) building;
} capture_stack_t;

void begin_capture(capture_stack_t *stack, const char *src)
{
    DA_PUSH(stack->building, stack->slices.len);
    DA_PUSH(stack->slices, ((slice_t){src, 0}));
}

void end_capture(capture_stack_t *stack, const char *src)
{
    size_t start = DA_POP(stack->building);
    slice_t slice = stack->slices.data[start];
    slice.len = src - slice.str;
    stack->slices.data[start] = slice;
}

void free_capture(capture_stack_t *stack)
{
    DA_FREE(stack->slices);
    DA_FREE(stack->building);
}

slice_t get_capture(const capture_stack_t *stack, size_t index)
{
    return stack->slices.data[index];
}

token_t create_token(const slice_t slice, enum token_kind_t kind, union token_as_t as)
{
    return (token_t){slice, kind, as};
}

token_t ident_token(capture_stack_t *stack)
{
    token_t token = create_token(get_capture(stack, 0), TOKEN_IDENTIFIER, (union token_as_t){0});
    free_capture(stack);
    return token;
}

token_t rstr_token(capture_stack_t *stack)
{
    token_t token = create_token(get_capture(stack, 0), TOKEN_RSTR, (union token_as_t){ .slice = get_capture(stack, 2) });
    free_capture(stack);
    return token;
}

token_t error(const char *src)
{
    fprintf(stderr, "Error: Unexpected character '%c'\n", *src);
    exit(1);
}

token_t lex(const char* src)
{
    capture_stack_t stack = {0};
    const char *marker;

    begin_capture(&stack, src);

    switch (*src)
    {
    case 'r':
        ++src; goto rstr_r;
    case 'a' ... 'q':
    case 's' ... 'z':
    case 'A' ... 'Z':
    case '_':
        ++src; goto ident;
    default:
        return error(src);
    }
ident:
    switch (*src)
    {
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '0' ... '9':
    case '_':
        ++src; goto ident;
    default:
        end_capture(&stack, src); return ident_token(&stack);
    }
rstr_r:
    switch (*src)
    {
    case '#':
        begin_capture(&stack, src); goto rstr_hash;
    case '"':
        goto rstr_quote;
    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '0' ... '9':
    case '_':
        goto ident;
    default:
        break;
    }
rstr_hash:
    switch (*src)
    {
    case '#':
        ++src; goto rstr_hash;
    case '"':
        end_capture(&stack, src); ++src; begin_capture(&stack, src); goto rstr_quote;
    default:
        return error(src);
    }
rstr_quote:
    switch (*src)
    {
    case '"':
        if (stack.slices.len == 2)
        {
            end_capture(&stack, src - 1); goto rstr_end;
        }
        slice_t hash_group = get_capture(&stack, 1);
        marker = src + 1;
        for (size_t i = 0; i < hash_group.len && marker; ++i, ++marker)
        {
            if (*marker != hash_group.str[i])
            {
                ++src; goto rstr_quote;
            }
        }
        src = marker - 1; end_capture(&stack, src - hash_group.len); goto rstr_end;
    case '\0':
        return error(src);
    default:
        ++src; goto rstr_quote;
    }
rstr_end:
    ++src;
    end_capture(&stack, src);
    return rstr_token(&stack);
}