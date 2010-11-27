///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Models source file

#include "REVExternal.h"
#include "REVModels.h"
#include "REVMaterials.h"
#include "REVLights.h"

#include "REVConsole.h"

#include "REV.h"

#include "REVConfig.h"

#include <map>

//New implementation, faster, better.
//-------------------------------------------------------------------------
void getValidRmdLine(FILE * f, char * dstBuffer)
{
	do
	{
		fgets(dstBuffer, MAX_PATH, f);
	}
	while(dstBuffer[0] == '#');
}

//-------------------------------------------------------------------------
void TModel::log()
{
	char msg[MAX_PATH];
	sprintf(msg, "model %hu %hu %hu %hu", m_nVertices, m_nNormals, m_nUVs, m_nMeshes);
	REVConsole->write(msg);
}

//-------------------------------------------------------------------------
TModel * loadRMD(const char * _filename, bool _loadMaterials)
{
	//Variables used
	char linebuffer[MAX_PATH];
	std::multimap<u8, TMesh*> tempBuffer;
	FILE * f = fopen(_filename, "rt");
	if(!f)
	{
		REVConsole->write("file not found");
		return NULL;//Couldn't load
	}
	//Is this model binary or plain text?
	getValidRmdLine(f, linebuffer);
	if(linebuffer[0] == 'b')//Binaries not yet supported
	{
		fclose(f);
		return NULL;
	}
	//If not binary, we assume text
	getValidRmdLine(f, linebuffer);
	if(linebuffer[0] != '6')//Only version 6
	{
		fclose(f);
		return NULL;
	}
	//Create the model
	TModel * model = new TModel;
	//Get data header
	getValidRmdLine(f, linebuffer);
	sscanf(linebuffer, "%hu %hu %hu %hu", &model->m_nVertices, &model->m_nNormals, &model->m_nUVs, &model->m_nMeshes);
	//Allocate memory for data
	model->m_vertices = (f32*)memalign(32, 3*sizeof(f32)*model->m_nVertices);
	model->m_normals = (f32*)memalign(32, 3*sizeof(f32)*model->m_nNormals);
	model->m_uvs = (f32*)memalign(32, 3*sizeof(f32)*model->m_nUVs);
	if(!model->m_nVertices || !model->m_nNormals || !model->m_nUVs)
	{
#ifdef _DEBUG
		REVConsole->write("Error allocating memory");
#endif
		delete model;
		fclose(f);
		return NULL;
	}
	//Fill that data
	//vertices
	for(unsigned i = 0; i < model->m_nVertices; ++i)
	{
		getValidRmdLine(f, linebuffer);
		sscanf(linebuffer, "v %f %f %f", &model->m_vertices[3*i+0],
											&model->m_vertices[3*i+1],
											&model->m_vertices[3*i+2]);
	}
	//normals
	for(unsigned i = 0; i < model->m_nNormals; ++i)
	{
		getValidRmdLine(f, linebuffer);
		sscanf(linebuffer, "n %f %f %f", &model->m_normals[3*i+0],
											&model->m_normals[3*i+1],
											&model->m_normals[3*i+2]);
	}
	//texture coordinates
	for(unsigned i = 0; i < model->m_nUVs; ++i)
	{
		getValidRmdLine(f, linebuffer);
		sscanf(linebuffer, "t %f %f", &model->m_uvs[2*i+0],
											&model->m_uvs[2*i+1]);
	}
	//Flush memory
	DCFlushRange(model->m_vertices, model->m_nVertices*sizeof(f32)*3);
	DCFlushRange(model->m_normals, model->m_nNormals*sizeof(f32)*3);
	DCFlushRange(model->m_uvs, model->m_nUVs*sizeof(f32)*2);
#ifdef _DEBUG
	for(u16 i = 0; i < model->m_nUVs; ++i)
	{
		sprintf(linebuffer,  "t %f %f", model->m_uvs[2*i], model->m_uvs[2*i+1]);
		REVConsole->write(linebuffer);
	}
#endif
	//Now parse every mesh
	for(unsigned i = 0; i < model->m_nMeshes; ++i)
	{
//#ifdef NDEBUG
		REVConsole->write("mesh loaded");
//#endif
		//Read the header
		getValidRmdLine(f, linebuffer);
		TMesh * mesh = new TMesh();
		char material[MAX_PATH];//Unused
		sscanf(linebuffer, "m %hu %hu %hu %s", &mesh->m_nTris,
			&mesh->m_nQuads, &mesh->m_materialSlot, material);
		//Allocate memory for indices
		u16 nVerts = 3*mesh->m_nTris + 4*mesh->m_nQuads;
		mesh->m_verts = (u16*)memalign(32, nVerts*sizeof(u16));
		mesh->m_normals = (u16*)memalign(32, nVerts*sizeof(u16));
		mesh->m_uvs = (u16*)memalign(32, nVerts*sizeof(u16));
		//TODO: Test allocation succeded
		//Load data
		unsigned nTris = 0, nQuads = 0;
		for(unsigned i = 0; i < nVerts;)
		{
			getValidRmdLine(f, linebuffer);
			if(linebuffer[0] == '3')//Triangle
			{
				sscanf(linebuffer, "3 %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
				&mesh->m_verts[nTris],&mesh->m_normals[nTris],&mesh->m_uvs[nTris],
				&mesh->m_verts[nTris+1],&mesh->m_normals[nTris+1],&mesh->m_uvs[nTris+1],
				&mesh->m_verts[nTris+2],&mesh->m_normals[nTris+2],&mesh->m_uvs[nTris+2]);
				++nTris;
				i+=3;
			}
			else if(linebuffer[0] == '4')//Quad
			{
				int ret =
				sscanf(linebuffer, "4 %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu %hu/%hu/%hu",
				&mesh->m_verts[nTris], &mesh->m_normals[nTris],&mesh->m_uvs[nTris],
				&mesh->m_verts[nTris+1],&mesh->m_normals[nTris+1],&mesh->m_uvs[nTris+1],
				&mesh->m_verts[nTris+2],&mesh->m_normals[nTris+2],&mesh->m_uvs[nTris+2],
				&mesh->m_verts[nTris+3],&mesh->m_normals[nTris+3],&mesh->m_uvs[nTris+3]);
				++nQuads;
				i+=4;
			}
		}
		//Flush memory
		DCFlushRange(mesh->m_verts, (3*mesh->m_nTris+4*mesh->m_nQuads)*sizeof(u16));
		DCFlushRange(mesh->m_normals, (3*mesh->m_nTris+4*mesh->m_nQuads)*sizeof(u16));
		DCFlushRange(mesh->m_uvs, (3*mesh->m_nTris+4*mesh->m_nQuads)*sizeof(u16));
		//Data is ready, push_back this model
		tempBuffer.insert(pair<u8, TMesh*>(mesh->m_materialSlot, mesh));
		sprintf(linebuffer, "v=%hu,q=%hu", mesh->m_nTris, mesh->m_nQuads);
		REVConsole->write(linebuffer);
	}
	//Copy the buffer into the final vector
	std::multimap<u8, TMesh*>::iterator iter = tempBuffer.begin();
	model->m_vMeshes.clear();
	for(; iter != tempBuffer.end(); ++iter)
	{
		model->m_vMeshes.push_back((*iter).second);
		sprintf(linebuffer, "adress %u", (void*)(*iter).second);
		REVConsole->write(linebuffer);
	}
	//close the file
	fclose(f);
	//And return the model
	return model;
}

