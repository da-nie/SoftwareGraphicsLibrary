#ifndef FIXED_MATH_H
#define FIXED_MATH_H

#include <math.h>
#include <stdint.h>

typedef int32_t fixed_t;

inline fixed_t int2fixed_t(int16_t x)
{
 return(((int32_t)x)<<16);
}
inline fixed_t float2fixed_t(float x)
{
 return((int32_t)(65536.0f*x));
}
inline int32_t fraction2fixed_t(int32_t a,int32_t b)
{
 return((int32_t)((((int32_t) a)<<16)/(int32_t)b));
}
inline int32_t fixed2Int(fixed_t x)
{
 return((int32_t)(x>>16));
}
inline float fixed2Float(fixed_t x)
{
 return(((float)x)/65536.0f);
}
inline fixed_t fixAbs(fixed_t x)
{
 if (x<0) return(-x);
 return(x);
}
inline fixed_t frac(fixed_t x)
{
 return(x&0xFFFFl);
}
#endif
