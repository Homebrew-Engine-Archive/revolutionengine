///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Models header file

#ifndef REV_MODELS_H
#define REV_MODELS_H

//New implementation, better, more efficient and powerfull
//Header files//header files
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <math.h>
#include <unistd.h>

#include <vector>

#include "REVUtils.h"
#include "REVPhysics.h"

//Forward declarations
class IMaterial;
class TModel;

class TMesh
{
public:
	TMesh();
	~TMesh();
	void		render();
	u8			getSlot()	{ return m_materialSlot; }
protected:
	u16		*	m_verts;//Indices
	u16		*	m_normals;//Indices
	u16		*	m_uvs;//Indices
	
	u16			m_nTris;
	u16			m_nQuads;

	u16			m_materialSlot;
friend TModel * loadRMD(const char * filename, bool loadMaterials);
};

class TModel
{
public:
	TModel		();
	~TModel		();
	const Vector&	getCenter	()const { return getBBox().getCenter(); }
	const Vector&	getSize		()const { return getBBox().getSize(); }
	TBBox&			getBBox		()const;
	void			scale		(f32 _factor);
	void			scale		(const Vector& _factor);
	
	void			render		(std::vector<IMaterial*> materials, GXColor& clr, u32 _lightMask);
private:
	std::vector<TMesh*> m_vMeshes;
protected:
	f32		*	m_vertices;
	f32		*	m_normals;
	f32		*	m_uvs;

	u16			m_nVertices;
	u16			m_nNormals;
	u16			m_nUVs;
private:
	u16			m_nMeshes;
friend TModel * loadRMD(const char * filename, bool loadMaterials);
};

TModel * loadRMD(const char * filename, bool loadMaterials = false);

#endif
