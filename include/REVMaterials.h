///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Mateirals header file

#ifndef REV_MATERIALS_H
#define REV_MATERIALS_H

//Hermes tiling function
#define TILE_LITTLE_ENDIAN 128 // change the byte order in the color
#define TILE_CI4       0 // for CI4  (color index of 4 bits)
#define TILE_CI8       1 // for CI8  (color index of 8 bits)
#define TILE_RGB5A3    2 // for RGB5A3 
#define TILE_RGB5A1    2 // pseudo RGB5A1
#define TILE_RGB565    3 // for RGB565 
#define TILE_SRGB565   4 // for RGB565 with R and B swapped
#define TILE_SRGB5A1   5 // for RGB5A1 with R and B swapped
#define TILE_RGBA8     6 // for RGBA8
#define TILE_SRGBA8    7 // for RGBA8 with R and B swapped

class MATERIAL
{
public:
	MATERIAL();
	f32 specularity;
	u8 flags;
protected:
	u8 type;
private:
	virtual void setTev(u8 nUVs, u8 * clrChannels, u8 * texChannels, u32 lightMask)=0;
friend void render(NODE * node, Vector camPos);
};

class BMATERIAL:public MATERIAL
{
public:
	BMATERIAL(TEXTURE * map);//Check for tex before render
	TEXTURE * map;
private:
	virtual void setTev(u8 nUVs, u8 * clrChannels, u8 * texChannels, u32 lightMask);
};

class TMATERIAL:public MATERIAL
{
};

GXTexObj * loadPng(const char * filename);

void tiling4x4(void *mem, int format, int tx, int ty);//Thanks hermes

#endif
