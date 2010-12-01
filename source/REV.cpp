///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Main source file, contains the workflow of the engine

//header files
#include "REV.h"

//Internal structures
typedef struct _tracker
{
	OBJECT * target;
	struct _tracker * next;
	f32 z;
}TRACKER;

//Global variables
CONSOLE * REVConsole;
SCRIPTMGR * ScriptMgr;
//REVScriptManager System;
GXRModeObj *rMode;//GX render mode
void * frameBuffer[2] = { NULL, NULL};//To store external framebuffers, double buffering
u8 fb = 0;//Frame buffer index
Mtx view, modelM, modelview, lightProj, sdwMtx, stgm;
Mtx44 perspective;
WPADData * wmData[4];
u16 w, h;
ROOT * mainRoot;//Root of the 3D space;
TRACKER * transQueue;
TRACKER * solidQueue;
TRACKER * zQueue;

//Private prototypes
void setUp3D(void);
void setUp2D(void);
void init3dConfig(void);
void init2dConfig(void);

//Public functions
void ROOT::enableShadows(LIGHT * l, SDW_PARAMS shadow)
{
	this->shadowCaster = l;
	s = shadow;
	this->stencilBuffer = memalign(32, GX_GetTexBufferSize(256, 256, GX_TF_I8, GX_FALSE,0));
}

void ROOT::shadowScene(void)
{
	//Viewport and perspective
	/*GX_SetScissor(0,0,256,256);
	GX_SetViewport(0,0,256,256,0.0,1.0f);
	guOrtho(perspective,-100,100,-100,100,0.1f,1000);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);
	//Light position
	Vector pos = vector3(100,0,100);
	Vector look = nullVector;
	Vector up = vector3(-1,0,1);
	guVecNormalize(&up);
	guLookAt(view,&pos,&up,&look);
	//Ortho light
	guLightOrtho(
            lightProj,
            -100, 
            100,
            -100,
            100,
            0.5F,
            0.5F,
            0.5F,
            0.5F );
	guMtxConcat(lightProj, view, sdwMtx);
	//Adjust TEV and blending
	GX_SetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_PixModeSync();
	GX_SetZCompLoc(GX_FALSE);
	GX_SetNumChans(1); 
	GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE, GX_SRC_REG, GX_SRC_REG, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE );
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevColorIn(GX_TEVSTAGE0,GX_CC_ZERO, GX_CC_ZERO , GX_CC_ZERO,GX_CC_C0 ); // dejamos pasar el Color del rasterizador
	GX_SetTevAlphaIn(GX_TEVSTAGE0,GX_CA_ZERO,GX_CA_ZERO,GX_CA_ZERO,GX_CA_A0);     // dejamos pasar el Alfa del reasterizador
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV );
	GX_SetTevAlphaOp(GX_TEVSTAGE0,GX_TEV_ADD,GX_TB_ZERO,GX_CS_SCALE_1,GX_TRUE,GX_TEVPREV);
    GX_SetNumTevStages(1);
	GX_SetNumTexGens(1);
	TRACKER * aux = transQueue;
	GXColor idc;
	u8 id = 100;
	while(aux)
	{
		if(aux->target->m_model)
		{
			idc = rgba(id,id,id,255);
			GX_SetTevColor(GX_TEVREG0,idc);
			aux->target->absMtx(modelM);
			guMtxConcat(view, modelM, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			aux->target->m_model->shadowRender();
			id+=50;
		}
		aux = aux->next;
	}
	aux = solidQueue;
	while(aux)
	{
		if(aux->target->model)
		{
			idc = rgba(id,id,id,255);
			GX_SetTevColor(GX_TEVREG0,idc);
			aux->target->absMtx(modelM);
			guMtxConcat(view, modelM, modelview);
			GX_LoadPosMtxImm(modelview, GX_PNMTX0);
			aux->target->m_model->shadowRender();
			aux->target->shadowClr = idc;
			id+=50;
		}
		aux = aux->next;
	}
	//Copy the framebuffer
	GX_SetCopyFilter(GX_FALSE, NULL, GX_FALSE, NULL);
	GX_SetTexCopySrc(0,0,256,256);
	GX_SetTexCopyDst(256,256,GX_CTF_R8, GX_FALSE);
	GX_CopyTex(stencilBuffer, GX_TRUE);
	GX_PixModeSync();
	GX_InitTexObj(&shadowTex,stencilBuffer,256,256, GX_TF_I8, GX_CLAMP,GX_CLAMP,GX_FALSE);
	//Load it for textures map
	GX_LoadTexObj(&shadowTex, GX_TEXMAP0);*/
}

