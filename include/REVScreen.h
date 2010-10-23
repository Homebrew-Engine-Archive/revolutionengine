///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Screen header file
#ifndef SCREEN_H
#define SCREEN_H

//Classes and data types and structures
class PANEL:public N2D
{
public:
	PANEL(Vector pos = nullVector, f32 sx = 50.0f, f32 sy = 50.0f, TEXTURE * texture = NULL, GXColor = SC_WHITE, u8 flags = F_Visible);
	TEXTURE * texture;
	GXColor clr;
	virtual void setSize(f32 x, f32 y);
	Vector getSize();
private:
	void render();
protected:
	f32 sx, sy;//Size
friend void REV_init();
friend void render2D();
friend class VIEWPORT;
};

class WINDOW:public PANEL
{
public:
	WINDOW(Vector pos, f32 sx, f32 sy, TEXTURE * texture, u8 flags = F_Visible);
	void setWindowRect(f32 x0,f32 y0,f32 size_x,f32 size_y);
	RECTANGLE getWindowSize();
protected:
	f32 x0,y0,szx,szy;
private:
	void render();
friend void REV_init();
friend void render2D();
};

class BUTTON:public PANEL
{
public:
	BUTTON(Vector pos = nullVector, f32 sx = 50.0f, f32 sy = 50.0f, TEXTURE * texture = NULL, GXColor = SC_WHITE, u8 flags = F_Visible);
	u8 mode;
	void (*onClick)(u8 channel, BUTTON * self);
	void (*onHeld)(u8 channel, BUTTON * self);
	void (*onRelease)(u8 channel, BUTTON * self);
	void (*onOver)(u8 channel, BUTTON * self);
	void (*offOver)(u8 channel, BUTTON * self);
	void (*offClick)(u8 channel, BUTTON * self);
private:
	u8 state[4];//internal state
	u8 over[4];//Pointer over button?
	BUTTON * nextBtn;
	void checkStat(u8 channel, f32 x, f32 y);
friend void update2D();
};

class TEXT:public N2D
{
public:
	TEXT(u8 size, Vector pos, char * content, GXColor clr = SC_WHITE, u8 flags = F_Visible);
	TEXT(u8 size, Vector pos, u8 length = TEXT_DEFAULT_LENGTH, GXColor clr = SC_WHITE, u8 flags = F_Visible);
	GXColor clr;
	char * text;
	u8 size;
private:
	void render();
friend void render2D();
};

typedef struct n2dTracker
{
	N2D * target;
	struct n2dTracker * next;
}N2DT;

typedef struct ir_pointer
{
	f32 x, y ,angle;
	N2D * nexus;
	u8 mode;
}IRPOINTER;

void order2D();
void init2D(f32 w, f32 h);
void update2D();
void parse2D(N2D * node);
void render2D();
void setPointerOn(u8 channel);
void setPointerOff(u8 channel);
void pointerAttach(u8 channel, N2D * node);
void pointerRelease(u8 channel);
u8 getPointerMode(u8 channel);
Vector pointerGetPos(u8 channel);
N2D * pointerGetNexus(u8 channel);

#endif
