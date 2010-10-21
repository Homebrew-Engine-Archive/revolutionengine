///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Skybox header file

#ifndef REV_SKYBOX_H
#define REV_SKYBOX_H

class SKYBOX
{
public:
	SKYBOX();
	GXTexObj * front, *back,
		*left, *right,
		*top, *bottom;
private:
	void render(CAMERA * cam);
friend void REV_process(tMapQ3 * map);
};

#endif