void clasify3D(NODE * node)
{
	//Clasify childs
	if(!node) return;//TODO: Make an assert from this
	NODE * aux = node->child;
	while(aux)
	{
		clasify3D(aux);
		aux = aux->next;
	}
	//Classify this node
	if(node->type == NT_OBJECT)
	{
		TRACKER * auxT = (TRACKER*)malloc(sizeof(TRACKER));
		auxT->next = NULL;
		auxT->target = (OBJECT*)node;
		if(auxT->target->usesAlpha())
		{
			auxT->next = transQueue;
			transQueue = auxT;
		}
		else
		{
			auxT->next = solidQueue;
			solidQueue = auxT;
		}
	}
}

void orderQueue(CAMERA * cam)
{
	TRACKER * tz = transQueue;//Temporal storage Queue;
	TRACKER * aux, *t;
	transQueue = NULL;
	Vector v;
	aux = tz;
	while(aux)
	{
		tz = aux->next;
		aux->next = NULL;
		//
		v = aux->target->getPos();
		guVecMultiply(view, &v, &v);
		aux->z = v.z;
		//
		t = transQueue;
		if(t)
		{
			while(t->next)
			{
				if(aux->z > t->next->z)
					t = t->next;
				else break;
			}
			if(aux->z > t->z)
			{
				aux->next = t->next;
				t->next = aux;
			}
			else
			{
				aux->next = transQueue;
				transQueue = aux;
			}
		}
		else
		{
			transQueue = aux;
			aux->next = NULL;
		}
		aux = tz;
	}
}

void setBGColor(GXColor clr)
{
	GX_SetCopyClear(clr, 0x00ffffff);
}

ROOT::ROOT()
{
	rootNode = NULL;
	rootN2D = NULL;
	fstView = NULL;
	m_vLights.reserve(8);
	fstHTC = NULL;
	fstBtn = NULL;
	shadowCaster = NULL;
	stencilBuffer = NULL;
}

void ROOT::init(f32 w, f32 h)
{
	//Init node spaces
	rootNode = new NODE;
	rootN2D = new N2D;
	//Init cameras
	defaultCam = new CAMERA();
	//Create main Viewport
	TRender2Texture * mainR2T = new TRender2Texture(defaultCam,w,h);
	fstView = new PANEL(vector3(0,0,1),w,h,mainR2T);
	
	REVConsole = new CONSOLE();

	ScriptMgr = new SCRIPTMGR;
}

u8 ROOT::shadowsEnabled()
{
	if(shadowCaster)
		return 1;
	return 0;
}

