#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define CHUNKSIZE (256)

int vasprintf(char **ret, const char *fmt, va_list ap)
{
    size_t chunks;
    size_t buflen;
    char * buf;
    int    len;

    chunks = ((strlen(fmt) + 1) / CHUNKSIZE) + 1;
    buflen = chunks * CHUNKSIZE;

    *ret = NULL;
    for (;;) {
        buf = malloc(buflen);
        if (buf == NULL) {
            return -ENOMEM;
        }
        len = vsnprintf(buf, buflen, fmt, ap);

        if (len >= 0 && (size_t)len < buflen - 1) {
            break;
        }
        free(buf);
        buflen = (++chunks) * CHUNKSIZE;

        /*
        * len >= 0 is required for vsnprintf implementations that
        * return -1 for insufficient buffer
        */

        if (len >= 0 && buflen <= (size_t)len) {
            buflen = len + 1;
        }
    }

    if (len > 0 && buf != NULL) {
        // strip the string
        *ret = strdup(buf);
        if (*ret != NULL)
            free(buf);
        else
            *ret = buf;
    }

    return len;
}

int asprintf(char **buf, const char *fmt, ...)
{
    va_list args;
    int     i;

    va_start(args, fmt);
    i = vasprintf(buf, fmt, args);
    va_end(args);
    return i;
}
