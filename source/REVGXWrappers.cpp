///////////////////////////////////
//Technik 2010
//Revolution Engine 0.6
///////////////////////////////////
//GX_WRAPPERS
#include "REVGXWrappers.h"
#include "REV.h"

void _REVGX_Position1x16(u16 index)
{
#if REVGX_PRINT_VIDX > 0
	static unsigned count = 0;
	if(count < REVGX_PRINT_VIDX)
	{
#endif
		char msg[16];
		int val = (int)index;//safety cast
		sprintf(msg, "vi=%d", val);
		REVConsole->write(msg);
#if REVGX_PRINT_VIDX > 0
		count++;
	}
#endif
	GX_Position1x16(index);
}

void _REVGX_Normal1x16(u16 index)
{
#if REVGX_PRINT_NIDX > 0
	static unsigned count = 0;
	if(count < REVGX_PRINT_NIDX)
	{
#endif
		char msg[16];
		int val = (int)index;//safety cast
		sprintf(msg, "ni=%d", val);
		REVConsole->write(msg);
#if REVGX_PRINT_NIDX > 0
		count++;
	}
#endif
	GX_Normal1x16(index);
}