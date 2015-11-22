#include <stdio.h>
#include <hpfiles.h>
#include <hpcables.h>
#include <hpcalcs.h>
#include <hpopers.h>
#include <filetypes.h>
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

    hpfiles_init(NULL);
    hpfiles_exit();

    hpcables_init(NULL);
    hpcables_exit();

    hpcalcs_init(NULL);
    hpcalcs_exit();

    hpopers_init(NULL);
    hpopers_exit();

    return 0;
}
