#include <stdio.h>
#include <hpcalcs.h>
//#include <hpcables.h>
//#include <hpfiles.h>
#include <prime_cmd.h>

#define PRINTF(FUNCTION, TYPE, args...) \
fprintf(stderr, "%d\t" TYPE "\n", i, FUNCTION(args)); i++

#define PRINTFVOID(FUNCTION, args...) \
fprintf(stderr, "%d\n", i); FUNCTION(args); i++

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""
#define VOID ""

static void output_log_callback(const char *format, va_list args) {
    vprintf(format, args);
    fflush(stdout);
}

int main(int argc, char **argv) {
    int i = 1;

    hpcalcs_init(output_log_callback);
    hpcalcs_exit();

    return 0;
}
