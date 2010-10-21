///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Screen Source file
#include "REV.h"

//data structures
typedef struct REV_Font
{
	ftgxCharData * fstChar;
	u8 size;
	struct REV_Font * next;
}FNT;

//Global data
FT_Library ftlibrary;
FT_Face face;
FT_GlyphSlot slot;

FNT * fstFnt, *auxFnt;

extern char fontface[];
extern int fontsize;

//Internal prototypes
ftgxCharData * findChar(u8 key, u8 size);
ftgxCharData * newChar(u8 key);
static wchar_t* charToWideChar(char* p);

void REV_fontsInit(void)
{
	FT_Init_FreeType (&ftlibrary);
	FT_New_Memory_Face (ftlibrary, (FT_Byte *) fontface, fontsize, 0, &face);
	setfontsize (16);
	slot = face->glyph;
	fstFnt = NULL;
	auxFnt = NULL;
}

void REV_FT_Done(void)
{
	FT_Done_FreeType(ftlibrary);
}

void setfontsize (u8 pixels)
{
	FT_Set_Pixel_Sizes (face, 0, pixels);
}

u16 CalcTextSize(TEXT * text)
{
u16 len;
u16 i, size;
ftgxCharData* glyphData;


    len = strlen(text->text);
    size = 0;
    
    for(i=0;i<len;i++)
    {
        glyphData = findChar(text->text[i], text->size);
        if(!glyphData)
        glyphData = newChar(text->text[i]);
        
        if(glyphData)
        {
            size += glyphData->glyphAdvanceX;
        }
    }
    
    return (size);
}

u16 adjustTextureWidth(u16 textureWidth, u8 textureFormat)
{
	u16 alignment;
	
	switch(textureFormat) {
		case GX_TF_I4:		/* 8x8 Tiles - 4-bit Intensity */
		case GX_TF_I8:		/* 8x4 Tiles - 8-bit Intensity */
		case GX_TF_IA4:		/* 8x4 Tiles - 4-bit Intensity, , 4-bit Alpha */
			alignment = 8;
			break;

		case GX_TF_IA8:		/* 4x4 Tiles - 8-bit Intensity, 8-bit Alpha */
		case GX_TF_RGB565:	/* 4x4 Tiles - RGB565 Format */
		case GX_TF_RGB5A3:	/* 4x4 Tiles - RGB5A3 Format */
		case GX_TF_RGBA8:	/* 4x4 Tiles - RGBA8 Dual Cache Line Format */
		default:
			alignment = 4;
			break;
	}
	return textureWidth % alignment == 0 ? textureWidth : alignment + textureWidth - (textureWidth % alignment);

}

u16 adjustTextureHeight(u16 textureHeight, u8 textureFormat)
{
	u16 alignment;
	
	switch(textureFormat) {
		case GX_TF_I4:		/* 8x8 Tiles - 4-bit Intensity */
			alignment = 8;
			break;

		case GX_TF_I8:		/* 8x4 Tiles - 8-bit Intensity */
		case GX_TF_IA4:		/* 8x4 Tiles - 4-bit Intensity, , 4-bit Alpha */
		case GX_TF_IA8:		/* 4x4 Tiles - 8-bit Intensity, 8-bit Alpha */
		case GX_TF_RGB565:	/* 4x4 Tiles - RGB565 Format */
		case GX_TF_RGB5A3:	/* 4x4 Tiles - RGB5A3 Format */
		case GX_TF_RGBA8:	/* 4x4 Tiles - RGBA8 Dual Cache Line Format */
		default:
			alignment = 4;
			break;
	}
	return textureHeight % alignment == 0 ? textureHeight : alignment + textureHeight - (textureHeight % alignment);

}

