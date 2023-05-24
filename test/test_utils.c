#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include "test_utils.h"

int err_report(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    return -1;
}

int fd2 = 0;

int redirect_stdout_to_file(char *file)
{
    /* Connect standard output to given file */
    fflush(stdout);
    int fd1 = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd1 < 0)
        return err_report("Failed to open %s for writing\n", file);
    // int fd2 = dup(STDOUT_FILENO);
    fd2 = dup(STDOUT_FILENO);
    if (fd2 < 0)
        return err_report("Failed to duplicate standard output\n");
    if (dup2(fd1, STDOUT_FILENO) < 0)
        return err_report("Failed to duplicate %s to standard output\n", file);
    close(fd1);

    return 0;
}

int reconnect_echo_to_stdout(void) {
    /* Reconnect original standard output */
    fflush(stdout);
    if (dup2(fd2, STDOUT_FILENO) < 0)
        return err_report("Failed to reinstate standard output\n");
    close(fd2);
    fd2 = 0;
    return 0;
}


int assert_count = 0;
int assert_failed = 0;
int assert_succeeded = 0;

void assert_true(bool predicate, char* msg) {
    assert_count++;
    printf("= testing %s\n", msg);
    if (predicate) {
        assert_succeeded++;
        printf("[OK] ✅\n");
    } else {
        assert_failed++;
        printf("[FAILED] ❌\n");
    }
}

void print_assert_status(void) {
    printf("= tests succeeded %d/%d │ %d failures =\n", assert_succeeded, assert_count, assert_failed);
}
