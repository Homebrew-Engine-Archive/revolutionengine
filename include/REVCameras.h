///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Cameras header file

#ifndef REV_CAMERAS_H
#define REV_CAMERAS_H
#include "REVQBsp.h"

class CAMERA:public NODE
{
public:
	f32 fov;//Field of view
	f32 ncd, fcd;//Near and far clipping distances
	CAMERA();
private:
	virtual void setForRender(Mtx m);
friend void REV_process(tMapQ3 * map);
friend class ROOT;
};

class HTCAMERA:public CAMERA
{
public:
	HTCAMERA(u8 channel = 0);
	~HTCAMERA();
	u8 channel;
private:
	NODE eye;
	f32 factor;
	void setForRender		(Mtx m);
	void updateEye			();
	HTCAMERA * nextHTC;
friend void REV_preProcess	();
};

extern CAMERA * defaultCam;

#endif
