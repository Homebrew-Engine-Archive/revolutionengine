///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Materials source file

#include "REV.h"

#include <map>

using namespace std;

//------------------------------------------------------------
TTevInfo::TTevInfo()
{
	tevStage	= GX_TEVSTAGE0;
	texMap		= GX_TEXMAP0;
	texCoords	= GX_TEXCOORD0;
	outReg		= GX_TEVPREV;
	m_lockedRegs	= 0;//No locked registries
}
//------------------------------------------------------------
void TTevInfo::lock(u8 _registries)
{
	//There should be debug code here to detect when yo try to lock an alredy locked registry
	m_lockedRegs |= (_registries << 1);
}

//------------------------------------------------------------
void TTevInfo::unlock(u8 _registries)
{
	m_lockedRegs &= ~(_registries << 1);
}

//------------------------------------------------------------
u8 TTevInfo::getFreeReg()
{
	if (!(m_lockedRegs & (GX_TEVREG0 << 1))) return GX_TEVREG0;
	if (!(m_lockedRegs & (GX_TEVREG1 << 1))) return GX_TEVREG1;
	if (!(m_lockedRegs & (GX_TEVREG2 << 1))) return GX_TEVREG2;
	if (!(m_lockedRegs & (GX_TEVPREV << 1))) return GX_TEVPREV;
	return 0xff;//Invalid
}

//------------------------------------------------------------
TTexture::TTexture(const bool _alpha)
{
	m_alpha = _alpha;
	m_pData = NULL;
	m_pGXTexture = NULL;
}

//------------------------------------------------------------
TTexture::TTexture(const char * _filename, const bool _alpha)
{
	//Allocate the texObj
	m_pGXTexture = (GXTexObj*)memalign(32, sizeof(GXTexObj));
	PNGUPROP imgProp;
	IMGCTX ctx = PNGU_SelectImageFromDevice(_filename);
	PNGU_GetImageProperties(ctx,&imgProp);
	//Allocate data
	m_pData = memalign(32, imgProp.imgWidth * imgProp.imgHeight * 4);
	PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, m_pData, 255);
	PNGU_ReleaseImageContext (ctx);
	DCFlushRange (m_pData, imgProp.imgWidth * imgProp.imgHeight * 4);
	GX_InitTexObj (m_pGXTexture, m_pData, imgProp.imgWidth, imgProp.imgHeight, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	
	m_alpha = _alpha;
}

//------------------------------------------------------------
TTexture::~TTexture()
{
	free(m_pData);
	free(m_pGXTexture);
}

