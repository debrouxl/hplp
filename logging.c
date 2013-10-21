#include "logging.h"

void hpcables_debug (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables DEBUG: ") + 4);
    sprintf(format2, "hpcables DEBUG: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_info (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables INFO: ") + 4);
    sprintf(format2, "hpcables INFO: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_warning (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables WARN: ") + 4);
    sprintf(format2, "hpcables WARN: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcables_error (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcables ERROR: ") + 4);
    sprintf(format2, "hpcables ERROR: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}



void hpcalcs_debug (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs DEBUG: ") + 4);
    sprintf(format2, "hpcalcs DEBUG: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_info (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs INFO: ") + 4);
    sprintf(format2, "hpcalcs INFO: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_warning (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs WARN: ") + 4);
    sprintf(format2, "hpcalcs WARN: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}

void hpcalcs_error (const char *format, ...) {
    char * format2 = (char *)alloca(strlen(format) + sizeof("hpcalcs ERROR: ") + 4);
    sprintf(format2, "hpcalcs ERROR: %s\n", format);
    va_list args;
    va_start (args, format);
    vprintf(format2, args);
}
