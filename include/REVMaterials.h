///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Mateirals header file

#ifndef REV_MATERIALS_H
#define REV_MATERIALS_H

#include <map>

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

class TTevInfo
{
public:
	TTevInfo();
	void	lock		(u8 registries);
	void	unlock		(u8 registries);
	u8		getFreeReg	();//Returns the first unused registry, GX_TEVPREV is the last one
	u8 tevStage;
	u8 texMap;
	u8 texCoords;
	u8 outReg;
private:
	u8 m_lockedRegs;
};

class IMaterial
{
public:
	virtual bool	usesAlpha	()=0;//This material uses alpha?
	virtual void	setTev		(TTevInfo& info)=0;//Adjust texture enviroment hardware for render
};

class TTexture : public IMaterial
{
public:
	TTexture					(const bool alpha = true);
	TTexture					(const char * filename,const bool alpha = true);
	~TTexture					();
	virtual bool	usesAlpha	()								{ return m_alpha;		}

	GXTexObj	*	getTexture	()								{ return m_pGXTexture;	}
protected:
	virtual void	setTev		(TTevInfo& info);
	GXTexObj	*	m_pGXTexture;
	void		*	m_pData;//Buffer to store the render
private:
	bool	m_alpha;
};

class TRender2Texture: public TTexture
{
public:
	TRender2Texture	(CAMERA *const camera, const u16 size_x, const u16 size_y, const bool alpha = true);
	~TRender2Texture();

	void			setCamera			(CAMERA * pCamera);
	CAMERA *		getCamera			()			const		{ return m_pCamera; }
	void			resize				(u16 x, u16 y);

	std::multimap<CAMERA*, TRender2Texture*>::iterator getRender2Texture(TRender2Texture* _target);

protected:
	virtual void	setForRender		(Mtx44 m)	const;
private:
	void			copyTexture			();
protected:
	CAMERA	*	m_pCamera;
	u16			m_sx,
				m_sy;// Size in pixels
	friend void REV_process(tMapQ3*);
};

class TDiffuseMaterial : public IMaterial
{
public:
	TDiffuseMaterial			( IMaterial * baseMaterial );
	virtual bool	usesAlpha	()								{ return m_pBaseMaterial?m_pBaseMaterial->usesAlpha():false; }
	IMaterial	*	m_pBaseMaterial;
protected:
	virtual void	setTev		(TTevInfo& info);
};

GXTexObj * loadPng(const char * filename);

void tiling4x4(void *mem, int format, int tx, int ty);//Thanks hermes

#endif
