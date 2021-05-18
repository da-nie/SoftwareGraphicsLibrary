#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <math.h>
#include <stdint.h>

typedef int32_t fixed_t;

const float FIXED2FLOAT_COEFF = 1.0f / 65536.0f;

inline fixed_t int2fixed_t(int16_t x)
{
 return(((int32_t)x)<<16);
}
inline fixed_t float2fixed_t(float x)
{
 return((fixed_t)(65536.0f*x));
}
inline int32_t fraction2fixed_t(int32_t a,int32_t b)
{
 return((int32_t)((((int32_t)a)<<16)/(int32_t)b));
}
inline fixed_t fixdiv(fixed_t a,fixed_t b)
{
 return((a<<16)/b);
}
inline fixed_t fixmul(fixed_t a,fixed_t b)
{
 return((a>>16)*b);
}
inline int32_t fixed2int(fixed_t x)
{
 return((int32_t)(x>>16));
}
inline float fixed2float(fixed_t x)
{
 return(((float)x) * 65536.0f);
}
inline fixed_t fixabs(fixed_t x)
{
 if (x<0) return(-x);
 return(x);
}
inline fixed_t frac(fixed_t x)
{
 return(x&0xFFFFl);
}
#endif

