/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2011 Bo Berry
 *  Copyright (c) 2011 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "safe_str_lib.h"


/**
 * NAME
 *    strnterminate_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    rsize_t
 *    strnterminate_s(char *dest, rsize_t dmax)
 *
 * DESCRIPTION
 *    The strnterminate_s function will terminate the string if a
 *    null is not encountered before dmax characters.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731-1, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest - pointer to string
 *
 *    dmax - restricted maximum length
 *
 * OUTPUT PARAMETERS
 *    dest - dest is terminated if needed
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer
 *    dmax shall not be greater than RSIZE_MAX_STR
 *    dmax shall not equal zero
 *
 * RETURN VALUE
 *    The function returns a terminated string.  If a null is not
 *    encountered prior to dmax characters, the dmax character is
 *    set to null terminating the string. The string length is
 *    also returned.
 *
 * ALSO SEE
 *    strnlen_s()
 *
 */
rsize_t
strnterminate_s (char *dest, rsize_t dmax)
{
    rsize_t count;

    if (dest == NULL) {
        return (0);
    }

    if (dmax == 0) {
        invoke_safe_str_constraint_handler("strnterminate_s: dmax is 0",
                   NULL, ESZEROL);
        return (0);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strnterminate_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return (0);
    }

    count = 0;
    while (dmax > 1) {
        if (*dest) {
            count++;
            dmax--;
            dest++;
        } else {
            break;
        }
    }
    *dest = '\0';

    return (count);
}
EXPORT_SYMBOL(strnterminate_s)
