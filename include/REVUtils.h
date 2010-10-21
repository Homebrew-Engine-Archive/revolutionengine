///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Main Header file
#ifndef REV_UTILS_H
#define REV_UTILS_H

//Type definitions
#define Vector guVector

//Common flags
#define F_Visible	1
#define F_Active	2
#define F_Center	4

//Reference types
#define RT_SELF		0
#define RT_ABS		1
#define RT_PAR		2

//Node and N2D types
#define NT_NODE		0
#define NT_CAM		1
#define NT_OBJECT	2
#define NT_HTCAM	3
#define NT_PANEL	1
#define NT_VIEWPORT	2
#define NT_TEXT		3
#define NT_BUTTON	4
#define NT_WINDOW	5

//Bounding body types
#define BD_SPHERE	0
#define BD_BOX		1
#define BD_INFPLANE	2
#define BD_CONVEX	9

//Light types
#define LT_DIR		0
#define LT_OMNI		1
#define LT_SPOT		2

//Model types
#define MDT_BAD		0
#define MDT_STATIC	1
#define MDT_ANIM	2

//Material types
#define MTT_BASIC	0
#define MTT_TERRAIN 1

//File Formats
#define FF_RMS		0
#define FF_RMD		1
#define FF_MD5		2

//Pointer modes
#define PM_OFF		0
#define PM_ON		1

//Button modes
#define BM_OFF		0
#define BM_ON		1

//Button states		
#define BS_RELEASED	0
#define BS_CLICKED	1
#define BS_HELD		2

//Color channels
#define CC_NONE		0
#define CC_DIFF		1
#define CC_SPEC		2

//Node physical mode
#define PX_PHANTOM	0//Phantom
#define PX_SOLID	1//Solid mode

//Macros
#define nullVector ((Vector){0.0f, 0.0f, 0.0f})
#define AXIS_X	((Vector){1.0f, 0.0f, 0.0f})
#define AXIS_Y	((Vector){0.0f, 1.0f, 0.0f})
#define AXIS_Z	((Vector){0.0f, 0.0f, 1.0f})
#define vector3(a, b, c) ((Vector){a, b, c})
#define vector2(x, y) ((Vector){x, y, 0.0f})
#define rgba(r, g, b, a) ((GXColor){r, g, b, a})
#define rgb(r, g, b) ((GXColor){r, g, b, 255})

//Colors
//	Solid colors
#define SC_WHITE	((GXColor){255, 255, 255, 255})
#define SC_RED		((GXColor){255, 0, 0, 255})
#define SC_GREEN	((GXColor){0, 255, 0, 255})
#define SC_BLUE		((GXColor){0, 0, 255, 255})
#define SC_BLACK	((GXColor){0, 0, 0, 255})
#define SC_GREY		((GXColor){128, 128, 128, 255})
#define SC_YELLOW   ((GXColor){255, 255, 0, 255})
#define SC_ORANGE	((GXColor){255, 128, 0, 255})
#define SC_PURPLE	((GXColor){128, 0, 255, 255})
#define SC_PINK     ((GXColor){255, 0, 128, 255})
#define SC_AQUA		((GXColor){0, 255, 255, 255})
//  Solid light colors
#define SC_LRED		((GXColor){255, 100, 100, 255})
#define SC_LGREEN	((GXColor){100, 255, 100, 255})
#define SC_LBLUE	((GXColor){80, 190, 255, 255})

typedef struct REV_Rectangle
{
	f32 x0,y0,sx,sy;
}RECTANGLE;

#define Rectangle4(a,b,c,d) ((RECTANGLE){a,b,c,d})

#endif
