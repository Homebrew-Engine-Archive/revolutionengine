///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Textures source file

#include "REV.h"
#define screenHeightMM	30.0f

//Class methods
TEXTURE::TEXTURE()
{
	tex = NULL;
	data = NULL;
	guMtxIdentity(mtx);
}

TEXTURE::~TEXTURE()
{
	//Release some memory
	if(data) free(data);
	if(tex) free(tex);
}

void TEXTURE::setTevStage(texRenderData * data)
{
	GX_LoadTexMtxImm(mtx, data->texMtx, GX_MTX3x4);
	GX_SetTexCoordGen(data->dstCoords, GX_TG_MTX3x4, data->srcCoords, data->texMtx);
	GX_LoadTexObj(tex, data->texMap);
	//Update data for other textures tu use it
}

R2TEXTURE::R2TEXTURE(CAMERA * cam, f32 sx, f32 sy)
{
	this->sx = sx;
	this->sy = sy;
	//Allocate memory for the texture
	data = memalign(32, GX_GetTexBufferSize(sx, sy, GX_TF_RGBA8, GX_FALSE,0));
	tex = (GXTexObj*)memalign(32, sizeof(GXTexObj));
	this->cam = cam;
	next = mainRoot->fstTex;
	mainRoot->fstTex = this;
}

void R2TEXTURE::setSize(f32 sx, f32 sy)
{
	if(this->sx * this-> sy != sx*sy)
	{
		free(data);
		data = memalign(32, GX_GetTexBufferSize(sx, sy, GX_TF_RGBA8, GX_FALSE,0));
	}
	this->sx = sx;
	this->sy = sy;
}

void R2TEXTURE::setForRender(Mtx44 m)
{
	if(cam->type == NT_CAM)
		guPerspective(m, cam->fov, sx/sy, cam->ncd, cam->fcd);
	if(cam->type == NT_HTCAM)
	{
		Vector v = cam->getPos(); 
		guFrustum(m, -screenHeightMM*cam->ncd*(0.5 * screenHeightMM* sx/sy + v.x)/v.y
					, -screenHeightMM*cam->ncd*(-0.5f * screenHeightMM*sx/sy + v.x)/v.y
					, screenHeightMM*cam->ncd*(0.5f + v.z)/v.y
					, screenHeightMM*cam->ncd*(0.5f + v.z)/v.y, cam->ncd, cam->fcd);
	}
	GX_LoadProjectionMtx(m, GX_PERSPECTIVE);
	GX_SetViewport(0,0,sx,sy,0.0f,1.0f);
	GX_SetScissor(0,0,sx,sy);
}

void R2TEXTURE::copyTexture()
{
	GX_SetCopyFilter(GX_FALSE, NULL, GX_FALSE, NULL);
	GX_SetTexCopySrc(0, 0, sx, sy);
	GX_SetTexCopyDst(sx, sy, GX_TF_RGBA8, GX_FALSE);
	GX_CopyTex(data, GX_TRUE);
	GX_PixModeSync();
	//
	GX_InitTexObj (tex, data, sx, sy, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
}

//Public functions
TEXTURE * loadTexture(const char * filename)
{
	//Check if the file exists at all
	FILE * temp = fopen(filename, "rb");
	if(!temp)
		return NULL;
	else fclose(temp);
	//Allocate the texObj
	GXTexObj * map = (GXTexObj*)memalign(32, sizeof(GXTexObj));
	PNGUPROP imgProp;
	IMGCTX ctx = PNGU_SelectImageFromDevice(filename);
	PNGU_GetImageProperties(ctx,&imgProp);
	//Allocate data
	void * data = memalign(32, imgProp.imgWidth * imgProp.imgHeight * 4);
	PNGU_DecodeTo4x4RGBA8 (ctx, imgProp.imgWidth, imgProp.imgHeight, data, 255);
	PNGU_ReleaseImageContext (ctx);
	DCFlushRange (data, imgProp.imgWidth * imgProp.imgHeight * 4);
	GX_InitTexObj (map, data, imgProp.imgWidth, imgProp.imgHeight, GX_TF_RGBA8, GX_MIRROR, GX_MIRROR, GX_FALSE);
	TEXTURE * tex = new TEXTURE();
	tex->tex = map;
	tex->data = data;
	return tex;
}