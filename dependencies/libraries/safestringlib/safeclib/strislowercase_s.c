/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2005 Bo Berry
 *  Copyright (c) 2008-2011, 2013 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_str_constraint.h"
#include "safe_str_lib.h"


/**
 * NAME
 *    strislowercase_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    bool
 *    strislowercase_s(const char *dest, rsize_t dmax)
 *
 * DESCRIPTION
 *    This function checks if entire string is lowercase.
 *    The scanning stops at the first null or after dmax
 *    characters.
 *
 * EXTENSION TO
 *    ISO/IEC TR 24731, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest      pointer to string
 *
 *    dmax      maximum length of string
 *
 * OUTPUT PARAMETERS
 *    none
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    dest shal be null terminated.
 *    dmax shall not equal zero.
 *    dmax shall not be greater than RSIZE_MAX_STR.
 *
 * RETURN VALUE
 *    true      string is lowercase
 *    false     string is not lowercase or an error occurred
 *
 * ALSO SEE
 *    strisalphanumeric_s(), strisascii_s(), strisdigit_s(),
 *    strishex_s(), strismixedcase_s(),
 *    strisuppercase_s()
 *
 */
bool
strislowercase_s (const char *dest, rsize_t dmax)
{
    if (!dest) {
        invoke_safe_str_constraint_handler("strislowercase_s: "
                   "dest is null",
                   NULL, ESNULLP);
        return (false);
    }

    if (dmax == 0) {
        invoke_safe_str_constraint_handler("strislowercase_s: "
                   "dmax is 0",
                   NULL, ESZEROL);
        return (false);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strislowercase_s: "
                   "dmax exceeds max",
                   NULL, ESLEMAX);
        return (false);
    }

    if (*dest == '\0') {
        return (false);
    }

    while (dmax && *dest) {

        if ((*dest < 'a') || (*dest > 'z')) {
            return (false);
        }
        dest++;
        dmax--;
    }

    return (true);
}
EXPORT_SYMBOL(strislowercase_s)
