///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Quake 3 bsp loader
//Based on simple q3 bsp loader by nicholas baudrey
#ifndef REV_QUAKE3_BSP_H
#define REV_QUAKE3_BSP_H

//Structs
typedef struct _tLump
{
	int					mOffset;
	int					mLength;
}tLump;

typedef struct _tHeader
{
	char				mMagicNumber[4];	//  Magic number. Always "IBSP".  
	int					mVersion;  			//	Version number 0x2e for the BSP files distributed with Quake 3.  
	tLump				mLumpes[17];		//	Lump directory, seventeen entries.
}tHeader;

typedef struct _tEntity
{
	int					mSize;				// Size of the description.
	char*				mBuffer;			// Entity descriptions, stored as a string. 
}tEntity;//Probably will skip this in Rev

typedef struct _tTexture
{
	char				mName[64];			// Texture name.  
	int					mFlags;				// Surface flags.  
	int					mContents;			// Content flags.  
}tTexture;//This will have to be adapted into the engine

typedef struct _tPlane
{
	float				mNormal[3];			// Plane normal.  
	float				mDistance;			// Distance from origin to plane along normal.  
}tPlane;

/**
 * Node of the Q3 map.
 * The nodes lump stores all of the nodes in the map's BSP tree.
 */
typedef struct _tNode
{
	int					mPlane;				// Plane index.  
	int					mChildren[2];		// Children indices. Negative numbers are leaf indices: -(leaf+1).  
	int					mMins[3];			// Integer bounding box min coord.
	int					mMaxs[3];			// Integer bounding box max coord.  
}tNode;

typedef struct _tLeaf
{
	int					mCluster;			// Visdata cluster index.
	int					mArea;				// Areaportal area.
	int					mMins[3];			// Integer bounding box min coord.
	int					mMaxs[3];			// Integer bounding box max coord.
	int					mLeafFace;			// First leafface for leaf.
	int					mNbLeafFaces;		// Number of leaffaces for leaf.
	int					mLeafBrush;			// First leafbrush for leaf.
	int					mNbLeafBrushes;		// Number of leafbrushes for leaf.
}tLeaf;

typedef struct TLeafFace
{
	int					mFaceIndex;			// Face index.  
}tLeafFace;

/**
 * Leaf Brush of the Q3 map.
 * The leafbrushes lump stores lists of brush indices, with one list per leaf. 
 */
typedef struct TLeafBrush
{
	int					mBrushIndex;		// Brush index.  
}tLeafBrush;

typedef struct TQModel
{
	float				mMins[3];			// Bounding box min coord.  
	float				mMaxs[3];			// Bounding box max coord.  
	int					mFace;				// First face for model.  
	int					mNbFaces;			// Number of faces for model.  
	int					mBrush;				// First brush for model.  
	int					mNBrushes;			// Number of brushes for model.  

}tModel;

/**
 * Brush of the Q3 map.
 * The brushes lump stores a set of brushes, which are in turn used for collision detection.
 */
typedef struct TBrush
{
	int					mBrushSide;			// First brushside for brush.  
	int					mNbBrushSides;		// Number of brushsides for brush.  
	int					mTextureIndex;		// Texture index.  
}tBrush;

typedef struct TBrushSide
{
	int					mPlaneIndex;		// Plane index.  
	int					mTextureIndex;		// Texture index.  
}tBrushSide;

/**
 * Vertex of the Q3 map.
 * The vertexes lump stores lists of vertices used to describe faces.
 */
typedef struct TVertex
{
	float				mPosition[3];		// Vertex position.  
	float				mTexCoord[2][2];	// Vertex texture coordinates. 0 = Surface, 1 = Lightmap.  
	float				mNormal[3];			// Vertex normal.  
	unsigned char		mColor[4];			// Vertex color (RGBA).  
}tVertex;

typedef struct TMeshVert
{
	int					mMeshVert;			// Vertex index offset, relative to first vertex of corresponding face.
}tMeshVert;

/**
 * Effect of the Q3 map.
 * The effects lump stores references to volumetric shaders (typically fog) which affect the rendering of a particular group of faces.
 */
typedef struct TEffect
{
	char				mName[64];			// Effect shader.  
	int					mBrush;				// Brush that generated this effect.  
	int					mUnknown;			// Always 5, except in q3dm8, which has one effect with -1.  
}tEffect;

/**
 * Face of the Q3 map.
 * The faces lump stores information used to render the surfaces of the map.
 */
