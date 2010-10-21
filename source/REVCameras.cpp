///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Cameras source file
#include "REV.h"

#define WM_RAD_PER_PIXEL M_PI*0.25f/104.0f
#define dotDistance		24.5f


CAMERA * defaultCam;

CAMERA::CAMERA()
{
	type = NT_CAM;
	fov = DEFAULT_FIELD_OF_VIEW;
	ncd = NEAR_CLIP_DISTANCE;
	fcd = FAR_CLIP_DISTANCE;
}

void CAMERA::setForRender(Mtx m)
{
	Mtx aux;
	absMtx(aux);
	Vector pos = vector3(aux[0][3], aux[1][3], aux[2][3]),
		  look = vector3(aux[0][2], aux[1][2], aux[2][2]),
		    up = vector3(aux[0][1]+aux[0][3], aux[1][1]+aux[1][3], aux[2][1]+aux[2][3]);
	guLookAt(m, &pos, &look, &up);
}

HTCAMERA::HTCAMERA(u8 channel)
{
	this->channel = channel;
	eye.attach(this);
	factor = DEFAULT_HTCAM_FACTOR;
	eye.setPos((Vector){0.0f, -factor, 0.0f}, RT_PAR);
	nextHTC = mainRoot->fstHTC;
	mainRoot->fstHTC = this;
	WPAD_SetDataFormat(channel, WPAD_FMT_BTNS_ACC_IR);
}

HTCAMERA::~HTCAMERA()
{
	HTCAMERA * aux = mainRoot->fstHTC;
	if(aux == this)
	{
		mainRoot->fstHTC = nextHTC;
	}
	else
	{
		while(aux->nextHTC != this)
		{
			aux = aux->nextHTC;
		}
		aux->nextHTC = nextHTC;
	}
}

void HTCAMERA::setForRender(Mtx m)
{
	Mtx aux;
	eye.absMtx(aux);
	Vector pos = vector3(aux[0][3], aux[1][3], aux[2][3]),
		  look = vector3(aux[0][2], aux[1][2], aux[2][2]),
		    up = vector3(aux[0][1]+aux[0][3], aux[1][1]+aux[1][3], aux[2][1]+aux[2][3]);
	guLookAt(m, &pos, &look, &up);
}

void HTCAMERA::updateEye()
{
	if(wmData[channel]->ir.num_dots == 2)
	{
		f32 dx = wmData[channel]->ir.dot[0].rx - wmData[channel]->ir.dot[1].rx;
		f32 dy = wmData[channel]->ir.dot[0].ry - wmData[channel]->ir.dot[1].ry;
		f32 pointDst = sqrt(dx*dx + dy*dy);
		
		f32 angle = WM_RAD_PER_PIXEL * pointDst * 0.5f;
		
		f32 headDst = factor * ((dotDistance * 0.5f) / tanf(angle));//In centimeters
		
		f32 avx = (wmData[channel]->ir.dot[0].rx + wmData[channel]->ir.dot[1].rx) * 0.5f;
		f32 avy = (wmData[channel]->ir.dot[0].ry + wmData[channel]->ir.dot[1].ry) * 0.5f;
		
		f32 angX = WM_RAD_PER_PIXEL * (avx - 512);
		f32 headX = (f32)(factor *  sinf(angX) * headDst);
		//wiimote above screen
		f32 angY = (avy - 384) * WM_RAD_PER_PIXEL;
		f32 headY = 0.5f+(f32)(factor * sinf(angY) * headDst);
		/////////
		//headDst *= cosf(angX) * cosf(angY);
		eye.setPos((Vector){headX, -headDst, headY});
		//eye.lookAt((Vector){headX, 0, headY});
	}
}
