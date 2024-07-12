/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011, 2013 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "safe_str_lib.h"


/*
 *-
 * NAME
 *    strisascii_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    bool
 *    strisascii_s(const char *dest, rsize_t dmax)
 *
 * DESCRIPTION
 *    This function checks if the entire string contains ascii
 *    characters.  The scanning stops at the first null or
 *    at most dmax characters.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest       pointer to string
 *
 *    dmax       maximum length of string
 *
 * OUTPUT PARAMETERS
 *    none
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    dmax shall not equal zero.
 *    dmax shall not be greater than RSIZE_MAX_STR.
 *
 * RETURN VALUE
 *    true, string is ascii
 *    false, string contains one or more non-ascii or an error occurred
 *
 * ALSO SEE
 *    strisalphanumeric_s(), strisdigit_s(), strishex_s(),
 *    strislowercase_s(), strismixedcase_s(), strisuppercase_s()
 *-
 */
bool
strisascii_s (const char *dest, rsize_t dmax)
{
    if (!dest) {
        invoke_safe_str_constraint_handler("strisascii_s: dest is null",
                   NULL, ESNULLP);
        return (false);
    }

    if (dmax == 0) {
        invoke_safe_str_constraint_handler("strisascii_s: dmax is 0",
                   NULL, ESZEROL);
        return (false);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strisascii_s: dmax "
                   "exceeds max",
                   NULL, ESLEMAX);
        return (false);
    }

    while (dmax && *dest) {
        if ((unsigned char)*dest > 127) {
            return (false);
        }
        dest++;
        dmax--;
    }

    return (true);
}
EXPORT_SYMBOL(strisascii_s)
