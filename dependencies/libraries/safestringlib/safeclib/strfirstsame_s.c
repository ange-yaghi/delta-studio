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
 *    strfirstsame_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    errno_t
 *    strfirstsame_s(const char *dest, rsize_t dmax,
 *                   const char *src,  rsize_t *index)
 *
 * DESCRIPTION
 *    Returns the index of the first character that is the
 *    same between dest and src.  The scanning stops at the
 *    fisrt null in dest or src, or after dmax characters.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest     pointer to string to compare against
 *
 *    dmax     restricted maximum length of string dest
 *
 *    src      pointer to the string to be compared to dest
 *
 *    index    pointer to returned index
 *
 * OUTPUT PARAMETERS
 *    index    updated index
 *
 * RUNTIME CONSTRAINTS
 *    Neither dest nor src shall be a null pointer.
 *    indicator shall not be a null pointer.
 *    dmax shall not be 0
 *    dmax shall not be greater than RSIZE_MAX_STR
 *
 * RETURN VALUE
 *    index to first same char, when the return code is OK
 *
 *    EOK         index to first same char is returned
 *    ESNULLP     NULL pointer
 *    ESZEROL     zero length
 *    ESLEMAX     length exceeds max limit
 *    ESNOTFND    not found
 *
 * ALSO SEE
 *    strfirstchar_s(), strfirstdiff_s(), strlastchar_s(),
 *    strlastdiff_s(), strlastsame_s()
 *
 */
errno_t
strfirstsame_s (const char *dest, rsize_t dmax,
                const char *src,  rsize_t *index)
{
    const char *rp = 0;

    if (index == NULL) {
        invoke_safe_str_constraint_handler("strfirstsame_s: index is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }
    *index = 0;

    if (dest == NULL) {
        invoke_safe_str_constraint_handler("strfirstsame_s: dest is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }

    if (src == NULL) {
        invoke_safe_str_constraint_handler("strfirstsame_s: src is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }

    if (dmax == 0 ) {
        invoke_safe_str_constraint_handler("strfirstsame_s: dmax is 0",
                   NULL, ESZEROL);
        return (ESZEROL);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strfirstsame_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return (ESLEMAX);
    }

    /* hold reference point */
    rp = dest;

    /*
     * find the offset
     */
    while (*dest && *src && dmax) {

        if (*dest == *src) {
            *index = (uint32_t)(dest - rp);
            return (EOK);
        }

        dest++;
        src++;
        dmax--;
    }

    return (ESNOTFND);
}
EXPORT_SYMBOL(strfirstsame_s)
