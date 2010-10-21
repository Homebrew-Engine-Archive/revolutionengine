///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Quake 3 bsp loader
#include "REV.h"

//Functions
void fixEndian(void * ptr, int amount, int size)
{
	u8 * tmp = (u8*)malloc(size*amount);
	u8 * dst = (u8*)ptr;
	for(int i = 0; i < amount; i++)
	{
		for(int j = 0; j < size; j++)
		{
			tmp[i*size+j] = dst[(i+1)*size-j-1];
		}
	}
	memcpy(ptr,tmp,amount*size);
	free(tmp);
}

u8 isValid(tMapQ3 * pMap)
{
	// Check if the header is equal to ID Software Magic Number.
	if(strncmp(pMap->mHeader.mMagicNumber, MagicNumber, 4) != 0)
		return 0;
	// Check if the version number is equal to the Q3 map.
	if(pMap->mHeader.mVersion != Q3Version)
		return 0;
	return 1;
}

u8 readHeader(FILE * pFile, tMapQ3 * pMap)
{
	fread(&pMap->mHeader, 1, sizeof(tHeader), pFile);
	//Fix endians
	fixEndian(&pMap->mHeader.mVersion, 1, sizeof(int));
	fixEndian(pMap->mHeader.mLumpes, 17*2, sizeof(int));
	//End endians
	return isValid(pMap);
}

void readTexture(FILE * pFile, tMapQ3 * pMap)
{
	int	lNbTextures = pMap->mHeader.mLumpes[cTextureLump].mLength / sizeof(tTexture);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cTextureLump].mOffset, SEEK_SET);
	
	//Allocate memory for textures
	pMap->mGXTextures = (GXTexObj**)memalign(32, sizeof(GXTexObj)*lNbTextures);
	pMap->mTextures = (tTexture*)malloc(pMap->mHeader.mLumpes[cTextureLump].mLength);

	for(int i = 0; i < lNbTextures; ++i)
	{
		fread(&(pMap->mTextures[i]), 1, sizeof(tTexture), pFile);
		fixEndian(&(pMap->mTextures[i].mFlags),2,sizeof(int));//Fix endianness
		//We depreciate contents in order to support TEV
		pMap->mTextures[i].mContents = i;
		//Now load the actual texture, first overwrite
		u8 length = strlen(pMap->mTextures[i].mName)-3;
		pMap->mTextures[i].mName[length] = 'p';
		pMap->mTextures[i].mName[length+1] = 'n';
		pMap->mTextures[i].mName[length+2] = 'g';
		pMap->mGXTextures[i] = loadPng(pMap->mTextures[i].mName);
	}
}

void readEntity(FILE * pFile, tMapQ3 * pMap)
{
	// Set the entity size.
	pMap->mEntity.mSize = pMap->mHeader.mLumpes[cEntityLump].mLength;
	
	// Allocate the entity buffer.
	pMap->mEntity.mBuffer =  new char[pMap->mEntity.mSize];

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cEntityLump].mOffset, SEEK_SET);
	
	// Read the buffer.
	fread(pMap->mEntity.mBuffer, pMap->mEntity.mSize, sizeof(char), pFile);
};

void readPlane(FILE* pFile, tMapQ3 * pMap)
{
	int	lNbPlanes = pMap->mHeader.mLumpes[cPlaneLump].mLength / sizeof(tPlane);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cPlaneLump].mOffset, SEEK_SET);
	
	//Allocate memory for planes
	pMap->mPlanes = (tPlane*)malloc(pMap->mHeader.mLumpes[cPlaneLump].mLength);

	for(int i = 0; i < lNbPlanes; ++i)
	{
		fread(&(pMap->mPlanes[i]), 1, sizeof(tPlane), pFile);
		//fix endian
		fixEndian(pMap->mPlanes[i].mNormal,3, sizeof(float));
		fixEndian(&(pMap->mPlanes[i].mDistance),1, sizeof(float));
		//fix coordinates
		for(u8 j = 0; j < 3; j++)
			pMap->mPlanes[i].mNormal[j] *= -1.0f;
	}
}

