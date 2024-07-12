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
 *    strtolowercase_s
 *
 * SYNOPSIS
 *    #include "safe_str_lib.h"
 *    errno_t
 *    strlolowercase_s(char *dest, rsize_t dmax)
 *
 * DESCRIPTION
 *    Scans the string converting uppercase characters to
 *    lowercase, leaving all other characters unchanged.
 *    The scanning stops at the first null or after dmax
 *    characters.
 *
 * Extenstion to:
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
 *    dest       updated string
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    dmax shall not equal zero.
 *    dmax shall not be greater than RSIZE_MAX_STR.
 *
 * RETURN VALUE
 *    EOK        successful operation
 *    ESNULLP    NULL pointer
 *    ESZEROL    zero length
 *    ESLEMAX    length exceeds max limit
 *
 * ALSO SEE
 *    strtouppercase_s()
 *
 */
errno_t
strtolowercase_s (char *dest, rsize_t dmax)
{
    if (!dest) {
        invoke_safe_str_constraint_handler("strtolowercase_s: "
                   "dest is null",
                   NULL, ESNULLP);
        return (ESNULLP);
    }

    if (dmax == 0) {
        invoke_safe_str_constraint_handler("strtolowercase_s: "
                   "dmax is 0",
                   NULL, ESZEROL);
        return (ESZEROL);
    }

    if (dmax > RSIZE_MAX_STR) {
        invoke_safe_str_constraint_handler("strtolowercase_s: "
                   "dmax exceeds max",
                   NULL, ESLEMAX);
        return (ESLEMAX);
    }

    while (dmax && *dest) {

        if ( (*dest >= 'A') && (*dest <= 'Z')) {
              *dest = (char)(*dest + (char)32);
        }
        dest++;
        dmax--;
    }

    return (EOK);
}
EXPORT_SYMBOL(strtolowercase_s)