u32 * convertBufferToRGBA8(u32 * rgbaBuffer, u16 bufferWidth, u16 bufferHeight)
{
	u32 bufferSize = (bufferWidth * bufferHeight) << 2;
	u32 * dataBufferRGBA8 = (u32 *)memalign(32, bufferSize);
	memset(dataBufferRGBA8, 0x00, bufferSize);

	u8 *src = (u8 *)rgbaBuffer;
	u8 *dst = (u8 *)dataBufferRGBA8;
	
	u16 block, i, c, ar, gb;

	for(block = 0; block < bufferHeight; block += 4) {
		for(i = 0; i < bufferWidth; i += 4) {
			for (c = 0; c < 4; c++) {
				for (ar = 0; ar < 4; ar++) {
					*dst++ = src[(((i + ar) + ((block + c) * bufferWidth)) * 4) + 3];
					*dst++ = src[((i + ar) + ((block + c) * bufferWidth)) * 4];
				}
			}
			for (c = 0; c < 4; c++) {
				for (gb = 0; gb < 4; gb++) {
					*dst++ = src[(((i + gb) + ((block + c) * bufferWidth)) * 4) + 1];
					*dst++ = src[(((i + gb) + ((block + c) * bufferWidth)) * 4) + 2];
				}
			}
		}
	}
	DCFlushRange(dataBufferRGBA8, bufferSize);

	return dataBufferRGBA8;
}

void loadGlyphData(FT_Bitmap * bmp, ftgxCharData * charData)
{
	u16 imagePosY, imagePosX;
	u32 * glyphData = (u32 *)memalign(32, charData->textureWidth * charData->textureHeight * 4);
	memset(glyphData, 0x00, charData->textureWidth * charData->textureHeight * 4);
	
	for (imagePosY = 0; imagePosY < bmp->rows; imagePosY++) {
		for (imagePosX = 0; imagePosX < bmp->width; imagePosX++) {
			u32 pixel = (u32) bmp->buffer[imagePosY * bmp->width + imagePosX];
			glyphData[imagePosY * charData->textureWidth + imagePosX] = 0x00000000 | (pixel << 24) | (pixel << 16) | (pixel << 8) | pixel;
		}
	}
	
	charData->glyphDataTexture = convertBufferToRGBA8(glyphData, charData->textureWidth, charData->textureHeight);
	
	free(glyphData);
}

ftgxCharData * newChar(u8 key)
{
	u16 textureWidth = 0, textureHeight = 0;
	u32 gIndex = FT_Get_Char_Index( face, *charToWideChar(((char*)(&key))));
	ftgxCharData * auxChar = (ftgxCharData*)malloc(sizeof(ftgxCharData));
	auxChar->key = key;
	auxChar->next = auxFnt->fstChar;
	auxFnt->fstChar = auxChar;
	if (!FT_Load_Glyph(face, gIndex, FT_LOAD_DEFAULT ))
	{
		FT_Render_Glyph( slot, FT_RENDER_MODE_NORMAL );
		if(slot->format == FT_GLYPH_FORMAT_BITMAP)
		{
			FT_Bitmap *glyphBitmap = &slot->bitmap;
			textureWidth = adjustTextureWidth(glyphBitmap->width, GX_TF_RGBA8);
			textureHeight = adjustTextureHeight(glyphBitmap->rows, GX_TF_RGBA8);
			
			auxChar->glyphAdvanceX = slot->advance.x >> 6;
			auxChar->glyphIndex = gIndex;
			auxChar->textureWidth = textureWidth;
			auxChar->textureHeight = textureHeight;
			auxChar->renderOffsetY = slot->bitmap_top;
			auxChar->renderOffsetMax = slot->bitmap_top;
			auxChar->renderOffsetMin = textureHeight - slot->bitmap_top;
			auxChar->glyphDataTexture = NULL;
			
			loadGlyphData(glyphBitmap, auxChar);
			
			return auxChar;
		}
	}
	free(auxChar);
	return NULL;
}

ftgxCharData * findChar(u8 key, u8 size)//The font should actually be created by newChar.
{
	auxFnt = fstFnt;
	while(auxFnt)
	{
		if(auxFnt->size == size) break;
		auxFnt = auxFnt->next;
	}
	if(!auxFnt)
	{
		auxFnt = (FNT*)malloc(sizeof(FNT));
		auxFnt->next = fstFnt;
		fstFnt = auxFnt;
		auxFnt->size = size;
		auxFnt->fstChar = NULL;
	}
	ftgxCharData * auxChar = auxFnt->fstChar;
	while(auxChar)
	{
		if(auxChar->key == key) return auxChar;
		auxChar = auxChar->next;
	}
	return NULL;
}

static wchar_t* charToWideChar(char* p)
{
      wchar_t * strWChar;
      strWChar = (wchar_t*)malloc(sizeof(wchar_t)*(strlen(p) + 1));

      char *tempSrc = p;
      wchar_t * tempDest = strWChar;
      while((*tempDest++ = *tempSrc++));

      return strWChar;
}