//-------------------------------------------------------------------------
TMesh::TMesh()
{
	m_nTris = 0;
	m_nQuads = 0;
	m_materialSlot = 0;
	//Index lists
	m_verts = 0;
	m_normals = 0;
	m_uvs = 0;
}

//-------------------------------------------------------------------------
TMesh::~TMesh()
{//TODO: free resources
}

//-------------------------------------------------------------------------
void TMesh::render()
{
	GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*m_nTris);
	/*if(!done)
	{
		sprintf(msg, "adress %u", (void*)this);
		REVConsole->write(msg);
		done = true;
	}*/
	for(u16 j = 0; j< m_nTris; ++j)
	{
		for(u8 k = 0; k<3; ++k)
		{
			GX_Position1x16(m_verts[j*3+k]-1);
			GX_Normal1x16(m_normals[j*3+k]-1);
			GX_TexCoord1x16(m_uvs[3*j+k]-1);
		}
	}
	GX_End();
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4*3);//To do, only one GX_Begin/End for each model
	u16 quad0 = 3*m_nTris;
	for(u16 j = 0; j < 3; ++j)
	{
		for(u8 k = 0; k<4; ++k)
		{
			REVGX_Position1x16(m_verts[quad0+j*4+k]-1);
			GX_Normal1x16(m_normals[quad0+j*4+k]-1);
			GX_TexCoord1x16(m_uvs[quad0+4*j+k]-1);
		}
	}
	GX_End();
}

//-------------------------------------------------------------------------
TModel::TModel()
{
	m_nMeshes = 0;
	m_nVertices = 0;
	m_nNormals = 0;
	m_nUVs = 0;
	m_vertices = NULL;
	m_normals = NULL;
	m_uvs = NULL;
}

//-------------------------------------------------------------------------
TModel::~TModel()
{
	free(m_vertices);
	free(m_normals);
	free(m_uvs);
	//Free all meshes
}

//-------------------------------------------------------------------------
void TModel::render(std::vector<IMaterial*> _materials)
{
	//Set vertex descriptors
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_TEX0, GX_INDEX16);
	//Set arrays
	GX_SetArray(GX_VA_POS, m_vertices, 3*sizeof(f32));
	GX_SetArray(GX_VA_NRM, m_normals, 3*sizeof(f32));
	GX_SetArray(GX_VA_TEX0, m_uvs, 2*sizeof(f32));
	//Iterate through meshes
	s32 lastSlot = -1;
	std::vector<TMesh*>::iterator iter = m_vMeshes.begin();
	TTevInfo renderInfo;
	for(; iter != m_vMeshes.end(); ++iter)
	{
		if((*iter)->getSlot() != lastSlot)
		{//Load material
			lastSlot = (*iter)->getSlot();
			//Get material for the new slot
			IMaterial * mat;
			if(_materials.empty()) mat = NULL;
			else if(((s32)_materials.capacity()) <= lastSlot) mat = _materials[0];
			else mat = _materials[lastSlot];
			//Adjust TEV
			if(mat)
				mat->setTev(renderInfo);
			else 
			{
				REVConsole->write("no material");
				return;//By now, we don't paint non-material meshes
			}
			//TODO: default set TEV
		}
		(*iter)->render();
	}
}
