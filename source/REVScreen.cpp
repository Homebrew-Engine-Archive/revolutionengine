///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Screen Source file
#include "REV.h"

//Internal prototypes
ftgxCharData * findChar(u8 key, u8 size);
ftgxCharData * newChar(u8 key);
wchar_t* charToWideChar(char* p);

//Global data
IRPOINTER cursor[4];
N2DT * renderQ = NULL;

//Internal functions
void order2D()
{
	N2DT * tz = renderQ;//Temporal storage Queue;
	N2DT * aux, *t;
	renderQ = NULL;
	aux = tz;
	while(aux)
	{
		tz = aux->next;
		aux->next = NULL;
		t = renderQ;
		if(t)
		{
			while(t->next)
			{
				if(aux->target->getPos().z > t->next->target->getPos().z)
					t = t->next;
				else break;
			}
			if(aux->target->getPos().z > t->target->getPos().z)
			{
				aux->next = t->next;
				t->next = aux;
			}
			else
			{
				aux->next = renderQ;
				renderQ = aux;
			}
		}
		else
		{
			renderQ = aux;
		}
		//
		aux = tz;
	}
}

void render2D(){
	N2DT * nt = renderQ;
	while(nt)
	{
		GX_ClearVtxDesc();
		switch(nt->target->type)
		{
			case NT_PANEL:
			case NT_BUTTON:
			{
				PANEL * p = (PANEL*)nt->target;
				p->render();
				break;
			}
			case NT_WINDOW:
			{
				WINDOW * w = (WINDOW*)nt->target;
				w->render();
				break;
			}
			case NT_TEXT:
			{
				TEXT * t = (TEXT*)nt->target;
				t->render();
				break;
			}
		}
		renderQ = nt->next;
		free(nt);
		nt = renderQ;
	}
	renderQ = NULL;
}

void BUTTON::checkStat(u8 channel, f32 x, f32 y)
{
	Vector pos = this->getPos();
	u8 newStat = BS_RELEASED, newOver = 0;
	if(this->flags & F_Center)
	{
		f32 szx = this->sx * 0.5f;
		f32 szy = this->sy * 0.5f;
		if((x > pos.x-szx) && (x < pos.x+szx)&&(y > pos.y - szy) && (y < pos.y+szy))
		{
			newOver = 1;
			if((WPAD_ButtonsDown(channel) & WPAD_BUTTON_A)
			||(WPAD_ButtonsHeld(channel) & WPAD_BUTTON_A))
			{
				newStat = BS_CLICKED;
			}
		}
		else//Outside clicks
		{
			if(WPAD_ButtonsDown(channel) & WPAD_BUTTON_A)
			if(this->offClick)
			this->offClick(channel, this);
		}
	}
	else
	{
		f32 szx = this->sx;
		f32 szy = this->sy;
		if((x > pos.x) && (x < pos.x+szx)&&(y > pos.y) && (y < pos.y+szy))
		{
			newOver = 1;
			if((WPAD_ButtonsDown(channel) & WPAD_BUTTON_A)
			||(WPAD_ButtonsHeld(channel) & WPAD_BUTTON_A))
			{
				newStat = BS_CLICKED;
			}
		}
		else//Outside clicks
		{
			if(WPAD_ButtonsDown(channel) & WPAD_BUTTON_A)
			if(this->offClick)
			this->offClick(channel, this);
		}
	}
	if((newStat==BS_CLICKED)&&(this->state[channel]==BS_RELEASED))
	{
		this->state[channel] = BS_CLICKED;
		if(this->onClick)
		this->onClick(channel, this);
	}
	if(this->state[channel] != BS_RELEASED)
	{
		if(newStat == BS_CLICKED)
		{
			this->state[channel] = BS_HELD;
			if(this->onHeld)
			this->onHeld(channel, this);
		}
		else
		{
			this->state[channel] = BS_RELEASED;
			if(this->onRelease)
			this->onRelease(channel, this);
		}
	}
	if(newOver > this->over[channel])
	{
		this->over[channel] = 1;
		if(this->onOver)
		this->onOver(channel, this);
	}
	else if(newOver < this->over[channel])
	{
		this->over[channel] = 0;
		if(this->offOver)
		this->offOver(channel, this);
	}
	
}

