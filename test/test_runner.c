#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF_SZ 1024
#define MAX_TEST_NAME 256
#define MAX_LINE_SZ 1024
#define MAX_TEST_CASES 256
#define MAX_TEST_SUITES 256
#define MAX_EXE_PATH 256

typedef struct {
    char name[MAX_TEST_NAME];
    char stdin[MAX_BUF_SZ];
    char stdout[MAX_BUF_SZ];
} testcase_t;

typedef struct {
    char executable[MAX_TEST_NAME];
    testcase_t tests[MAX_TEST_CASES];
    int test_count;
} testsuite_t;

testcase_t *parse_testfile(const char *testfile, testcase_t *tests, int *test_count)
{
    FILE *f = fopen(testfile, "r");
    if (!f) { perror("fopen"); exit(1); }

    *test_count = 0;
    char line[MAX_LINE_SZ];
    testcase_t *current_test;
    int in_stdin = 0;

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "test: ", 6) == 0) {
            if (*test_count < MAX_TEST_CASES) {
                fprintf(stderr, "too many test cases in %s\n", testfile);
                fclose(f);
            }
        }
    }
}
