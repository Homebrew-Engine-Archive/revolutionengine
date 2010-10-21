///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
#ifndef REV_HBMENU_H
#define REV_HBMENU_H

typedef struct _HBM_Custom{
BUTTON * ldrBtn;//Return to loader button
BUTTON * wmnBtn;//Return to Wii menu button
BUTTON * rstBtn;//Reset button
BUTTON * sdnBtn;//Shoutdown button
}HBMCUSTOM;

void homeMenuInit(void);
void REV_homeMenu(void);

#endif
