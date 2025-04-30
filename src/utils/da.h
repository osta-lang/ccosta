#ifndef DA_H
#define DA_H

#define DA(T) struct { \
    T *data; \
    size_t len; \
    size_t cap; \
}

#define DA_PUSH(da, value) { \
    if ((da).len == (da).cap) { \
        (da).cap = (da).cap >= 4090 ? 4096 : (da).cap ? (da).cap << 1 : 1; \
        void *data = realloc((da).data, (da).cap * sizeof(*(da).data)); \
        if (!data) { \
            fprintf(stderr, "Failed to allocate memory for da\n"); \
            exit(1); \
        } \
        (da).data = data; \
    } \
    (da).data[(da).len++] = value; \
}

// #define DA_INSERT(da, i, value) { \

#define DA_POP(da) ({ \
    if ((da).len == 0) { \
        fprintf(stderr, "da is empty\n"); \
        exit(1); \
    } \
    (da).data[--(da).len]; \
})

#define DA_FREE(da) { \
    free((da).data); \
    (da).data = NULL; \
    (da).len = 0; \
    (da).cap = 0; \
}

#endif //DA_H