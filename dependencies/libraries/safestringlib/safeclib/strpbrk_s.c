/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "safe_str_lib.h"


/**
 * NAME
 *    strpbrk_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    errno_t
 *    strpbrk_s(char *dest, rsize_t dmax,
 *              char *src,  rsize_t slen, char **first)
 *
 * DESCRIPTION
 *    Returns a pointer, first, to the first ocurrence of any character
 *    in src which is contained in dest.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest     pointer to string
 *
 *    dmax     restricted maximum length of string dest
 *
 *    src      pointer to string
 *
 *    slen     restricted length of string src
 *
 *    first    returned pointer to first occurence
 *
 * OUTPUT PARAMETERS
 *    none
 *
 * RUNTIME CONSTRAINTS
 *    Neither dest nor src shall be a null pointer.
 *    first shall not be a null pointer.
 *    dmax shall not be 0
 *    dmax shall not be greater than RSIZE_MAX_STR
 *
 * RETURN VALUE
 *    pointer to the first ocurrence of any character
 *    contained in src
 *
 *    EOK         count
 *    ESNULLP     NULL pointer
 *    ESZEROL     zero length
 *    ESLEMAX     length exceeds max limit
 *
 * ALSO SEE
 *    strfirstchar_s(), strlastchar_s(), strfirstdiff_s(),
 *    strfirstsame_s(), strlastdiff_s(), strlastsame_s()
 *
 */
errno_t
strpbrk_s (char *dest, rsize_t dmax,
           char *src,  rsize_t slen, char **first)
{
    char *ps;

    if (first == NULL) {
        invoke_safe_str_constraint_handler("strpbrk_s: count is null",
                   NULL, ESNULLP);
        return RCNEGATE(ESNULLP);
    }
    *first = NULL;

    if (dest == NULL) {
        invoke_safe_str_constraint_handler("strpbrk_s: dest is null",
                   NULL, ESNULLP);
        return RCNEGATE(ESNULLP);
    }

    if (src == NULL) {
        invoke_safe_str_constraint_handler("strpbrk_s: src is null",
                   NULL, ESNULLP);
        return RCNEGATE(ESNULLP);
    }

    if (dmax == 0 ) {
        invoke_safe_str_constraint_handler("strpbrk_s: dmax is 0",
                   NULL, ESZEROL);
        return RCNEGATE(ESZEROL);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strpbrk_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return RCNEGATE(ESLEMAX);
    }

    if (slen == 0 ) {
        invoke_safe_str_constraint_handler("strpbrk_s: slen is 0",
                   NULL, ESZEROL);
        return RCNEGATE(ESZEROL);
    }

    if (slen > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strpbrk_s: slen exceeds max",
                   NULL, ESLEMAX);
        return RCNEGATE(ESLEMAX);
    }

    /*
     * look for a matching char in the substring src
     */
    while (dmax && *dest) {

        rsize_t len = slen;
        ps = src;
        while (*ps && len > 0) {
            /* check for a match with the substring */
            if (*dest == *ps) {
                *first = dest;
                return RCNEGATE(EOK);
            }
            ps++;
            len--;
        }
        dest++;
        dmax--;
    }

    return RCNEGATE(ESNOTFND);
}
EXPORT_SYMBOL(strpbrk_s)
