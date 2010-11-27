///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Configuration Header file
/*
Change work configuration of the engine here
*/
#ifndef REV_CONFIG_H
#define REV_CONFIG_H

//Basic parameters
#define DEF_BG_COLOR				((GXColor){255, 255, 255, 0xff}) //Default Background Color
#define DEFAULT_FIFO_SIZE			(256*1024)//Graphics commands fifo size
#define DEFAULT_FIELD_OF_VIEW		45//Default viewport field of view
#define NEAR_CLIP_DISTANCE			0.1f//Default near clip distance
#define FAR_CLIP_DISTANCE			1000000.0f//Default far clip distance
#define DEFAULT_OBJECT_COLOR		((GXColor){255, 255, 255, 0xff})//Color of new Objects
#define DEFAULT_AMBIENT_LIGHT		((GXColor){10,12,16,255})
#define DEFAULT_HTCAM_FACTOR		1.0f
#define PNL_MAX_LAYER				100.0f

//texts and panels
#define TEXT_DEFAULT_LENGTH			32
#define MAX_PATH					128

//Definitions that control debugging helpers for GX
/*
* 0 means no printing,
* > 0 means how many indices you want to print
* -1 means print all calls
*/
#define REVGX_PRINT_VIDX			12//Print vertex idx to console
#define REVGX_PRINT_NIDX			0//Print normal idx to console
#define REVGX_PRINT_TIDX			0//Print texture idx to console

#endif
