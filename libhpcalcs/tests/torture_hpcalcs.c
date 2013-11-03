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

int main(int argc, char **argv)
{
    int i = 1;

    hpcalcs_init();
    hpcalcs_exit();

    return 0;
}
