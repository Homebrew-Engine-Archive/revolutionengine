///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Textures header file

#ifndef REV_TEXTURES_H
#define REV_TEXTURES_H

typedef struct _texRenderData
{
	u8 texMap;
	u8 texMtx;
	u8 srcCoords;
	u8 dstCoords;
}texRenderData;

class TEXTURE
{
public:
	TEXTURE();
	~TEXTURE();
	GXTexObj * tex;
	void * data;
	virtual void setTevStage(texRenderData * data);
	Mtx mtx;
};

class R2TEXTURE:public TEXTURE
{
public:
	R2TEXTURE(CAMERA * cam, f32 sx, f32 sy);
	CAMERA * cam;
	void setSize(f32 sx, f32 sy);
private:
	f32 sx, sy;
	void setForRender(Mtx44 m);
	void copyTexture();
	R2TEXTURE * next;
friend class VIEWPORT;
friend void REV_process(tMapQ3 * map);
};

TEXTURE * loadTexture(const char * filename);

//Video,animated,procedural

#endif