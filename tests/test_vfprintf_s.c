/*------------------------------------------------------------------
 * test_vfprintf_s
 * File 'vfprintf_s.c'
 * Lines executed:100.00% of 18
 *
 *------------------------------------------------------------------
 */

#include "test_private.h"
#include "safe_str_lib.h"
#include <unistd.h>

#define TMP   "tmpvfp"
#define LEN   ( 128 )

static FILE* out;
static char   str1[LEN];
static char   str2[LEN];

int vtfprintf_s (FILE *restrict stream, const char *restrict fmt, ...) {
    int rc;
    va_list ap;
    va_start(ap, fmt);
    rc = vfprintf_s(stream, fmt, ap);
    va_end(ap);
    return rc;
}

int test_vfprintf_s (void)
{
    errno_t rc;
    int32_t ind;
    int errs = 0;

    out = fopen(TMP, "w");

/*--------------------------------------------------*/

    rc = vtfprintf_s(out, NULL);
    ERR(ESNULLP)

    rc = vtfprintf_s(NULL, "");
    ERR(ESNULLP)

    rc = vtfprintf_s(out, "", NULL);
    ERR(EOK)

    /* TODO
    rc = vtfprintf_s(NULL, "%s", NULL);
    ERR(ESNULLP);
    */

/*--------------------------------------------------*/

    str1[0] = '\0'; 
    rc = vtfprintf_s(out, "%s%n\n", str1, &ind);
    ERR(EINVAL)

/*--------------------------------------------------*/

    rc = vtfprintf_s(out, "%s%%n\n", str1);
    if (rc < 0) {
        printf("Failed to open stdout for write: %s\n",
               strerror(errno));
        return errs;
    }
    ERR(3)

    rc = vtfprintf_s(out, "%%n\n");
    ERR(3);

/*--------------------------------------------------*/

    /* TODO
    rc = vtfprintf_s(out, "%s", NULL);
    ERR(ESNULLP)
    */

/*--------------------------------------------------*/

    strcpy(str1, "12");
    strcpy(str2, "34");

    rc = vtfprintf_s(out, "%s%s", str1, str2);
    ERR(4);

/*--------------------------------------------------*/

#if 0
    /* 0x7fffffff + 1 >INT_MAX */
    rc = printf_s("\n%2147483648d\n", INT_MAX);
    ANYERR();
#if defined(__GLIBC__)
    ERRNO(EOVERFLOW);
#endif

    /* segfaults under darwin */
    rc = printf_s("%s\n", L'\xd834df01');
    ANYERR();
#endif

/*--------------------------------------------------*/

    fclose(out);

    /* print to closed stream */
    rc = vtfprintf_s(out, "%s", str1);
#if defined(__GLIBC__)
    if (rc < 0) {
        ERR(-1);
        ERRNO(EBADF);
    } else {
        /* older glibc upstream bug */
        NOERR();
        ERRNO(0);
    }
#else
    ERR(-1);
#endif
    /* musl throws no error */
    
    unlink(TMP);

    return (errs);
}

#ifndef __KERNEL__
/* simple hack to get this to work for both userspace and Linux kernel,
   until a better solution can be created. */
int main (void)
{
    return (test_vfprintf_s());
}
#endif