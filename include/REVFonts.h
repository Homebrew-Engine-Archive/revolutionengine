///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//fonts header file
#ifndef FONTS_H
#define FONTS_H

//Structures
typedef struct ftgxCharData_ {
	u16 glyphAdvanceX;	/**< Character glyph X coordinate advance in pixels. */
	u16 glyphIndex;	/**< Charachter glyph index in the font face. */

	u16 textureWidth;	/**< Texture width in pixels/bytes. */
	u16 textureHeight;	/**< Texture glyph height in pixels/bytes. */

	u16 renderOffsetY;	/**< Texture Y axis bearing offset. */
	u16 renderOffsetMax;	/**< Texture Y axis bearing maximum value. */
	u16 renderOffsetMin;	/**< Texture Y axis bearing minimum value. */

	u32 * glyphDataTexture;	/**< Glyph texture bitmap data buffer. */
	
	//Rev Add ons
	struct ftgxCharData_ * next;
	u8 key;
} ftgxCharData;

//Public functions
void setfontsize (u8 pixels);
u16 CalcTextSize(TEXT * text);

//Internal functions
void REV_fontsInit(void);
void REV_FT_Done(void);

#endif
