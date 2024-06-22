/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2008 Bo Berry
 *  Copyright (c) 2008-2009 Cisco Systems
 *  Copyright (c) 2022 Intel Corp
 */

#ifndef __SAFE_MEM_CONSTRAINT_H__
#define __SAFE_MEM_CONSTRAINT_H__

#include "safeclib_private.h"

/*
 * Function used by the libraries to invoke the registered
 * runtime-constraint handler. Always needed.
 */
extern void invoke_safe_mem_constraint_handler(
                           const char *msg,
                           void *ptr,
                           errno_t error);

#endif /* __SAFE_MEM_CONSTRAINT_H__ */