typedef struct TFace
{
	int					mTextureIndex;		// Texture index.  
	int					mEffectIndex;		// Index into lump 12 (Effects), or -1.  
	int					mType;				// Face type. 1 = Polygon, 2 = Patch, 3 = Mesh, 4 = Billboard. 
	int					mVertex;			// Index of first vertex.  
	int					mNbVertices;		// Number of vertices.  
	int					mMeshVertex;		// Index of first meshvert.  
	int					mNbMeshVertices;	// Number of meshverts.  
	int					mLightmapIndex;		// Lightmap index.  
	int					mLightmapCorner[2];	// Corner of this face's lightmap image in lightmap.  
	int					mLightmapSize[2];	// Size of this face's lightmap image in lightmap.  
	float				mLightmapOrigin[3];	// World space origin of lightmap.  
	float				mLightmapVecs[2][3];// World space lightmap s and t unit vectors.  
	float				mNormal[3];			// Surface normal.  
	int					mPatchSize[2];		// Patch dimensions.  

}tFace;

/**
 * Lightmap of the Q3 map.
 * The lightmaps lump stores the light map textures used make surface lighting look more realistic.
 */
typedef struct TLightMap
{
	unsigned char		mMapData[128][128][3];// Lightmap color data. RGB. 
}tLightMap;

/**
 * Light volume of the Q3 map.
 * The lightvols lump stores a uniform grid of lighting information used to illuminate non-map objects.
 */
typedef struct TLightVol
{
	unsigned char		mAmbient[3];		// Ambient color component. RGB.  
	unsigned char		mDirectional[3];	// Directional color component. RGB.  
	unsigned char		mDir[2];			// Direction to light. 0=phi, 1=theta.  
}tLightVol;

/**
 * The Visibility data of the Q3 map.
 * The visdata lump stores bit vectors that provide cluster-to-cluster visibility information.
 */
typedef struct TVisData
{
	int					mNbClusters;		// The number of clusters
    int					mBytesPerCluster;	// Bytes (8 bits) in the cluster's bitset
    unsigned char*		mBuffer;			// Array of bytes holding the cluster vis.
}tVisData;

typedef struct TMapQ3
{
	
	tHeader					mHeader;		// Header of the file.
	tEntity					mEntity;		// Entitys.
	tTexture				*mTextures;		// Array of the textures.
	tPlane					*mPlanes;		// Array of the planes.
	tNode					*mNodes;		// Array of the nodes.
	tLeaf					*mLeaves;		// Array of the leaves.
	tLeafFace				*mLeafFaces;	// Array of the leaf faces.
	tLeafBrush				*mLeafBrushes;	// Array of the leaf brushes.
	tModel					*mModels;		// Array of the models.
	tBrush					*mBrushes;		// Array of the brushes.
	tBrushSide				*mBrushSides;	// Array of the brush sides.
	tVertex					*mVertices;		// Array of the vertices.
	tMeshVert				*mMeshVertices;	// Array of the mesh vertices.
	tEffect					*mEffects;		// Array of the effects.
	tFace					*mFaces;		// Array of the faces.
	tLightMap				*mLightMaps;	// Array of the light maps.
	tLightVol				*mLightVols;	// Array of the light volumes.
	tVisData				mVisData;		// The visibility datas.
	GXTexObj 				*mRealLightMaps;//TIled and converted
	GXTexObj				**mGXTextures;    //Array of pointers
}tMapQ3;

//Constants
#define MagicNumber ("IBSP")
#define Q3Version 0x2e

const int					cEntityLump		= 0x00; // Entities : Game-related object descriptions.  
const int					cTextureLump	= 0x01; // Textures : Surface descriptions.  
const int					cPlaneLump		= 0x02; // Planes : Planes used by map geometry.  
const int					cNodeLump		= 0x03; // Nodes : BSP tree nodes.  
const int					cLeafLump		= 0x04; // Leafs : BSP tree leaves.  
const int					cLeafFaceLump	= 0x05; // LeafFaces : Lists of face indices, one list per leaf.  
const int					cLeafBrushLump	= 0x06; // LeafBrushes  Lists of brush indices, one list per leaf.  
const int					cModelLump		= 0x07; // Models  Descriptions of rigid world geometry in map.  
const int					cBrushLump		= 0x08; // Brushes  Convex polyhedra used to describe solid space.  
const int					cBrushSideLump	= 0x09; // Brushsides  Brush surfaces.  
const int					cVertexLump		= 0x0A; // Vertexes  Vertices used to describe faces.  
const int					cMeshVertLump	= 0x0B; // MeshVerts  Lists of offsets, one list per mesh.  
const int					cEffectLump		= 0x0C; // Effects  List of special map effects.  
const int					cFaceLump		= 0x0D; // Faces  Surface geometry.  
const int					cLightMapLump	= 0x0E; // LightMaps  Packed lightmap data.  
const int					cLightVolLump	= 0x0F; // LightVols  Local illumination data.  
const int					cVisDataLump	= 0x10; // Visdata  Cluster-cluster visibility data.

u8 readMap(const char * filename, tMapQ3 * pMap);
void freeMap(tMapQ3 * pMap);
u8 isValid(tMapQ3 * pMap);

//Render
void renderQ3Map(Vector cam, tMapQ3 * pMap);
#endif