void readNode(FILE* pFile, tMapQ3 * pMap)
{
	int	lNbNodes = pMap->mHeader.mLumpes[cNodeLump].mLength / sizeof(tNode);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cNodeLump].mOffset, SEEK_SET);
	
	//Allocate memory for nodes
	pMap->mNodes = (tNode*)malloc(pMap->mHeader.mLumpes[cNodeLump].mLength);

	for(int i = 0; i < lNbNodes; ++i)
	{
		fread(&(pMap->mNodes[i]), 1, sizeof(tNode), pFile);
		//fix endian
		fixEndian(&pMap->mNodes[i].mPlane,1,sizeof(int));
		fixEndian(pMap->mNodes[i].mChildren,2,sizeof(int));
		fixEndian(pMap->mNodes[i].mMaxs,3,sizeof(int));
		fixEndian(pMap->mNodes[i].mMins,3,sizeof(int));
	}	
}

void readLeaf(FILE * pFile, tMapQ3 * pMap)
{
	int	lNbLeaves = pMap->mHeader.mLumpes[cLeafLump].mLength / sizeof(tLeaf);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cLeafLump].mOffset, SEEK_SET);
	
	//Allocate memory for leaves
	pMap->mLeaves = (tLeaf*)malloc(pMap->mHeader.mLumpes[cLeafLump].mLength);

	for(int i = 0; i < lNbLeaves; ++i)
	{
		fread(&(pMap->mLeaves[i]), 1, sizeof(tLeaf), pFile);
		//fix endianness
		fixEndian(&pMap->mLeaves[i], 2, sizeof(int));
		fixEndian(pMap->mLeaves[i].mMins, 3, sizeof(int));
		fixEndian(pMap->mLeaves[i].mMaxs, 3, sizeof(int));
		fixEndian(&pMap->mLeaves[i].mLeafFace, 4, sizeof(int));
	}	
}

void readLeafFace(FILE * pFile, tMapQ3 * pMap)
{
	int	lNbLeafFaces = pMap->mHeader.mLumpes[cLeafFaceLump].mLength / sizeof(tLeafFace);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cLeafFaceLump].mOffset, SEEK_SET);
	//Allocate memory
	pMap->mLeafFaces = (tLeafFace*)malloc(pMap->mHeader.mLumpes[cLeafFaceLump].mLength);

	for(int i = 0; i < lNbLeafFaces; ++i)
	{
		fread(&pMap->mLeafFaces[i], 1, sizeof(TLeafFace), pFile);
		//Fix endian
		fixEndian(&pMap->mLeafFaces[i],1,sizeof(int));
	}			
}

void readLeafBrush(FILE * pFile, TMapQ3 * pMap)
{
	int	lNbLeafBrushes = pMap->mHeader.mLumpes[cLeafBrushLump].mLength / sizeof(tLeafBrush);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cLeafBrushLump].mOffset, SEEK_SET);
	
	//Allocate memory
	pMap->mLeafBrushes = (tLeafBrush*)malloc(pMap->mHeader.mLumpes[cLeafBrushLump].mLength);

	for(int i = 0; i < lNbLeafBrushes; ++i)
	{
		fread(&pMap->mLeafBrushes[i], 1, sizeof(tLeafBrush), pFile);
		//fix endian
		fixEndian(&pMap->mLeafBrushes[i], 1, sizeof(int));
	}	
}

