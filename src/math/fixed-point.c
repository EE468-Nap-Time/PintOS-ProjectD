#include "fixed-point.h"

fp_t INT_TO_FP(int n)
{
    return n * F;
}

fp_t FP_TO_INT_FLOOR(fp_t x)
{
    return x / F;
}

fp_t FP_TO_INT_NEAR(fp_t x)
{
    if (x >= 0)
        return (x + F / 2) / F;

    return (x - F / 2) / F;
}

fp_t FP_ADD(fp_t x, fp_t y)
{
    return x + y;
}

fp_t FP_SUB(fp_t x, fp_t y)
{
    return x - y;
}

fp_t FP_MUL(fp_t x, fp_t y)
{
    return ((int64_t)x) * y / F;
}

fp_t FP_DIV(fp_t x, fp_t y)
{
    return ((int64_t)x) * F / y;
}

fp_t FP_INT_ADD(fp_t x, int n)
{
    return x + n * F;
}

fp_t INT_FP_SUB(int n, fp_t x)
{
    return x - n * F;
}

fp_t FP_INT_MUL(fp_t x, int n)
{
    return x * n;
}

fp_t FP_INT_DIV(fp_t x, int n)
{
    return x / n;
}
