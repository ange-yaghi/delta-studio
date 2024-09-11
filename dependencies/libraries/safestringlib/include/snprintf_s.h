/*  SPDX-License-Identifier: MIT */
/*
 *  Copyright (c) 2014 by Intel Corp
 */

#ifndef SPRINTF_S_H_
#define SPRINTF_S_H_

#include <safe_lib_errno.h>


#define SNPRFNEGATE(x) (-1*(x))



int snprintf_s_i(char *dest, rsize_t dmax, const char *format, int a);
int snprintf_s_si(char *dest, rsize_t dmax, const char *format, char *s, int a);
int snprintf_s_l(char *dest, rsize_t dmax, const char *format, long a);
int snprintf_s_sl(char *dest, rsize_t dmax, const char *format, char *s, long a);



#endif /* SPRINTF_S_H_ */
