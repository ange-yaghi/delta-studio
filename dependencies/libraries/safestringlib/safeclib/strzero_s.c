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
 *    strzero_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    errno_t
 *    strzero_s(char *dest, rsize_t dmax)
 *
 * DESCRIPTION
 *    Nulls dmax characters of dest.  This function can be used
 *    to clear strings that contained sensitive data.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest     pointer to string that will be nulled.
 *
 *    dmax     restricted maximum length of dest
 *
 * OUTPUT PARAMETERS
 *    dest     updated string
 *
 * RETURN VALUE
 *    EOK        successful operation
 *    ESNULLP    NULL pointer
 *    ESZEROL    zero length
 *    ESLEMAX    length exceeds max limit
 *
 * ALSO SEE
 *    strispassword_s()
 *
 */
errno_t
strzero_s (char *dest, rsize_t dmax)
{
    if (dest == NULL) {
        invoke_safe_str_constraint_handler("strzero_s: dest is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }

    if (dmax == 0) {
        invoke_safe_str_constraint_handler("strzero_s: dmax is 0",
                   NULL, ESZEROL);
        return (ESZEROL);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strzero_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return (ESLEMAX);
    }

    /* null string to eliminate data */
    while (dmax) {
        *dest = '\0';
        dmax--;
        dest++;
    }

    return (EOK);
}
EXPORT_SYMBOL(strzero_s)
