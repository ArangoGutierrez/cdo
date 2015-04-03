/*
  This file is part of CDO. CDO is a collection of Operators to
  manipulate and analyse Climate model Data.

  Copyright (C) 2003-2010 Uwe Schulzweida, Uwe.Schulzweida@zmaw.de
  See COPYING file for copying and redistribution conditions.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
*/

#ifndef _FIELD_H
#define _FIELD_H

#define  FIELD_NONE 0
#define  FIELD_ALL  1
#define  FIELD_PTR  2
#define  FILED_WGT  3


#define  FADD(x,y)  (DBL_IS_EQUAL((x),missval1) || DBL_IS_EQUAL((y),missval2) ? missval1 : (x)+(y))
#define  FSUB(x,y)  (DBL_IS_EQUAL((x),missval1) || DBL_IS_EQUAL((y),missval2) ? missval1 : (x)-(y))
#define  FMUL(x,y)  (DBL_IS_EQUAL((x),0.)||IS_EQUAL((y),0.) ? 0 : DBL_IS_EQUAL((x),missval1) || DBL_IS_EQUAL((y),missval2) ? missval1 : (x)*(y))
#define  FDIV(x,y)  (DBL_IS_EQUAL((x),missval1) || DBL_IS_EQUAL((y),missval2) || DBL_IS_EQUAL((y),0.) ? missval1 : (x)/(y))
#define  FPOW(x,y)  (DBL_IS_EQUAL((x),missval1) || DBL_IS_EQUAL((y),missval2) ? missval1 : pow((x),(y)))
#define  FSQRT(x)   (DBL_IS_EQUAL((x),missval1) || (x)<0 ? missval1 : sqrt(x))


double _FADD_(double x, double y, double missval1, double missval2);
double _FSUB_(double x, double y, double missval1, double missval2);
double _FMUL_(double x, double y, double missval1, double missval2);
double _FDIV_(double x, double y, double missval1, double missval2);
double _FPOW_(double x, double y, double missval1, double missval2);
double _FSQRT_(double x, double missval1);


#define ADD(x,y)  _FADD_(x, y, missval1, missval2)
#define SUB(x,y)  _FSUB_(x, y, missval1, missval2)
#define MUL(x,y)  _FMUL_(x, y, missval1, missval2)
#define DIV(x,y)  _FDIV_(x, y, missval1, missval2)
#define POW(x,y)  _FPOW_(x, y, missval1, missval2)
#define SQRT(x)   _FSQRT_(x, missval1)


typedef struct {
  int      grid;
  int      zaxis;
  int      size;
  int      nsamp;
  int      nmiss;
  double   missval;
  double  *weight;
  double  *ptr;
}
field_t;

/* fieldmem.c */

field_t **field_malloc(int vlistID, int ptype);
field_t **field_calloc(int vlistID, int ptype);
void      field_free(field_t **field, int vlistID);

/* field.c */

double fldfun(field_t field, int function);
double fldmin(field_t field);
double fldmax(field_t field);
double fldsum(field_t field);
double fldavg(field_t field);
double fldmean(field_t field);
double fldstd(field_t field);
double fldvar(field_t field);
/* RQ */
double fldpctl(field_t field, int k);
/* QR */
void   fldunm(field_t *field);
int    fldhvs(field_t *field, int nlevels);

/* ENS VALIDATION */
double fldcrps(field_t field);
double fldbrs(field_t field);
double fldrank(field_t field);
double fldroc(field_t field);

/* fieldzon.c */

void zonfun(field_t field1, field_t *field2, int function);
void zonmin(field_t field1, field_t *field2);
void zonmax(field_t field1, field_t *field2);
void zonsum(field_t field1, field_t *field2);
void zonavg(field_t field1, field_t *field2);
void zonmean(field_t field1, field_t *field2);
void zonstd(field_t field1, field_t *field2);
void zonvar(field_t field1, field_t *field2);
/* RQ */
void zonpctl(field_t field1, field_t *field2, int k);
/* QR */

/* fieldmer.c */

void merfun(field_t field1, field_t *field2, int function);
void mermin(field_t field1, field_t *field2);
void mermax(field_t field1, field_t *field2);
void mersum(field_t field1, field_t *field2);
void meravg(field_t field1, field_t *field2);
void mermean(field_t field1, field_t *field2);
void merstd(field_t field1, field_t *field2);
void mervar(field_t field1, field_t *field2);
/* RQ */
void merpctl(field_t field1, field_t *field2, int k);
/* QR */

void fldrms(field_t field1, field_t field2, field_t *field3);

void varrms(field_t field1, field_t field2, field_t *field3);

/* fieldc.c */

void farcfun(field_t *field, double rconst, int function);

void farcmul(field_t *field, double rconst);
void farcdiv(field_t *field, double rconst);
void farcadd(field_t *field, double rconst);
void farcsub(field_t *field, double rconst);

void farmod(field_t *field, double divisor);

void farinv(field_t *field);

/* field2.c */

void farfun(field_t *field1, field_t field2, int function);

void faradd(field_t *field1, field_t field2);
void farsum(field_t *field1, field_t field2);
void farsumq(field_t *field1, field_t field2);
void farsumtr(field_t *field1, field_t field2, double refval);
void farsub(field_t *field1, field_t field2);
void farmul(field_t *field1, field_t field2);
void fardiv(field_t *field1, field_t field2);
void farmin(field_t *field1, field_t field2);
void farmax(field_t *field1, field_t field2);
void farvar(field_t *field1, field_t field2, field_t field3);
void farstd(field_t *field1, field_t field2, field_t field3);
void farcvar(field_t *field1, field_t field2, double rconst1);
void farcstd(field_t *field1, field_t field2, double rconst1);
void farmoq(field_t *field1, field_t field2);
void faratan2(field_t *field1, field_t field2);

/* RQ */
void farcount(field_t *field1, field_t field2);
/* QR */

#endif  /* _FIELD_H */
