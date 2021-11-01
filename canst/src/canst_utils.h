/* date = March 21st 2021 4:49 pm */

#ifndef LANGUAGE_LAYER_H
#define LANGUAGE_LAYER_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // NOTE(ajeej): Temp
#include <math.h>   // NOTE(ajeej): Temp

//~ Memory
#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet memset
#define Bytes(n) (n)
#define Kilobytes(n) (n << 10)
#define Megabytes(n) (n << 20)
#define Gigabytes(n) (n << 30)
#define Terabytes(n) (n << 40)

//~ Math
#define Mod fmodf
#define AbsoluteValue fabsf
#define Sqrt sqrtf
#define Sin sinf
#define Cos cosf
#define Tan tanf
#define PI 3.1415926535897f
#define EPSILON 1e-5f

//~ String
#define StringLength (u32)strlen
#define StringToI32(str) ((i32)atoi(str))
#define StringToI16(str) ((i16)atoi(str))
#define StringToF32(str) ((f32)atoi(str))

//~ Static Defines
#define global static
#define internal static
#define local_persist static

//~ Types
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t b8;
typedef int16_t b16;
typedef int32_t b32;
typedef int64_t b64;
typedef float f32;
typedef double f64;

//~ Utility Macros
#define ArrayCount(x) (sizeof(x) / sizeof((x)[0]))
// TODO(ajeej): Implement asserts



#endif //LANGUAGE_LAYER_H