void readModel(FILE* pFile, TMapQ3 * pMap)
{
	int	lNbModels = pMap->mHeader.mLumpes[cModelLump].mLength / sizeof(tModel);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cModelLump].mOffset, SEEK_SET);
	//Allocate memory
	pMap->mModels = (tModel*)malloc(pMap->mHeader.mLumpes[cModelLump].mLength);

	for(int i = 0; i < lNbModels; ++i)
	{
		fread(&pMap->mModels[i], 1, sizeof(TModel), pFile);
		//Fix endian
		fixEndian(pMap->mModels[i].mMins, 3, sizeof(float));
		fixEndian(pMap->mModels[i].mMaxs, 3, sizeof(float));
		fixEndian(&pMap->mModels[i].mFace, 4, sizeof(int));
		//fix coordinates
		for(u8 j= 0; j < 3; j++)
		{
			pMap->mModels[i].mMins[j] *= -1.0f;
			pMap->mModels[i].mMaxs[j] *= -1.0f;
		}
	}
}

void readVertex(FILE* pFile, TMapQ3 * pMap)
{
	int	lNbVertices = pMap->mHeader.mLumpes[cVertexLump].mLength / sizeof(tVertex);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cVertexLump].mOffset, SEEK_SET);
	
	//Allocate memory
	pMap->mVertices = (tVertex*)malloc(pMap->mHeader.mLumpes[cVertexLump].mLength);

	for(int i = 0; i < lNbVertices; ++i)
	{
		fread(&pMap->mVertices[i], 1, sizeof(tVertex), pFile);
		//fix endian
		fixEndian(pMap->mVertices[i].mPosition,3,sizeof(float));
		fixEndian(pMap->mVertices[i].mNormal,3,sizeof(float));
		fixEndian(pMap->mVertices[i].mTexCoord[0],2,sizeof(float));
		fixEndian(pMap->mVertices[i].mTexCoord[1],2,sizeof(float));
		//fix coordinates
		for(u8 j = 0; j < 3; j++)
		{
			pMap->mVertices[i].mPosition[j]*=-1;
			pMap->mVertices[i].mNormal[j]*=-1;
		}
	}
}

void readMeshVert(FILE* pFile, TMapQ3 * pMap)
{
	int	lNbMeshVertices = pMap->mHeader.mLumpes[cMeshVertLump].mLength / sizeof(tMeshVert);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cMeshVertLump].mOffset, SEEK_SET);
	
	//Allocate memory
	pMap->mMeshVertices = (tMeshVert*)malloc(pMap->mHeader.mLumpes[cMeshVertLump].mLength);

	for(int i = 0; i < lNbMeshVertices; ++i)
	{
		fread(&pMap->mMeshVertices[i], 1, sizeof(tMeshVert), pFile);
		//Fix endian
		fixEndian(&pMap->mMeshVertices[i],1,sizeof(int));
	}
}

void readFace(FILE * pFile, TMapQ3 * pMap)
{
	int	lNbFaces = pMap->mHeader.mLumpes[cFaceLump].mLength / sizeof(TFace);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cFaceLump].mOffset, SEEK_SET);
	//Allocate memory
	pMap->mFaces = (tFace*)malloc(pMap->mHeader.mLumpes[cFaceLump].mLength);

	for(int i = 0; i < lNbFaces; ++i)
	{
		fread(&pMap->mFaces[i], 1, sizeof(tFace), pFile);
		//fix endian
		fixEndian(&pMap->mFaces[i], 8, sizeof(int));
		fixEndian(pMap->mFaces[i].mLightmapCorner, 2, sizeof(int));
		fixEndian(pMap->mFaces[i].mLightmapSize, 2, sizeof(int));
		fixEndian(pMap->mFaces[i].mLightmapOrigin, 3, sizeof(float));
		fixEndian(pMap->mFaces[i].mLightmapVecs[0], 3, sizeof(float));
		fixEndian(pMap->mFaces[i].mLightmapVecs[1], 3, sizeof(float));
		fixEndian(pMap->mFaces[i].mNormal, 3, sizeof(float));
		fixEndian(pMap->mFaces[i].mPatchSize, 2, sizeof(int));
		//fix coordinates
		for(u8 j=0;j<3;j++)
		{
			pMap->mFaces[i].mLightmapOrigin[j] *= -1.0f;
			pMap->mFaces[i].mLightmapVecs[0][j] *= -1.0f;
			pMap->mFaces[i].mLightmapVecs[1][j] *= -1.0f;
			pMap->mFaces[i].mNormal[j] *= -1.0f;
		}
	}
}

