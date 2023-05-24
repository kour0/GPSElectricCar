#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <stdbool.h>

int err_report(const char *fmt, ...);

int redirect_stdout_to_file(char *file);

int reconnect_echo_to_stdout(void);


void assert_true(bool predicate, char* msg);

void print_assert_status(void);

#endif // TEST_UTILS_H
