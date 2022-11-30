#ifndef _FIXED_POINT_H
#define _FIXED_POINT_H

#include <stdint.h>

// 17.14 format (17 bits before, 14 bits after decimal point)
#define P 17
#define Q 14
#define F (1<<Q) // F = 2**Q

typedef int32_t fp_t;

fp_t INT_TO_FP(int n);
fp_t FP_TO_INT_FLOOR(fp_t x);
fp_t FP_TO_INT_NEAR(fp_t x);

fp_t FP_ADD(fp_t x, fp_t y);
fp_t FP_SUB(fp_t x, fp_t y);
fp_t FP_MUL(fp_t x, fp_t y);
fp_t FP_DIV(fp_t x, fp_t y);

fp_t FP_INT_ADD(fp_t x, int n);
fp_t INT_FP_SUB(int n, fp_t x);
fp_t FP_INT_MUL(fp_t x, int n);
fp_t FP_INT_DIV(fp_t x, int n);

#endif // _FIXED_POINT_H
