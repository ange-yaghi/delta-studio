/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011 Cisco Systems
 */

#include "safeclib_private.h"
#include "safe_mem_constraint.h"
#include "mem_primitives_lib.h"
#include "safe_mem_lib.h"


/**
 * NAME
 *    memcpy16_s
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    errno_t
 *    memcpy16_s(uint16_t *dest, rsize_t dmax,
 *               const uint16_t *src, rsize_t smax)
 *
 * DESCRIPTION
 *    This function copies at most smax uint16_ts from src to dest, up to
 *    dmax.
 *
 * EXTENSION TO
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * INPUT PARAMETERS
 *    dest      pointer to memory that will be replaced by src.
 *
 *    dmax      maximum length of the resulting dest
 *
 *    src       pointer to the memory that will be copied to dest
 *
 *    smax      maximum number uint16_t of src to copy
 *
 *
 *  OUTPUT PARAMETERS
 *    dest      is updated
 *
 * RUNTIME CONSTRAINTS
 *    Neither dest nor src shall be a null pointer.
 *    Neither dmax nor smax shall be 0.
 *    dmax shall not be greater than RSIZE_MAX_MEM16.
 *    smax shall not be greater than dmax.
 *    Copying shall not take place between objects that overlap.
 *    If there is a runtime-constraint violation, the memcpy_s function stores
 *    zeros in the first dmax bytes of the object pointed to by dest
 *    if dest is not a null pointer and smax is valid.
 *
 * RETURN VALUE
 *    EOK        successful operation
 *    ESNULLP    NULL pointer
 *    ESZEROL    zero length
 *    ESLEMAX    length exceeds max limit
 *    ESOVRLP    source memory overlaps destination
 *
 * ALSO SEE
 *    memcpy_s(), memcpy32_s(), memmove_s(), memmove16_s(), memmove32_s()
 *
 */
errno_t
memcpy16_s (uint16_t *dest, rsize_t dmax, const uint16_t *src, rsize_t smax)
{
    if (dest == NULL) {
        invoke_safe_mem_constraint_handler("memcpy16_s: dest is NULL",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    if (dmax == 0) {
        invoke_safe_mem_constraint_handler("memcpy16_s: dmax is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (dmax > RSIZE_MAX_MEM16) {
        invoke_safe_mem_constraint_handler("memcpy16_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    if (smax == 0) {
        mem_prim_set16(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy16_s: smax is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (smax > dmax) {
        mem_prim_set16(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy16_s: smax exceeds dmax",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    if (src == NULL) {
        mem_prim_set16(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy16_s: src is NULL",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    /*
     * overlap is undefined behavior, do not allow
     */
    if( ((dest > src) && (dest < (src+smax))) ||
        ((src > dest) && (src < (dest+dmax))) ) {
        mem_prim_set16(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy16_s: overlap undefined",
                   NULL, ESOVRLP);
        return (RCNEGATE(ESOVRLP));
    }

    /*
     * now perform the copy
     */
    mem_prim_move16(dest, src, smax);

    return (RCNEGATE(EOK));
}
EXPORT_SYMBOL(memcpy16_s)
