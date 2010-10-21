///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Skybox source file

#include "REV.h"

SKYBOX::SKYBOX()
{
	front = NULL;
	back = NULL;
	left = NULL;
	right = NULL;
	top = NULL;
	bottom = NULL;
}

void SKYBOX::render(CAMERA * cam)
{
	Mtx m, aux;
	GX_ClearVtxDesc();
	GX_SetCullMode(GX_CULL_NONE);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_SetTevOp(GX_TEVSTAGE0,GX_REPLACE);
	GX_SetNumTevStages(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	
	GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_NONE, GX_AF_NONE);
	GX_SetChanMatColor(GX_COLOR0A0, SC_LBLUE);
	GX_SetChanAmbColor(GX_COLOR0, SC_WHITE);
	
	
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	
	cam->absMtx(aux);
	guVector pos = nullVector,
			 look = vector3(aux[0][2], aux[1][2], aux[2][2]),
			 up = vector3(aux[0][1], aux[1][1], aux[2][1]);
	guLookAt(m, &pos, &look, & up);
	GX_LoadPosMtxImm(m, GX_PNMTX0);
	if(front)
	{
		GX_LoadTexObj(front, GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-1000.0f, 1000.0f, 1000.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
			
			GX_Position3f32(1000.0f, 1000.0f, 1000.0f);
			GX_TexCoord2f32(1.0f, 0.0f);
			
			GX_Position3f32(1000.0f,1000.0f, -1000.0f);
			GX_TexCoord2f32(1.0f, 1.0f);
			
			GX_Position3f32(-1000.0f, 1000.0f, -1000.0f);
			GX_TexCoord2f32(0.0f, 1.0f);
		GX_End();
	}
	if(left)
	{
		GX_LoadTexObj(left, GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-1000.0f, -1000.0f, 1000.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
			
			GX_Position3f32(-1000.0f, 1000.0f, 1000.0f);
			GX_TexCoord2f32(1.0f, 0.0f);
			
			GX_Position3f32(-1000.0f,1000.0f, -1000.0f);
			GX_TexCoord2f32(1.0f, 1.0f);
			
			GX_Position3f32(-1000.0f, -1000.0f, -1000.0f);
			GX_TexCoord2f32(0.0f, 1.0f);
		GX_End();
	}
	if(right)
	{
		GX_LoadTexObj(right, GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(1000.0f, 1000.0f, 1000.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
			
			GX_Position3f32(1000.0f, -1000.0f, 1000.0f);
			GX_TexCoord2f32(1.0f, 0.0f);
			
			GX_Position3f32(1000.0f,-1000.0f, -1000.0f);
			GX_TexCoord2f32(1.0f, 1.0f);
			
			GX_Position3f32(1000.0f, 1000.0f, -1000.0f);
			GX_TexCoord2f32(0.0f, 1.0f);
		GX_End();
	}
	if(back)
	{
		GX_LoadTexObj(back, GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(1000.0f, -1000.0f, 1000.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
			
			GX_Position3f32(-1000.0f, -1000.0f, 1000.0f);
			GX_TexCoord2f32(1.0f, 0.0f);
			
			GX_Position3f32(-1000.0f, -1000.0f, -1000.0f);
			GX_TexCoord2f32(1.0f, 1.0f);
			
			GX_Position3f32(1000.0f, -1000.0f, -1000.0f);
			GX_TexCoord2f32(0.0f, 1.0f);
		GX_End();
	}
	if(bottom)
	{
		GX_LoadTexObj(bottom, GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-1000.0f, 1000.0f, -1000.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
			
			GX_Position3f32(1000.0f, 1000.0f, -1000.0f);
			GX_TexCoord2f32(1.0f, 0.0f);
			
			GX_Position3f32(1000.0f, -1000.0f, -1000.0f);
			GX_TexCoord2f32(1.0f, 1.0f);
			
			GX_Position3f32(-1000.0f, -1000.0f, -1000.0f);
			GX_TexCoord2f32(0.0f, 1.0f);
		GX_End();
	}
	if(top)
	{
		GX_LoadTexObj(top, GX_TEXMAP0);
		GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
			GX_Position3f32(-1000.0f, -1000.0f, 1000.0f);
			GX_TexCoord2f32(0.0f, 0.0f);
			
			GX_Position3f32(1000.0f, -1000.0f, 1000.0f);
			GX_TexCoord2f32(1.0f, 0.0f);
			
			GX_Position3f32(1000.0f, 1000.0f, 1000.0f);
			GX_TexCoord2f32(1.0f, 1.0f);
			
			GX_Position3f32(-1000.0f, 1000.0f, 1000.0f);
			GX_TexCoord2f32(0.0f, 1.0f);
		GX_End();
	}
}
