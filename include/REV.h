///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Main Header file
#ifndef REV_MAIN_H
#define REV_MAIN_H

//External header files
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <math.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <pngu.h>
#include <time.h>
#include <ogc/lwp_watchdog.h>
#include <sys/dir.h>
#include <sys/statvfs.h>
#include <wiikeyboard/keyboard.h>
#include <unistd.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>

//Revolution engine header files
#include "REVUtils.h"
#include "REVConfig.h"
extern "C"
{
	#include "Lua/lua.h"
}
#include "REVMaths.h"
#include "REVTime.h"
#include "REVPhysics.h"
#include "REVNodes.h"
#include "REVLights.h"
#include "REVCameras.h"
#include "REVMaterials.h"
#include "REVScreen.h"
#include "REVFonts.h"
#include "REVConsole.h"
#include "REVModels.h"
#include "REVMd5.h"
#include "REVObjects.h"
#include "REVSkybox.h"
#include "REVQBsp.h"
#include "REVResourceMgr.h"
#include "REVHBMenu.h"
#include "REVScript.h"

//Data types, classes and structures
void setBGColor(GXColor clr);

class ROOT
{
public:
	SKYBOX skyBox;
	PANEL * mainView(void) {return fstView;}
	ROOT();
	void init(f32 w, f32 h);
	void enableShadows(LIGHT * l, SDW_PARAMS shadow);
	u8 shadowsEnabled();
private:
	std::multimap<CAMERA*, TRender2Texture*> m_Render2Textures;
	PANEL * fstView;
	NODE * rootNode;
	N2D * rootN2D;
	LIGHT * fstLight;
	HTCAMERA * fstHTC;
	BUTTON * fstBtn;
	LIGHT * shadowCaster;
	void * stencilBuffer;
	void shadowScene(void);
	GXTexObj shadowTex;
	SDW_PARAMS s;
friend class NODE;
friend class N2D;
friend class HTCAMERA;
friend class LIGHT;
friend class AMODEL;//Temporary
friend class SMODEL;//Temporary
friend class TRender2Texture;
friend class BUTTON;
friend void REV_preProcess(void);
friend void render(NODE * node, Vector camPos);
friend void REV_process(tMapQ3* map);
friend void update2D(void);
friend u8 getClrIntensity(Vector normal);
friend void updatePhysics();
};
//Global data
extern ROOT * mainRoot;
extern WPADData * wmData[4];
extern GXRModeObj *rMode;
extern CONSOLE * REVConsole;
extern SCRIPTMGR * ScriptMgr;

//Public functions
void REV_init();
void REV_preProcess();
void REV_process(tMapQ3 * map);
void REV_exit();

void screenShot(const char* filename, PANEL * v = NULL);

//Private functions

#endif