void parse2D(N2D * node)
{
	if(node->flags & F_Visible)
	{
		N2DT * nt = (N2DT*)malloc(sizeof(N2DT));
		nt->target = node;
		nt->next = renderQ;
		renderQ = nt;
		N2D * aux = node->child;
		while(aux)
		{
			parse2D(aux);
			aux = aux->next;
		}
	}
}

PANEL::PANEL(Vector pos, f32 sx, f32 sy, TEXTURE * texture, GXColor clr, u8 flags)
{
	this->sx = sx;
	this->sy = sy;
	this->setPos(pos, RT_ABS);
	this->texture = texture;
	this->clr = clr;
	this->flags = flags;
	this->type = NT_PANEL;
}

Vector PANEL::getSize(){
	return vector2(sx, sy);
}

BUTTON::BUTTON(Vector pos, f32 sx, f32 sy, TEXTURE * texture, GXColor clr, u8 flags)
{
	//just like a panel
	this->sx = sx;
	this->sy = sy;
	this->setPos(pos, RT_ABS);
	this->texture = texture;
	this->clr = clr;
	this->flags = flags;
	this->type = NT_BUTTON;
	this->mode = BM_ON;
	for(u8 i = 0; i < 4; i++)
	{
		this->state[i] = 0;
		this->over[i] = 0;
	}
	onClick = NULL;
	onHeld = NULL;
	onRelease = NULL;
	onOver = NULL;
	offOver = NULL;
	offClick = NULL;
	this->nextBtn = mainRoot->fstBtn;
	mainRoot->fstBtn = this;
}

void PANEL::setSize(f32 x, f32 y)
{
	this->sx = x;
	this->sy = y;
}

