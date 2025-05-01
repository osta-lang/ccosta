#include <stdio.h>

#include "syntax/lex.h"

int main() {
    token_t token = lex("r###\"###\"##\"###");
    printf("token.kind = %d\n", token.kind);
    printf("token.slice.str = %.*s\n", (int) token.slice.len, token.slice.str);
    printf("token.as.slice.str = %.*s\n", (int) token.as.slice.len, token.as.slice.str);

    return 0;
}