void REV_init()
{
	//Local variables
	f32 yscale;
	u32 xfbHeight;
	//Load the right IOS, so keyboard works fin
	u32 iosVersion = IOS_GetVersion();
	if(iosVersion != 36)
	{
		IOS_ReloadIOS(36);
	}
	////
	VIDEO_Init();//Video Library Init
	PAD_Init();//Gamecube Pad Library Init
	WPAD_Init();//Wiimote Library Init
	rMode = VIDEO_GetPreferredMode(NULL);//Set render mode
	// allocate 2 framebuffers for double buffering
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rMode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rMode));
	//Configure Video System
	VIDEO_Configure(rMode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);//Initial Framebuffer
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rMode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	fb ^= 1;//Set next index
	//Set up the FIFO
	void *gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);//Allocate
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);//and Clear
	//Init GX hardware
	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);
	// clears the bg to color and clears the z buffer
	GX_SetCopyClear(DEF_BG_COLOR , 0x00ffffff);
	//More initialization
	w = rMode->viWidth;
    h = rMode->viHeight;
	
	yscale = GX_GetYScaleFactor(rMode->efbHeight,rMode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetDispCopySrc(0,0,rMode->fbWidth,rMode->efbHeight);
	GX_SetDispCopyDst(rMode->fbWidth,xfbHeight);
	GX_SetFieldMode(rMode->field_rendering,((h==2*rMode->xfbHeight)?GX_ENABLE:GX_DISABLE));
	
	if (rMode->aa)
        GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
    else
        GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
	
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);
	
	for(u8 i = 0; i < 4; i++)
	WPAD_SetVRes(i,w,h);
	
	//Hardware Initialized, now engine initialization
	//Init filesystem
	fatInitDefault();
	//First create a Root
	mainRoot = new ROOT;
	mainRoot->init((f32)w,(f32)h);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetColorUpdate(GX_ENABLE);
	GX_SetAlphaUpdate(GX_ENABLE);
	GX_SetZCompLoc(GX_TRUE);
	GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);
	init3dConfig();
	init2dConfig();
	init2D(w, h);
	transQueue = NULL;
	solidQueue = NULL;
	zQueue = NULL;
	REV_fontsInit();
	KEYBOARD_Init(NULL);
	keyboardInit();
}

void REV_preProcess()
{
	WPAD_ScanPads();
	PAD_ScanPads();
	keyboardProcess();
	//Update time
	updateTime();
	HTCAMERA * htc = mainRoot->fstHTC;
	for(u8 i = 0; i < 4; i++)
	{
		wmData[i] = WPAD_Data(i);
	}
	update2D();
	while(htc)//Update Head-Tracking cameras's point of view
	{
		htc->updateEye();
		htc = htc->nextHTC;
	}
}

void render(NODE * node, Vector camPos)
{
	Mtx tmp;
	if(!node->isRenderable()) return;//Skip non-renderable nodes such as empty nodes, regions or lights
	if(node->flags & F_Visible)//Is it visible?
	{
		//Set matrices
		Mtx aux, inv;
		GX_ClearVtxDesc();
		node->absMtx(modelM);
		char msg[64];

		guMtxConcat(view, modelM, modelview);
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);//Load model view matrix
		guMtxInverse(modelview, tmp);
		guMtxTranspose(tmp,inv);
		GX_LoadNrmMtxImm(inv, GX_PNMTX0);//Load normal matrix

		//TODO: Better lighting (multiple lights)
		std::vector<LIGHT*>::iterator light = mainRoot->getLights().begin();
		//if(light == mainRoot->getLights().end())
		//	REVConsole->write("LIGHT");
		GXLightObj lObj[8];
		u8 lightMask = 0, tmpLight = GX_LIGHT0;
		for(u8 i = 0;light != mainRoot->getLights().end(); ++light, ++i)
		{
			Vector lpos = (*light)->getPos();
			guVecMultiply(view, &lpos, &lpos);
			
			GX_InitLightPos(&lObj[i],lpos.x,lpos.y,lpos.z);
			GX_InitLightAttn(&lObj[i], 1.0f,0.0f,0.0f,1.0f,0.0f,0.0f);
			GX_InitLightSpot(&lObj[i],0.0f,GX_SP_OFF);
			GX_InitLightColor(&lObj[i],(*light)->clr);
			GX_LoadLightObj(&lObj[i],tmpLight);
			lightMask |= tmpLight;
			tmpLight *= 2;
		}
		//Render the node
		node->render(lightMask);
	}
}

