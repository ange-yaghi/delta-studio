/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2011 Cisco Systems
 *  Copyright (c) 2022 Intel Corp
 */

#ifndef __MEM_PRIMITIVES_LIB_H__
#define __MEM_PRIMITIVES_LIB_H__

#include "safeclib_private.h"

/*
 * These are prototypes for _unguarded_ memory routines.  The caller must
 * validate all parameters prior to invocation.  Useful for diagnostics
 * and system initialization processing.
 */

/* moves (handles overlap) memory  */
extern void
mem_prim_move(void *dest, const void *src, uint32_t length);


/* uint8_t moves (handles overlap) memory */
extern void
mem_prim_move8(uint8_t *dest, const uint8_t *src, uint32_t length);

/* uint16_t moves (handles overlap) memory */
extern void
mem_prim_move16(uint16_t *dest, const uint16_t *src, uint32_t length);

/* uint32_t moves (handles overlap) memory */
extern void
mem_prim_move32(uint32_t *dest, const uint32_t *src, uint32_t length);


/* set bytes */
extern void
mem_prim_set(void *dest, uint32_t dmax, uint8_t value);

/* set uint16_ts */
extern void
mem_prim_set16(uint16_t *dest, uint32_t dmax, uint16_t value);

/* set uint32_ts */
extern void
mem_prim_set32(uint32_t *dest, uint32_t dmax, uint32_t value);


#endif  /* __MEM_PRIMITIVES_LIB_H__ */
