/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011 Cisco Systems
 *  Copyright (c) 2018-2022 Intel Corp
 */

#include "safeclib_private.h"
#include "safe_mem_constraint.h"
#include "mem_primitives_lib.h"
#include "safe_mem_lib.h"

/**
 * NAME
 *    memcpy32_s
 *
 * SYNOPSIS
 *    #include "safe_mem_lib.h"
 *    errno_t
 *    memcpy32_s(uint32_t *dest, rsize_t dmax,
 *                const uint32_t *src, rsize_t smax)
 *
 * DESCRIPTION
 *    This function copies at most smax uint32_ts from src to dest, up to
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
 *    smax      maximum number uint32_t of src to copy
 *
 *  OUTPUT PARAMETERS
 *    dest      is updated
 *
 * RUNTIME CONSTRAINTS
 *    Neither dest nor src shall be a null pointer.
 *    Neither dmax nor smax shall be 0.
 *    dmax shall not be greater than RSIZE_MAX_MEM32.
 *    smax shall not be greater than dmax.
 *    Copying shall not take place between objects that overlap.
 *    If there is a runtime-constraint violation, the memcpy_s function stores
 *    zeros in the first dmax bytes of the object pointed to by dest
 *    if dest is not a null pointer and smax is valid.
 *
 * RETURN VALUE
 *    EOK       operation sucessful
 *    ESNULLP   NULL pointer
 *    ESZEROL   length was zero
 *    ESLEMAX   length exceeds max
 *    ESOVRLP   source memory overlaps destination
 *
 * ALSO SEE
 *    memcpy_s(), memcpy16_s(), memmove_s(), memmove16_s(), memmove32_s()
 *
 */
errno_t
memcpy32_s (uint32_t *dest, rsize_t dmax, const uint32_t *src, rsize_t smax)
{
    if (dest == NULL) {
        invoke_safe_mem_constraint_handler("memcpy32_s: dest is NULL",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    if (dmax == 0) {
        invoke_safe_mem_constraint_handler("memcpy32_s: dmax is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (dmax > RSIZE_MAX_MEM32) {
        invoke_safe_mem_constraint_handler("memcpy32_s: dmax exceeds max",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    if (smax == 0) {
        mem_prim_set32(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy32_s: smax is 0",
                   NULL, ESZEROL);
        return (RCNEGATE(ESZEROL));
    }

    if (smax > dmax) {
        mem_prim_set32(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy32_s: smax exceeds dmax",
                   NULL, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    if (src == NULL) {
        mem_prim_set32(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy32_s: src is NULL",
                   NULL, ESNULLP);
        return (RCNEGATE(ESNULLP));
    }

    /*
     * overlap is undefined behavior, do not allow
     */
    if( ((dest > src) && (dest < (src+smax))) ||
        ((src > dest) && (src < (dest+dmax))) ) {
        mem_prim_set32(dest, dmax, 0);
        invoke_safe_mem_constraint_handler("memcpy32_s: overlap undefined",
                   NULL, ESOVRLP);
        return (RCNEGATE(ESOVRLP));
    }

    /*
     * now perform the copy
     */
    mem_prim_move32(dest, src, smax);

    return (RCNEGATE(EOK));
}
EXPORT_SYMBOL(memcpy32_s)