void readLightMap(FILE* pFile, TMapQ3 * pMap)
{
	int	lNbLightMaps = pMap->mHeader.mLumpes[cLightMapLump].mLength / sizeof(tLightMap);

	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cLightMapLump].mOffset, SEEK_SET);
	//Allocate memory
	pMap->mLightMaps = (tLightMap*)malloc(pMap->mHeader.mLumpes[cLightMapLump].mLength);
	pMap->mRealLightMaps = (GXTexObj*)memalign(32, sizeof(GXTexObj)*lNbLightMaps);

	for(int i = 0; i < lNbLightMaps; ++i)
	{
		fread(&pMap->mLightMaps[i], 1, sizeof(tLightMap), pFile);
		u8 * buf01 = (u8*)memalign(32, 128*128*4);
		for(u8 j = 0; j < 128; j++)
		for(u8 k = 0; k < 128; k++)
		{
			buf01[(j+128*k)*4+0] = pMap->mLightMaps[i].mMapData[k][j][0];
			buf01[(j+128*k)*4+1] = pMap->mLightMaps[i].mMapData[k][j][1];
			buf01[(j+128*k)*4+2] = pMap->mLightMaps[i].mMapData[k][j][2];
			buf01[(j+128*k)*4+3] = 255;
		}
		tiling4x4(buf01, TILE_RGBA8, 128, 128);
		GX_InitTexObj(&pMap->mRealLightMaps[i], (void*)buf01, 128, 128, GX_TF_RGBA8, GX_CLAMP, GX_CLAMP, GX_FALSE);
	}
}

void readVisData(FILE* pFile, TMapQ3 * pMap)
{
	// Go to the start of the chunk.
	fseek(pFile, pMap->mHeader.mLumpes[cVisDataLump].mOffset, SEEK_SET);

	fread(&pMap->mVisData.mNbClusters, 1, sizeof(int), pFile);
	fread(&pMap->mVisData.mBytesPerCluster, 1, sizeof(int), pFile);
	//Fix endian
	fixEndian(&pMap->mVisData.mNbClusters, 2, sizeof(int));

	// Allocate the buffer.
	int lBufferSize = pMap->mVisData.mNbClusters * pMap->mVisData.mBytesPerCluster;
	pMap->mVisData.mBuffer = new unsigned char[lBufferSize];

	fread(pMap->mVisData.mBuffer, lBufferSize, sizeof(unsigned char), pFile);
}

u8 readMap(const char * filename, tMapQ3 * pMap)
{
	FILE * bspFile = fopen(filename, "rb");
	if(!bspFile)
	{
		return 0;
	}
	//Header
	if(readHeader(bspFile, pMap) == false)
	{
		return 0;
	}
	// Read the entity lump.
	readEntity(bspFile, pMap);
	
	// Read the texture lump.
	readTexture(bspFile, pMap);
	
	// Read the plane lump.
	readPlane(bspFile, pMap);
	
	// Read the node lump.
	readNode(bspFile, pMap);

	// Read the leaf lump.
	readLeaf(bspFile, pMap);

	// Read the leaf face lump.
	readLeafFace(bspFile, pMap);

	// Read the leaf brush lump.
	readLeafBrush(bspFile, pMap);

	// Read the model lump.
	readModel(bspFile, pMap);

	/*/ Read the brush lump.
	readBrush(bspFile, pMap);

	// Read the brushside lump.
	readBrushSide(bspFile, pMap);*/

	// Read the vertex lump.
	readVertex(bspFile, pMap);

	// Read the meshvert lump.
	readMeshVert(bspFile, pMap);

	/*/ Read the effect lump.
	readEffect(bspFile, pMap);*/

	// Read the face lump.
	readFace(bspFile, pMap);

	// Read the lightmap lump.
	readLightMap(bspFile, pMap);

	/*/ Read the lightvol lump.
	readLightVol(bspFile, pMap);*/

	// read the visdata lump.
	readVisData(bspFile, pMap);
	
	// Close the file.
	fclose(bspFile);
	return 1;
}

