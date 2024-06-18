/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2014 by Intel Corp
 */

#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "safe_str_lib.h"


/**
 * NAME
 *    wcsnlen_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    rsize_t
 *    wcsnlen_s(const wchar_t *dest, rsize_t dmax)
 *
 * DESCRIPTION
 *    The wcsnlen_s function computes the length of the wide character string pointed
 *    to by dest.
 *
 * SPECIFIED IN
 *    ISO/IEC TR 24731-1, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest      pointer to wide character string
 *
 *    dmax      restricted maximum length.
 *
 * OUTPUT PARAMETERS
 *    none
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer
 *    dmax shall not be greater than RSIZE_MAX_STR
 *    dmax shall not equal zero
 *
 * RETURN VALUE
 *    The function returns the number of wide characters in the string
 *    pointed to by dest, excluding  the terminating null character.
 *    If dest is NULL, then wcsnlen_s returns 0.
 *
 *    Otherwise, the wcsnlen_s function returns the number of wide characters
 *    that precede the terminating null character. If there is no null
 *    character in the first dmax characters of dest then wcsnlen_s returns
 *    dmax. At most the first dmax characters of dest are accessed
 *    by wcsnlen_s.
 *
 * ALSO SEE
 *    strnlen_s, strnterminate_s()
 *
 */
rsize_t
wcsnlen_s (const wchar_t *dest, rsize_t dmax)
{
    rsize_t count;

    if (dest == NULL) {
        return RCNEGATE(0);
    }

    if (dmax == 0) {
        invoke_safe_str_constraint_handler("wcsnlen_s: dmax is 0",
                   NULL, ESZEROL);
        return RCNEGATE(0);
    }

    if (dmax*sizeof(wchar_t) > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("wcsnlen_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return RCNEGATE(0);
    }

    count = 0;
    while (dmax && *dest) {
        count++;
        dmax--;
        dest++;
    }

    return RCNEGATE(count);
}
EXPORT_SYMBOL(wcsnlen_s)
