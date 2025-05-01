#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

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

int parse_testfile(const char *testfile, testcase_t *tests, int *test_count)
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
                return 1;
            }

            current_test = &tests[*test_count];
            strncpy(current_test->name, line, MAX_TEST_NAME);
            current_test->name[MAX_TEST_NAME - 1] = 0;
            current_test->stdin[0] = 0;
            current_test->stdout[0] = 0;
            in_stdin = 1;
            (*test_count)++;
            continue;
        }

        if (strncmp(line, "========", 8) == 0) {
            in_stdin = 0;
            continue;
        }

        if (!current_test) {
            fprintf(stderr, "invalid test format file in : %s\n", testfile);
            fclose(f);
            return 1;
        }

        char *target = in_stdin ? current_test->stdin : current_test->name;
        size_t target_size = MAX_BUF_SZ;
        size_t current_len = strlen(target);
        size_t line_len = strlen(line);

        if (current_len + line_len + 1 >= target_size) {
            fprintf(stderr, "%s too large in %s\n", in_stdin ? "stdin" : "stdout", testfile);
            fclose(f);
            return 1;
        }

        strcat(target, line);
        strcat(target, "\n");
    }

    fclose(f);
    return 0;
}

void run_test(const char *executable, testcase_t *test)
{
    int stdin_pipe[2], stdout_pipe[2];
    pid_t pid;

    if (pipe(stdin_pipe) == -1 || pipe(stdout_pipe) == -1) {
      perror("pipe");
      exit(1);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(stdin_pipe[1]);
        close(stdout_pipe[0]);

        if (dup2(stdin_pipe[0], STDIN_FILENO) == -1) {
            perror("dup2 stdin");
            exit(1);
        }
        close(stdin_pipe[0]);

        if (dup2(stdout_pipe[1], STDOUT_FILENO) == -1) {
            perror("dup2 stdout");
            exit(1);
        }
        close(stdout_pipe[1]);

        char *args[] = {(char *)executable, (char *)NULL};
        execvp(executable, args);
        perror("execvp");
        exit(1);
    }

    close(stdin_pipe[0]);
    close(stdout_pipe[1]);

    size_t stdin_len = strlen(test->stdin);
    write(stdin_pipe[1], test->stdin, stdin_len);
    close(stdin_pipe[1]);

    char buffer[MAX_BUF_SZ];
    size_t buffer_pos = 0;
    ssize_t nbytes = 0;
    while ((nbytes = read(stdout_pipe[0], buffer + buffer_pos, MAX_BUF_SZ - buffer_pos - 1)) > 0) {
         buffer_pos += nbytes;
    }
    buffer[buffer_pos] = 0;
    close(stdout_pipe[0]);

    if (strcmp(buffer, test->stdout) == 0) {
        printf("test %s passed\n", test->name);
    } else {
        // TODO: mostrar el diff
        printf("test %s failed\n", test->name);
    }

    int status;
    waitpid(pid, &status, 0);
}

int main() {
    DIR *dir = opendir("test");
    if (!dir) {
        perror("opendir");
        return 1;
    }

    testsuite_t suites[MAX_TEST_SUITES];
    int suites_count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir))) {
        if (entry->d_type == DT_DIR) {
            char *name = entry->d_name;
            if (strncmp(name, "test_runner", 11) == 0) continue;

            char path[MAX_EXE_PATH];
            struct stat st;
            if (stat(path, &st) == 0 && (st.st_mode & S_IXUSR)) {
                char testfile[MAX_EXE_PATH];
                assert(strlen(name) < MAX_EXE_PATH - strlen("test/") + strlen(".test"));
                snprintf(testfile, sizeof(testfile), "test/%s.test", name);
                if (access(testfile, F_OK) == 0) {
                    if (suites_count >= MAX_TEST_SUITES) {
                        fprintf(stderr, "too many test suites\n");
                        closedir(dir);
                        return 1;
                    }
                    testsuite_t *suite = &suites[suites_count];
                    strncpy(suite->executable, path, MAX_EXE_PATH -1);
                    suite->executable[MAX_EXE_PATH - 1] = 0;
                    if (parse_testfile(testfile, suite->tests, &suite->test_count) != 0) {
                        closedir(dir);
                        return 1;
                    }

                    suites_count++;
                }
            }
        }
    }
    closedir(dir);

    // execute tests in parallel
    // TODO: use semaphore to limite parallelism
    for (int i = 0; i < suites_count; i++) {
        for (int j = 0; j < suites[i].test_count; j++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                return 1;
            }

            if (pid == 0) {
                run_test(suites[i].executable, &suites[i].tests[j]);
                exit(0);
            }
        }
    }

    while (wait(NULL) > 0);

    return 0;
}