void REV_process(tMapQ3 * map)
{
	//Variables
	f32 preWait = 0, postWait = 0;
	static u8 firstFrame = 1;
	TRACKER * auxT;
	setUp3D();
	//Wait just before drawing (instead of after), this should enhance performance
	VIDEO_Flush();
	preWait = (f32)(ticks_to_millisecs(gettime()));
 	VIDEO_WaitVSync();
	postWait = (f32)(ticks_to_millisecs(gettime()));
	GPUWaitTime = 0.001f * (postWait - preWait);
	//Update physics
	updatePhysics();
	setBGColor(SC_BLUE);
	//Clasify objects into solid or transparent queues
	//This is done before everything else because this clasification is the same for every viewport
	clasify3D(mainRoot->rootNode);
	//Now we use the clasified queues to render shadows
	//if(mainRoot->shadowCaster)
		//mainRoot->shadowScene();
	//Render each Viewport into it's texture
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	//GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);
	std::multimap<CAMERA*, TRender2Texture*>::iterator iter = mainRoot->m_Render2Textures.begin();
	for(;iter != mainRoot->m_Render2Textures.end(); ++iter)
	{
		(*iter).second->setForRender(perspective);
		(*iter).second->getCamera()->setForRender(view);
		//Before rendering the scene, render the skyBox
		GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);
		mainRoot->skyBox.render((*iter).second->getCamera());
		GX_SetZMode (GX_TRUE, GX_LEQUAL, GX_TRUE);
		//Now render the map
		//GX_LoadPosMtxImm(view, GX_PNMTX0);
		//if(map)
			//renderQ3Map(tTex->cam->getPos(), map);
		//Now render objects
		GX_SetCullMode(GX_CULL_NONE);
		auxT = solidQueue;
		while(auxT)
		{
			render(auxT->target, (*iter).second->getCamera()->getPos());
			auxT = auxT->next;
		}
		orderQueue((*iter).second->getCamera());
		auxT = transQueue;
		while(auxT)
		{
			render(auxT->target, (*iter).second->getCamera()->getPos());
			auxT = auxT->next;
		}
		//Copy the embeded frame buffer to the texture
		(*iter).second->copyTexture();
	}
	while(solidQueue)
	{
		auxT = solidQueue;
		solidQueue = solidQueue->next;
		free(auxT);
	}
	while(transQueue)
	{
		auxT = transQueue;
		transQueue = transQueue->next;
		free(auxT);
	}
	setBGColor(SC_WHITE);
	//2D System
	//GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);
	GX_SetCopyFilter(rMode->aa,rMode->sample_pattern,GX_TRUE,rMode->vfilter);
	GX_SetViewport(0,0, w, h,0,1);
	GX_SetScissor(0,0, w, h);
	guOrtho(perspective,0,h,0,w, 1.0,100.0);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);
	GX_SetCullMode(GX_CULL_NONE);
	setUp2D();
	parse2D(mainRoot->rootN2D);
	order2D();
	render2D();
	GX_DrawDone();
	
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	
	//Set out black screen after first frame
	if(firstFrame)
	{
		firstFrame = 0;
		VIDEO_SetBlack(FALSE);
	}
	
	fb ^= 1;
}

void REV_exit()
{
	delete REVConsole;
	WPAD_Flush(WPAD_CHAN_ALL);
	WPAD_Disconnect(WPAD_CHAN_ALL);
	WPAD_Shutdown();
	exit(0);
}

void screenShot(const char* filename, PANEL * v)
{
	IMGCTX pngContext;
    pngContext = PNGU_SelectImageFromDevice(filename);
	if(!v)
	{
		PNGU_EncodeFromYCbYCr(pngContext, 640, 480, (frameBuffer[fb]), 0);
	}
	else
	{
		Vector pos = v->getPos();
		Vector size = v->getSize();
		PNGU_ENCODE_TO_COORDS_YCbYCr(pngContext,(u32)pos.x,(u32)pos.y,(u32)size.x,(u32)size.y,640,480,(frameBuffer[fb]));
	}
    PNGU_ReleaseImageContext(pngContext);
}

//Private functions
void setUp3D(void)
{
	GX_InvVtxCache();
	GX_InvalidateTexAll();
}

void setUp2D(void)
{
	GX_InvVtxCache();
	GX_InvalidateTexAll();
	GX_SetNumTexGens(1);
	GX_SetChanCtrl(GX_COLOR1A1,GX_FALSE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
	GX_SetChanCtrl(GX_COLOR0A0,GX_TRUE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
}

void init3dConfig(void)
{
	//Atributes format
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX4, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX5, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX6, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX7, GX_TEX_ST, GX_F32, 0);
}

void init2dConfig(void)
{
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR1, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX1, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX2, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX3, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX4, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX5, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX6, GX_TEX_ST, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX7, GX_TEX_ST, GX_F32, 0);
}