int findLeaf(tMapQ3 * map, Vector cam)
{
	int idx = 0;
	while(idx >= 0)
	{
		tNode * n = &map->mNodes[idx];
		tPlane * p = &map->mPlanes[idx];
		f32 dst = sqrt(guVecDotProduct(&cam, (Vector*)p->mNormal)) - p->mDistance;
		if(dst >= 0) idx = n->mChildren[0];
		else idx = n->mChildren[1];
	}
	return -idx-1;
}

u8 isClusterVisible(tMapQ3 * map, int visCluster, int testCluster)
{
	//To do
	return 1;
}

void renderQ3Map(Vector cam, tMapQ3 * map)
{
	//int camLeafIdx = findLeaf(map, cam);
	//Set vertex descriptors
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	//Set tev
	GX_SetNumTevStages(1);
	GX_SetNumChans(1);
	GX_SetChanCtrl(GX_COLOR0A0,GX_DISABLE, GX_SRC_VTX, GX_SRC_VTX, GX_LIGHTNULL, GX_DF_CLAMP, GX_AF_NONE );
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetCullMode(GX_CULL_FRONT);
	//Render every point in the scene
	Vector pos;
	//u8 clr;
	int nFaces = map->mHeader.mLumpes[cFaceLump].mLength/sizeof(tFace);
	int index = 0;
	for(int i = 0; i<nFaces; i++)
	{
		tFace *fc = &map->mFaces[i];
		
		if(fc->mType == 3)//Only render polygon faces
		{
			if(fc->mNbVertices >= 3)
			{
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, fc->mNbMeshVertices);
			
				for(int j=0; j < fc->mNbMeshVertices; j++)
				{
					index = fc->mVertex+map->mMeshVertices[fc->mMeshVertex +j].mMeshVert;
					tVertex vtx = map->mVertices[index];
					pos = vector3(vtx.mPosition[0], vtx.mPosition[1], vtx.mPosition[2]);
					GX_Position3f32(pos.x, pos.y, pos.z);
					GX_Color4u8(vtx.mColor[0], vtx.mColor[1], vtx.mColor[2], vtx.mColor[3]);
				}
				
				GX_End();
			}
		}
	}
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_BLEND);
	GX_LoadTexObj(&map->mRealLightMaps[0], GX_TEXMAP0);
	int ltIdx = 0;
	//int txIdx = 0;
	for(int i = 0; i<nFaces; i++)
	{
		tFace *fc = &map->mFaces[i];
		if(fc->mLightmapIndex != ltIdx)
		{
			ltIdx = fc->mLightmapIndex;
			GX_LoadTexObj(&map->mRealLightMaps[ltIdx], GX_TEXMAP0);
		}
		if(fc->mType == 1)//Only render polygon faces
		{
			if(fc->mNbVertices >= 3)
			{
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, fc->mNbMeshVertices);
			
				for(int j=0; j < fc->mNbMeshVertices; j++)
				{
					index = fc->mVertex+map->mMeshVertices[fc->mMeshVertex +j].mMeshVert;
					tVertex vtx = map->mVertices[index];
					pos = vector3(vtx.mPosition[0], vtx.mPosition[1], vtx.mPosition[2]);
					GX_Position3f32(pos.x, pos.y, pos.z);
					GX_Color4u8(vtx.mColor[0], vtx.mColor[1], vtx.mColor[2], vtx.mColor[3]);
					GX_TexCoord2f32(vtx.mTexCoord[1][0], vtx.mTexCoord[1][1]);
				}
				
				GX_End();
			}
		}
	}
}
