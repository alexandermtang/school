#include <stdbool.h>
#include <ctype.h>
#include <sys/stat.h>
#include "util.h"

bool isnum(char *str)
{
    char c;
    while ((c = *str++) != '\0') {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool file_exists(char *filename)
{
    struct stat info;
    return (stat(filename, &info) == 0);
}
