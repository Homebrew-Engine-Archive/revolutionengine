///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Md5 format header file

#ifndef REV_MD5_H
#define REV_MD5_H

#define MD5_VERSION		10

/*typedef struct REV_Joint
{
	char name[64];
	s16 parent;
	Vector pos;
	QUATERNION orient;
}JOINT;

typedef struct REV_AnimatedVertex
{
	f32 s, t;
	u16 start, count;
}AVTX;

typedef struct REV_AnimatedFace
{
	u16 idx[3];
}AFACE;

typedef struct REV_VertexWeight
{
	u16 joint;
	f32 bias;

	Vector pos;
}AVWEIGHT;

class AMESH
{
public:
	AVTX 	*aVertices;
	AFACE 	*faces;
	AVWEIGHT 	*weights;
	Vector		*sNormals;

	f32	* vertices;			// Array de vertices
	f32	* coords;		// Array de coordenadas de textura
	f32	* normals;		// Array de normales
	
	GXColor * diffuse;

	u16 *vList;			// Indices de vertices

	u16 nVertices;
	u16 nFaces;
	u16 nWeights;

};

class AMODEL:public MODEL
{
public:
	AMODEL(const char * fileName = NULL, u8 format = FF_MD5);
	~AMODEL();
	void scale(f32 factor);
	void scale(Vector factor);
private:
	JOINT * baseSkeleton;
	AMESH * meshes;
	u16 nJoints;
	u16 nMeshes;
	void render(Mtx absMtx, Vector camPos, u8 clrChannels = 0, u8 texChannels = 0, f32 specularity = 1.0f);
	void prepare(AMESH * mesh, JOINT * skeleton);
	void diffuseLight(Mtx absMtx);
	void specularLightsClr(Mtx absMtx, f32 specularity, Vector camPos, AMESH * mesh, u16 vIdx, u16 nIdx);
	void shadowRender();
friend void render(NODE * node);
friend void computeNormals(AMODEL * model, AMESH * mesh);
};

void computeNormals(AMODEL * modelo, AMESH * mesh);*/

#endif
