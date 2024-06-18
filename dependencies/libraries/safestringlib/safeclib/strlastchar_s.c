/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011, 2013 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "safe_str_lib.h"


/**
 * NAME
 *    strlastchar_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    errno_t
 *    strlastchar_s(char *dest, rsize_t dmax, char c, char **last)
 *
 * DESCRIPTION
 *    Returns a pointer to the last occurrence of character c in
 *    dest.  The scanning stops at the first null or after dmax
 *    characters.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest    pointer to string
 *
 *    dmax    restricted maximum length of string
 *
 *    c       character to locate
 *
 *    last    returned pointer to last occurrence
 *
 * OUTPUT PARAMETERS
 *    last    updated pointer to last occurrence
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    last shall not be a null pointer.
 *    dmax shall not be 0
 *    dmax shall not be greater than RSIZE_MAX_STR
 *
 * RETURN VALUE
 *    pointer to the last occurrence, when the return code is OK
 *
 *    EOK         pointer to the last occurence is returned
 *    ESNOTFND    c not found in dest
 *    ESNULLP     NULL pointer
 *    ESZEROL     zero length
 *    ESLEMAX     length exceeds max limit
 *
 * ALSO SEE
 *    strfirstchar_s(), strfirstdiff_s(), strfirstsame_s(),
 *    strlastdiff_s(), strlastsame_s()
 *
 */
errno_t
strlastchar_s(char *dest, rsize_t dmax, char c, char **last)
{
    if (last == NULL) {
        invoke_safe_str_constraint_handler("strlastchar_s: last is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }
    *last = NULL;

    if (dest == NULL) {
        invoke_safe_str_constraint_handler("strlastchar_s: dest is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }

    if (dmax == 0 ) {
        invoke_safe_str_constraint_handler("strlastchar_s: dmax is 0",
                   NULL, ESZEROL);
        return (ESZEROL);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strlastchar_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return (ESLEMAX);
    }

    while (dmax && *dest) {

        if (*dest == c) {
            *last = dest;
        }

        dest++;
        dmax--;
    }

    if (*last == NULL) {
        return (ESNOTFND);
    } else {
        return (EOK);
    }
}
EXPORT_SYMBOL(strlastchar_s)
