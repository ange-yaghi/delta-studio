/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2014 by Intel Corp
 */

#include "safeclib_private.h"
#include "safe_mem_constraint.h"
#include "mem_primitives_lib.h"
#include "safe_mem_lib.h"


/**
 * NAME
 *    wmemset_s
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    errno_t
 *    wmemset_s(wchar_t *dest, wchar_t value, rsize_t len)
 *
 * DESCRIPTION
 *    Sets len number of wide characters starting at dest to the specified value.
 *
 * SPECIFIED IN
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest       pointer to memory that will be set to the value
 *
 *    value      byte value
 *
 *    len        number of wide characters to be set
 *
 * OUTPUT PARAMETERS
 *    dest      is updated
 *
 * RUNTIME CONSTRAINTS
 *    dest shall not be a null pointer.
 *    len shall not be 0 nor greater than RSIZE_MAX_MEM/sizeof(wchar_t).
 *    If there is a runtime constraint, the operation is not performed.
 *
 * RETURN VALUE
 *    EOK        successful operation
 *    ESNULLP    NULL pointer
 *    ESZEROL    zero length
 *    ESLEMAX    length exceeds max limit
 *
 * ALSO SEE
 *    memset_s, memset16_s(), memset32_s()
 *
 */
errno_t
wmemset_s (wchar_t *dest, wchar_t value, rsize_t len)
{
    if (dest == NULL) {
        invoke_safe_mem_constraint_handler("wmemset_s: dest is null",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    if (len == 0) {
        invoke_safe_mem_constraint_handler("wmemset_s: len is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (len*sizeof(wchar_t) > RSIZE_MAX_MEM) {
        invoke_safe_mem_constraint_handler("wmemset_s: len exceeds max",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    if (sizeof(wchar_t) == sizeof(uint32_t)) {
        mem_prim_set32((uint32_t *)dest, len, value);
        return (RCNEGATE(EOK));
    }

    if (sizeof(wchar_t) == sizeof(uint16_t)) {
        mem_prim_set16((uint16_t *)dest, len, value);
        return (RCNEGATE(EOK));
    }

    return (RCNEGATE(ESNOSPC));
}
EXPORT_SYMBOL(wmemset_s)
