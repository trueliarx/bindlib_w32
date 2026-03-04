#ifndef RESOLV_SECURE_COMPAT_H
#define RESOLV_SECURE_COMPAT_H

/* Disable MSVC secure CRT warnings in source files that include this header. */
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _MSC_VER
/* Also disable deprecation warnings (sprintf/strcpy/etc.) */
# pragma warning(disable:4996)
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

/* Case-insensitive compare portability */
#ifdef WINNT
#define STRCASECMP _stricmp
#define DUPSTR _strdup
#define strdup _strdup
#define strcasecmp _stricmp
#define getpid _getpid
#else
#define STRCASECMP strcasecmp
#define DUPSTR strdup
#endif

/* Provide a small, inline strcpy_s fallback for platforms that lack it.
 * Returns 0 on success or an errno value on failure.
 */
static inline int strcpy_s_compat(char *dest, size_t destsz, const char *src) {
    size_t srclen;
    if (dest == NULL || src == NULL)
        return EINVAL;
    if (destsz == 0)
        return EINVAL;
    srclen = strlen(src) + 1;
    if (srclen > destsz) {
        if (destsz > 0)
            dest[0] = '\0';
        return ERANGE;
    }
    memcpy(dest, src, srclen);
    return 0;
}

#ifndef HAVE_STRCPY_S
#define strcpy_s(dest, destsz, src) strcpy_s_compat((dest),(destsz),(src))
#endif

/* Safe copy for fixed-size buffers */
#define STRCOPY_SNPRINTF(dest, destsz, src) ((void)snprintf((dest),(destsz), "%s", (src)))

#endif /* RESOLV_SECURE_COMPAT_H */
