///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Models header file

#ifndef REV_MODELS_H
#define REV_MODELS_H

class UVSET
{
public:
	f32 * uvs;
	u16 * uvList;
	u16 nUVs;
};

class SMESH//Static mesh, no animations
{
public:
	f32		*	vertices;
	f32		*	normals;
	UVSET	*	uvSets;
	
	u16		*	vList;
	u16		*	nList;

	u16			nVertices;
	u16			nNormals;
	u16			nTris;
	u16			nQuads;
};

class MODEL
{
public:
	u8 flags;
	u8 getType();
	virtual void scale(f32 factor)=0;
	virtual void scale(Vector factor)=0;
protected:
	u8 nMeshes;
	u8 nUVSets;
	u8 type;
private:
	virtual void render(Mtx absMtx, Vector camPos, u8 clrChannels, u8 texChannels, f32 specularity = 1.0f) = 0;
	virtual void shadowRender()=0;
friend void render(NODE * node, Vector camPos);
friend class ROOT;
};

class SMODEL:public MODEL
{
public:
	SMODEL(const char * fileName = NULL, u8 format = FF_RMS);
	~SMODEL();
	void	scale			(f32 factor);
	void	scale			(Vector factor);
	f32		getRadius		();
	Vector	getCenter		();
private:
	SMESH * meshes;
	void	render			(Mtx absMtx, Vector camPos, u8 clrChannels = 0, u8 texChannels = 0, f32 specularity = 1.0f);
	void	specularLightsClr(Mtx absMtx, f32 specularity, Vector camPos, SMESH * mesh, u16 vIdx, u16 nIdx);
	void	shadowRender	();
friend void render			(NODE * node);
friend class ROOT;
};

#endif