//------------------------------------------------------------
void TTexture::setTev(TTevInfo& _info)
{
	if(!m_pGXTexture) return;
	//TODO: support coordinate generation
	//Load the map
	if(m_pGXTexture)
	GX_LoadTexObj(m_pGXTexture, GX_TEXMAP0);
	GX_SetTevColorIn(_info.tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GX_SetTevColorOp(_info.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, _info.outReg);
	if(m_alpha)
	{
		GX_SetTevAlphaIn(_info.tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GX_SetTevAlphaOp(_info.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, _info.outReg);
	}
	else
	{
		GX_SetTevAlphaIn(_info.tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
		GX_SetTevAlphaOp(_info.tevStage, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_SCALE_2, GX_ENABLE, _info.outReg);
	}
	GX_SetTevOrder(_info.tevStage, _info.texCoords, _info.texMap, GX_COLORNULL);
	++_info.tevStage;
	++_info.texMap;
	++_info.texCoords;
}

//------------------------------------------------------------
TRender2Texture::TRender2Texture(CAMERA *const _pCamera, const u16 _sx, const u16 _sy, const bool _alpha):TTexture(_alpha)
{
	m_pCamera = _pCamera;
	m_sx = _sx;
	m_sy = _sy;
	//Allocate memory for the buffer
	m_pData = memalign(32, sizeof(GXColor)*m_sx*m_sy);
	//Create the texture object
	m_pGXTexture = (GXTexObj*)memalign(32, sizeof(GXTexObj));//Probably we can do the Init here also
	//Register into main Root
	mainRoot->m_Render2Textures.insert(pair<CAMERA*, TRender2Texture*>(_pCamera, this));
}

//------------------------------------------------------------
////This method should be moved to ROOT
multimap<CAMERA*, TRender2Texture*>::iterator TRender2Texture::getRender2Texture(TRender2Texture* _target)
{
	//Get the whole range of Render2Texture that use this camera
	CAMERA * pCamera = _target->getCamera();
	pair<multimap<CAMERA*, TRender2Texture*>::iterator, multimap<CAMERA*, TRender2Texture*>::iterator> range =
		mainRoot->m_Render2Textures.equal_range(pCamera);
	//Find the exact iterator
	multimap<CAMERA*, TRender2Texture*>::iterator iter = range.first;
	for(; (*iter).first == pCamera; ++iter)
	{
		if((*iter).second == _target)
			return iter;
	}
	return mainRoot->m_Render2Textures.end();//Not found
}

//------------------------------------------------------------
TRender2Texture::~TRender2Texture()
{
	mainRoot->m_Render2Textures.erase(getRender2Texture(this));
	free(m_pGXTexture);
}

//------------------------------------------------------------
void TRender2Texture::setCamera(CAMERA * _pCamera)
{
	//Remove the old iterator in the general map that pointed to this cam
	mainRoot->m_Render2Textures.erase(getRender2Texture(this));
	//And add the new one
	m_pCamera = _pCamera;
	//TODO: register in the map
	mainRoot->m_Render2Textures.insert(pair<CAMERA*, TRender2Texture*>(_pCamera, this));
}

//------------------------------------------------------------
void TRender2Texture::resize(u16 _sx, u16 _sy)
{
	//Free the old buffer
	if(m_pData) free(m_pData);
	//And allocate a new one
	m_sx = _sx;
	m_sy = _sy;
	m_pData = memalign(32, sizeof(GXColor)*m_sx*m_sy);
}

//------------------------------------------------------------
void TRender2Texture::setForRender(Mtx44 _proyection) const
{
	if(m_pCamera)
	{
		if(m_pCamera->getType() == NT_CAM)
			guPerspective(_proyection, m_pCamera->fov, m_sx/m_sy, m_pCamera->ncd, m_pCamera->fcd);
		if(m_pCamera->getType() == NT_HTCAM)
		{

			Vector v = m_pCamera->getPos(); 
			//TODO:: support Head-tracking cameras
		}
		GX_LoadProjectionMtx(_proyection, GX_PERSPECTIVE);
		GX_SetViewport(0,0,m_sx,m_sy,0.0f,1.0f);
		GX_SetScissor(0,0,m_sx,m_sy);
	}
}

//------------------------------------------------------------
void TRender2Texture::copyTexture()
{
	GX_SetCopyFilter(GX_FALSE, NULL, GX_FALSE, NULL);
	GX_SetTexCopySrc(0, 0, m_sx, m_sy);
	GX_SetTexCopyDst(m_sx, m_sy, GX_TF_RGBA8, GX_FALSE);
	GX_CopyTex(m_pData, GX_TRUE);
	GX_PixModeSync();
	//Init texture
	GX_InitTexObj(m_pGXTexture, m_pData, m_sx, m_sy, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
}

//------------------------------------------------------------
TDiffuseMaterial::TDiffuseMaterial(IMaterial * _baseMaterial)
{
	m_pBaseMaterial = _baseMaterial;
}

//------------------------------------------------------------
void TDiffuseMaterial::setTev(TTevInfo& _info)
{
}

//------------------------------------------------------------
GXTexObj * loadPng(const char * filename)
{
	FILE * temp = fopen(filename, "rb");
	if(!temp)
		return NULL;
	else fclose(temp);
	GXTexObj * map = (GXTexObj*)memalign(32, sizeof(GXTexObj));
	PNGUPROP imgProp;
	IMGCTX ctx = PNGU_SelectImageFromDevice(filename);
	PNGU_GetImageProperties(ctx,&imgProp);
	void * data = memalign(32, imgProp.imgWidth * imgProp.imgHeight * 4);
	PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, data, 255);
	PNGU_ReleaseImageContext (ctx);
	DCFlushRange (data, imgProp.imgWidth * imgProp.imgHeight * 4);
	GX_InitTexObj (map, data, imgProp.imgWidth, imgProp.imgHeight, GX_TF_RGBA8, GX_MIRROR, GX_MIRROR, GX_FALSE);
	return map;//And return the texture
}

void tiling4x4(void *mem,int format,int tx,int ty)
{
int n,m,l;
u16 mem_tile[1024*8];
int swap=(format & TILE_LITTLE_ENDIAN)!=0;

format &= ~TILE_LITTLE_ENDIAN;

#define INV_COLORA16(x) ((x & 0x83e0) | ((x & 31)<<10) | ((x>>10) & 31))
#define INV_COLOR16(x) ((x & 0x7e0) | ((x & 31)<<11) | ((x>>11) & 31))
#define SWAP16(x) (((x)>>8) | ((x)<<8))

if(format==TILE_CI4)
	{
	u8 *p1,*p2;
	
	p1=(u8 *) mem;
    p2=(u8 *) &mem_tile[0];
    tx>>=1;
    for(n=0;n<ty;n+=8)
		{
		for(l=0;l<8;l++)
			{
			for(m=0;m<tx;m+=4)
				{
				p2[((l+(m<<1))<<2)]=p1[(n+l)*tx+m];
				p2[((l+(m<<1))<<2)+1]=p1[(n+l)*tx+m+1];
				p2[((l+(m<<1))<<2)+2]=p1[(n+l)*tx+m+2];
				p2[((l+(m<<1))<<2)+3]=p1[(n+l)*tx+m+3];
				}
			}
		for(l=0;l<8;l++)
			{
			for(m=0;m<tx;m++)
				{
				p1[(n+l)*tx+m]=p2[(l)*tx+m];
				}
			}
		}

	DCFlushRange(mem,tx*ty);
	return;
	}
            
if(format==TILE_CI8)
	{
	u16 *p1,*p2;
	
	p1=(u16 *) mem;
    p2=(u16 *) &mem_tile[0];
    tx>>=1;
    for(n=0;n<ty;n+=4)
		{
		for(l=0;l<4;l++)
			{
			for(m=0;m<tx;m+=4)
				{
				p2[((l+m)<<2)]=p1[(n+l)*tx+m];
				p2[((l+m)<<2)+1]=p1[(n+l)*tx+m+1];
				p2[((l+m)<<2)+2]=p1[(n+l)*tx+m+2];
				p2[((l+m)<<2)+3]=p1[(n+l)*tx+m+3];
				}
			}
		for(l=0;l<4;l++)
			{
			for(m=0;m<tx;m++)
				p1[(n+l)*tx+m]=p2[(l)*tx+m];
			}
		}

	DCFlushRange(mem,tx*2*ty);
	return;
	}

if(format<TILE_RGBA8) // color 16 bits
	{
	u16 *p1,*p2;
	
	p1=(u16 *) mem;
    p2=(u16 *) &mem_tile[0];

    for(n=0;n<ty;n+=4)
		{
		for(l=0;l<4;l++)
			{
			for(m=0;m<tx;m+=4)
				{
				p2[((l+m)<<2)]=p1[(n+l)*tx+m];
				p2[((l+m)<<2)+1]=p1[(n+l)*tx+m+1];
				p2[((l+m)<<2)+2]=p1[(n+l)*tx+m+2];
				p2[((l+m)<<2)+3]=p1[(n+l)*tx+m+3];
				}
			}
	if(format==TILE_RGB5A3 || format==TILE_RGB565)
		{
		for(l=0;l<4;l++)
			{
			if(swap) // for little endian to big endian order
			  for(m=0;m<tx;m++) p1[(n+l)*tx+m]= SWAP16(p2[(l)*tx+m]);
			else
			  for(m=0;m<tx;m++) p1[(n+l)*tx+m]=p2[(l)*tx+m];
	
			}
		}
	else
	if(format==TILE_SRGB565)
		{
		for(l=0;l<4;l++)
			{
			if(swap)  // for little endian to big endian order
			  for(m=0;m<tx;m++) p1[(n+l)*tx+m]=INV_COLOR16(SWAP16(p2[(l)*tx+m]));
		    else
			  for(m=0;m<tx;m++) p1[(n+l)*tx+m]=INV_COLOR16(p2[(l)*tx+m]);
			
			}
		}
	else
	if(format==TILE_SRGB5A1)
		{
		for(l=0;l<4;l++)
			{
			if(swap)  // for little endian to big endian order
				for(m=0;m<tx;m++) p1[(n+l)*tx+m]=INV_COLORA16(SWAP16(p2[(l)*tx+m]));  // color 16 bits (With R and B swap)
			else
			    for(m=0;m<tx;m++) p1[(n+l)*tx+m]=INV_COLORA16(p2[(l)*tx+m]);  // color 16 bits (With R and B swap)
				
			}
		}
		
		}
	}
if(format==TILE_RGBA8 || format==TILE_SRGBA8)
	{
	u16 *p1,*p2;
	u32 *p32;
	int tx2=tx*2;

	p1=(u16 *) mem;
    p2=(u16 *) &mem_tile[0];
    p32=(u32 *) p1;
    
	if(format==TILE_SRGBA8) // swap R and B
		{
		for(n=0;n<ty;n++)
		for(m=0;m<tx;m++)
			{
			u32 rgba;
		
			rgba=p32[n*tx+m];

			if(swap)  // for little endian to big endian order
				rgba=(rgba<<24) | ((rgba<<8) & 0xff0000) | ((rgba>>8) & 0xff00) | ((rgba>>24) & 0xff0000);
		
			rgba=(rgba & 0xff00ff00) | ((rgba>>16) & 0xff)  | ((rgba<<16) & 0xff0000);

			p32[n*tx+m]=rgba;

			}
		}

    for(n=0;n<ty;n+=4)
		{
		for(l=0;l<4;l++)
			{
		
			for(m=0;m<tx;m+=4)
				{
				int m2=m<<1;
				p2[((l+m2)<<2)]=p1[(((n+l)*tx+m)<<1)];
				p2[((l+m2)<<2)+1]=p1[(((n+l)*tx+m+1)<<1)];
				p2[((l+m2)<<2)+2]=p1[(((n+l)*tx+m+2)<<1)];
				p2[((l+m2)<<2)+3]=p1[(((n+l)*tx+m+3)<<1)];
               
                p2[((l+m2)<<2)+16]=p1[(((n+l)*tx+m)<<1)+1];
				p2[((l+m2)<<2)+17]=p1[(((n+l)*tx+m+1)<<1)+1];
				p2[((l+m2)<<2)+18]=p1[(((n+l)*tx+m+2)<<1)+1];
				p2[((l+m2)<<2)+19]=p1[(((n+l)*tx+m+3)<<1)+1];
				
				}
			}
	
		for(l=0;l<4;l++)
			{
			for(m=0;m<tx2;m++)
				{
				p1[(n+l)*tx2+m]=p2[(l)*tx2+m];
				}
			}
		
		}
	}

#undef INV_COLOR16
#undef INV_COLORA16
#undef SWAP16

if(format<TILE_RGBA8)
    DCFlushRange(mem,tx*ty*2);
else 
	DCFlushRange(mem,tx*ty*4);

}
