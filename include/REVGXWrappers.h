///////////////////////////////////
//Technik 2010
//Revolution Engine 0.6
///////////////////////////////////
//GX_WRAPPERS
#ifndef REV_GX_WRAPPERS_H
#define REV_GX_WRAPPERS_H

#include "REVExternal.h"
#include "REVConfig.h"

//Macros for debugging, use REVConfig to enable/disable them
#if REVGX_PRINT_VIDX
#define REVGX_Position1x16(idx)	_REVGX_Position1x16(idx)
#else
#define REVGX_Position1x16(idx)	GX_Position1x16(idx)
#endif

#if REVGX_PRINT_NIDX
#define REVGX_Normal1x16(idx)	_REVGX_Normal1x16(idx)
#else
#define REVGX_Normal1x16(idx)	GX_Normal1x16(idx)
#endif

void _REVGX_Position1x16(u16 index);

#endif
