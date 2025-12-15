#pragma once
#include "stdlib.h"
#include <string.h>
typedef char int8;
typedef signed char sint8;
typedef unsigned char uint8;
typedef short int16;
typedef signed short sint16;
typedef unsigned short uint16;
typedef int int32;
typedef signed int sint32;
typedef unsigned int uint32;


#define SIZEOF_UNSIGNED_INT 4
typedef unsigned char Bit8u;
typedef signed char Bit8s;

#if SIZEOF_UNSIGNED_INT == 4
typedef unsigned int Bit32u;
typedef signed int Bit32s;
#elif SIZEOF_UNSIGNED_LONG == 4
typedef unsigned long Bit32u;
typedef signed long Bit32s;
#endif
