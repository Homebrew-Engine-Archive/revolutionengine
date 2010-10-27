///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Time Source file

//header files
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

#include "REVTime.h"

f32 frameTime, gameTime, timeSpeed = 1.0f;
f32 last_frame = 0;
f32 GPUWaitTime = 0;

//Private functions
void updateTime()
{
	f32 current_time = (f32)(ticks_to_millisecs(gettime()));
	if(last_frame) frameTime = current_time - last_frame;
	else frameTime = 0;
	last_frame = current_time;
	frameTime *= 0.001f;
	gameTime = frameTime * timeSpeed;
}
