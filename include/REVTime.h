///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Time header file
#ifndef REV_TIME_H
#define REV_TIME_H

#define framesPerSecond (1.0f/frameTime)
//Public variables
extern f32 frameTime, gameTime, timeSpeed, GPUWaitTime;

//Private functions
void updateTime();

#endif