void PANEL::render()
{
	//setTev
	Vector v = getPos(RT_ABS);
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetChanCtrl(GX_COLOR0A0,GX_FALSE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetTevColor(GX_TEVREG0, clr);
	Mtx pnlMtx;
	guMtxRotDeg(pnlMtx, 'z', this->getAngle(RT_ABS));
	guMtxTransApply(pnlMtx, pnlMtx, v.x, v.y, -PNL_MAX_LAYER+v.z);
	GX_LoadPosMtxImm(pnlMtx, GX_PNMTX0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	if(texture)
	{
		GX_LoadTexObj(texture->tex, GX_TEXMAP0);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	}
	else
	{
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
	}
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_Begin(GX_QUADS, GX_VTXFMT1, 4);
	if(flags & F_Center)
	{
		GX_Position3f32(-sx*0.5, -sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(0.0f, 0.0f);
		GX_Position3f32(sx*0.5, -sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(1.0f, 0.0f);
		GX_Position3f32(sx*0.5, sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(1.0f, 1.0f);
		GX_Position3f32(-sx*0.5, sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(0.0f, 1.0f);
	}
	else
	{
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		if(texture)GX_TexCoord2f32(0.0f, 0.0f);
		GX_Position3f32(sx, 0.0f, 0.0f);
		if(texture)GX_TexCoord2f32(1.0f, 0.0f);
		GX_Position3f32(sx, sy, 0.0f);
		if(texture)GX_TexCoord2f32(1.0f, 1.0f);
		GX_Position3f32(0.0f, sy, 0.0f);
		if(texture)GX_TexCoord2f32(0.0f, 1.0f);
	}
	GX_End();
}

void selfstrcpy(char * dst, char * src)
{
	u16 i = 0;
	for(i = 0; src[i] != '\0'; i++)
	dst[i] = src[i];
	dst[i] = '\0';
}

TEXT::TEXT(u8 size, Vector pos, char * content, GXColor clr, u8 flags)
{
	this->text = (char*)malloc(TEXT_DEFAULT_LENGTH*sizeof(char));
	this->size = size;
	this->setPos(pos);
	strcpy(this->text, content);
	this->clr = clr;
	this->flags = flags;
	this->type = NT_TEXT;
}

TEXT::TEXT(u8 size, Vector pos, u8 length, GXColor clr, u8 flags)
{
	this->text = (char*)malloc(length*sizeof(char));
	this->size = size;
	this->setPos(pos);
	this->clr = clr;
	this->flags = flags;
	this->type = NT_TEXT;
}

void TEXT::render()
{
	Vector v = getPos(RT_ABS);
	Mtx pnlMtx, auxMt;
	u16 len = strlen(text);
	u16 i, x_pos = 0;
	u16 textureWidth, textureHeight;
	GXTexObj glyphTexture;
	ftgxCharData* glyphData;
	setfontsize(size);
	//setTev
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetChanCtrl(GX_COLOR0A0,GX_FALSE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetTevColor(GX_TEVREG0, clr);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
	GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	//Text drawing
	for(i=0;i<len;i++)
	{
		glyphData = findChar(text[i], size);
		if(!glyphData)
		glyphData = newChar(text[i]);
		//Matrix computing
		guMtxIdentity(pnlMtx);
		guMtxTransApply(pnlMtx, pnlMtx, x_pos, - glyphData->renderOffsetY, 0.0f);
		guMtxRotDeg(auxMt, 'z', this->getAngle(RT_ABS));
		guMtxConcat(auxMt, pnlMtx, pnlMtx);
		guMtxTransApply(pnlMtx, pnlMtx, v.x, v.y, -PNL_MAX_LAYER+v.z);
		GX_LoadPosMtxImm(pnlMtx, GX_PNMTX0);
		
		if(glyphData)
		{
			GX_InitTexObj(&glyphTexture, glyphData->glyphDataTexture, glyphData->textureWidth, glyphData->textureHeight, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
			textureWidth = glyphData->textureWidth;
			textureHeight = glyphData->textureHeight;
			GX_LoadTexObj(&glyphTexture, GX_TEXMAP0);
			GX_Begin(GX_QUADS, GX_VTXFMT1, 4);
				GX_Position3f32(0.0,0.0,0.0);
				GX_TexCoord2f32(0.0,0.0);
				
				GX_Position3f32(textureWidth,0.0,0.0);
				GX_TexCoord2f32(1.0,0.0);
				
				GX_Position3f32(textureWidth,textureHeight,0.0);
				GX_TexCoord2f32(1.0,1.0);
				
				GX_Position3f32(0.0,textureHeight,0.0);
				GX_TexCoord2f32(0.0,1.0);
			GX_End();
			x_pos += glyphData->glyphAdvanceX;
		}
	}
}

void init2D(f32 w, f32 h)
{
	for(u8 i = 0; i < 4; i++)
	{
		cursor[i].mode = PM_OFF;
		cursor[i].x = cursor[i].y = cursor[i].angle = 0;
		cursor[i].nexus = NULL;
		WPAD_SetDataFormat(i, WPAD_FMT_BTNS_ACC);
	}
}

void update2D(void)
{
	for(u8 i = 0; i < 4; i++)
	{
		//Update position
		if((cursor[i].mode == PM_ON)&&(cursor[i].nexus))
		{
			wmData[i] = WPAD_Data(i);
			
			if(wmData[i]->ir.valid)
			{
				cursor[i].angle = wmData[i]->ir.angle;
				cursor[i].x = wmData[i]->ir.x;
				cursor[i].y = wmData[i]->ir.y;
				cursor[i].nexus->setPos(vector3(cursor[i].x, cursor[i].y, cursor[i].nexus->getPos().z));
				cursor[i].nexus->setAngle(cursor[i].angle);
				cursor[i].nexus->flags |= F_Visible;
			}
			else cursor[i].nexus->flags &= ~F_Visible;
			
			BUTTON * aux = mainRoot->fstBtn;
			while(aux)
			{
				if(aux->mode & BM_ON)
				aux->checkStat(i, cursor[i].x, cursor[i].y);
				aux = aux->nextBtn;
			}
		}
	}
}

void pointerAttach(u8 channel, N2D * node)
{
	cursor[channel].nexus = node;
}
void pointerRelease(u8 channel)
{
	cursor[channel].nexus = NULL;
}

void setPointerOn(u8 channel)
{
	cursor[channel].mode = PM_ON;
	WPAD_SetDataFormat(channel, WPAD_FMT_BTNS_ACC_IR);
}

void setPointerOff(u8 channel)
{
	cursor[channel].mode = PM_OFF;
	WPAD_SetDataFormat(channel, WPAD_FMT_BTNS_ACC);
}

Vector pointerGetPos(u8 channel)
{
	return vector3(cursor[channel].x, cursor[channel].y, cursor[channel].angle);
}


N2D * pointerGetNexus(u8 channel)
{
	return cursor[channel].nexus;
}

WINDOW::WINDOW(Vector pos, f32 sx, f32 sy, TEXTURE * texture, u8 flags)
{
	this->setPos(pos,RT_ABS);
	this->setSize(sx,sy);
	this->texture = texture;
	this->flags = flags;
}

void WINDOW::setWindowRect(f32 x0,f32 y0,f32 size_x,f32 size_y)
{
	this->x0 = x0;
	this->y0 = y0;
	this->szx = size_x;
	this->szy = size_y;
}
RECTANGLE WINDOW::getWindowSize()
{
	return Rectangle4(x0,y0,szx,szy);
}

void WINDOW::render()
{
	//setTev
	Vector v = getPos(RT_ABS);
	GX_SetNumChans(1);
	GX_SetNumTevStages(1);
	GX_SetNumTexGens(1);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY);
	GX_SetChanCtrl(GX_COLOR0A0,GX_FALSE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetTevColor(GX_TEVREG0, clr);
	Mtx pnlMtx;
	guMtxRotDeg(pnlMtx, 'z', this->getAngle(RT_ABS));
	guMtxTransApply(pnlMtx, pnlMtx, v.x, v.y, -PNL_MAX_LAYER+v.z);
	GX_LoadPosMtxImm(pnlMtx, GX_PNMTX0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	if(texture)
	{
		GX_LoadTexObj(texture->tex, GX_TEXMAP0);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	}
	else
	{
		GX_SetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GX_SetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
	}
	GX_SetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_SetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_ENABLE, GX_TEVPREV);
	GX_Begin(GX_QUADS, GX_VTXFMT1, 4);
	if(flags & F_Center)
	{
		GX_Position3f32(-sx*0.5, -sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(x0, y0);
		GX_Position3f32(sx*0.5, -sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(szx+x0, y0);
		GX_Position3f32(sx*0.5, sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(szx+x0, szy+y0);
		GX_Position3f32(-sx*0.5, sy*0.5f, 0.0f);
		if(texture)GX_TexCoord2f32(x0, szy+y0);
	}
	else
	{
		GX_Position3f32(0.0f, 0.0f, 0.0f);
		if(texture)GX_TexCoord2f32(x0, y0);
		GX_Position3f32(sx, 0.0f, 0.0f);
		if(texture)GX_TexCoord2f32(x0+szx, y0);
		GX_Position3f32(sx, sy, 0.0f);
		if(texture)GX_TexCoord2f32(x0+szx, y0+szy);
		GX_Position3f32(0.0f, sy, 0.0f);
		if(texture)GX_TexCoord2f32(x0, y0+szy);
	}
	GX_End();
}
