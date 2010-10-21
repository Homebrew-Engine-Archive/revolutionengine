///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Models source file

//header files
#include "REV.h"

//Definitions
#define LINESIZE 60

//Prototypes
void getValidRmdLine(FILE * f);

//Global data
char linebuffer[LINESIZE];

u8 MODEL::getType()
{
	return type;
}

SMODEL::SMODEL(const char * fileName, u8 format)
{
	UVSET * uv;
	Vector * tNml;
	flags = 0;
	type = MDT_STATIC;
	u16 u16data, i, j;
	if(!fileName)
	{
		type = MDT_BAD;
		return;
	}
	switch(format)
	{
		case FF_RMS:
		{
			FILE * f = fopen(fileName, "rt");
			if(!f)
			{
				type = MDT_BAD;
				return;
			}
			nMeshes = 1;
			nUVSets = 1;
			meshes = (SMESH*)malloc(sizeof(SMESH));
			meshes->uvSets = (UVSET*)malloc(sizeof(UVSET));
			uv = meshes->uvSets;
			char * datatype;
			f32 version = 0;
			u16 verts = 0, texts = 0, normals = 0, faces = 0;
			
			meshes->nVertices = 0;
			meshes->nNormals = 0;
			uv->nUVs = 0;
			meshes->nTris = 0;
			meshes->nQuads = 0;
			
			//Una vez declarado todo lo que vamos a usar empezamos a explorar el archivo.
			while(!feof(f))
			{
				fgets(linebuffer, LINESIZE, f);
				if(!feof(f))
				{
					datatype = strtok(linebuffer," ");
					if(datatype[0] != '#')//Si una linea empieza por # es que es un comentario, por tanto no se procesa
					{//En caso contrario
						if(datatype[0] == 'm')//Estamos ante la informacion de version de archivo
						{
							version = atof(strtok(NULL, " "));
						}
						if(datatype[0] == 'q')
						{
							if(datatype[1] == 'v')
							{
								meshes->nVertices = atoi(strtok(NULL, " "));
								meshes->vertices = (f32*)memalign(32, meshes->nVertices*sizeof(f32)*3);
							}
							if(datatype[1] == 'n')
							{
								meshes->nNormals = atoi(strtok(NULL, " "));
								meshes->normals = (f32*)memalign(32, meshes->nNormals*sizeof(f32)*3);
							}
							if(datatype[1] == 't')
							{
								uv->nUVs = atoi(strtok(NULL, " "));
								uv->uvs = (f32*)memalign(32, uv->nUVs*sizeof(f32)*2);
							}
							if(datatype[1] == 'f')
							{
								meshes->nTris = atoi(strtok(NULL, " "));
								meshes->vList = (u16*)memalign(32, meshes->nTris*sizeof(u16)*3);
								meshes->nList = (u16*)memalign(32, meshes->nTris*sizeof(u16)*3);
								uv->uvList = (u16*)memalign(32, meshes->nTris*sizeof(u16)*3);
							}
						}
						if(datatype[0] == 'v')
						{if(verts < meshes->nVertices){
							meshes->vertices[verts*3] = (f32)atof(strtok(NULL, " "));
							meshes->vertices[verts*3+1] = (f32)atof(strtok(NULL, " "));
							meshes->vertices[verts*3+2] = (f32)atof(strtok(NULL, " "));
							verts ++;
						}}
						if(datatype[0] == 'n')
						{if(normals < meshes->nNormals){
							meshes->normals[normals*3] = (f32)atof(strtok(NULL, " "));
							meshes->normals[normals*3+1] = (f32)atof(strtok(NULL, " "));
							meshes->normals[normals*3+2] = (f32)atof(strtok(NULL, " "));
							normals ++;
						}}
						if(datatype[0] == 't')
						{ if(texts < uv->nUVs){
							uv->uvs[texts*2] = (f32)atof(strtok(NULL, " "));
							uv->uvs[texts*2+1] = (f32)atof(strtok(NULL, " "));
							texts ++;
						}}
						if(datatype[0] == 'f')
						{if(faces < meshes->nTris){
							meshes->vList[faces*3+2] = atoi(strtok(NULL, "/"));
							meshes->nList[faces*3+2] = atoi(strtok(NULL, "/"));
							uv->uvList[faces*3+2] = atoi(strtok(NULL, " "));
							
							meshes->vList[faces*3+1] = atoi(strtok(NULL, "/"));
							meshes->nList[faces*3+1] = atoi(strtok(NULL, "/"));
							uv->uvList[faces*3+1] = atoi(strtok(NULL, " "));
							
							meshes->vList[faces*3+0] = atoi(strtok(NULL, "/"));
							meshes->nList[faces*3+0] = atoi(strtok(NULL, "/"));
							uv->uvList[faces*3+0] = atoi(strtok(NULL, " "));
							faces++;
						}}
					}
				}
			}//////*/
			fclose(f);
			//normalize normals
			for(normals=0;normals<meshes->nNormals;normals++)
			{
				tNml = (Vector*)(&meshes->normals[3*normals]);
				guVecNormalize(tNml);
			}
			DCFlushRange(meshes->vertices, meshes->nVertices*sizeof(f32)*3);
			DCFlushRange(meshes->normals, meshes->nNormals*sizeof(f32)*3);
			DCFlushRange(uv->uvs, uv->nUVs*sizeof(f32)*2);
			break;
		}
		case FF_RMD:
		{
			FILE * f = fopen(fileName, "rt");
			if(!f)
			{
				type = MDT_BAD;
				return;
			}
			nMeshes = 1;
			getValidRmdLine(f);
			sscanf(linebuffer, "id 5 1 %hu", &u16data);
			nUVSets = (u8)u16data;
			meshes = (SMESH*)malloc(sizeof(SMESH));
			meshes->uvSets = (UVSET*)malloc(sizeof(UVSET) * nUVSets);
			getValidRmdLine(f);
			sscanf(linebuffer, "q %hu %hu %hu %hu", &meshes->nVertices,
				&meshes->nNormals, &meshes->nTris, &meshes->nQuads);
			meshes->vertices = (f32*)memalign(32, sizeof(f32)*3*meshes->nVertices);
			meshes->normals = (f32*)memalign(32, sizeof(f32)*3*meshes->nNormals);
			meshes->vList = (u16*)memalign(32, sizeof(u16) *3*meshes->nTris);
			meshes->nList = (u16*)memalign(32, sizeof(u16) *3*meshes->nTris);
			for(i=0;i<meshes->nVertices;i++)
			{
				getValidRmdLine(f);
				sscanf(linebuffer, "v %f %f %f", &meshes->vertices[3*i],
					&meshes->vertices[3*i+1], &meshes->vertices[3*i+2]);
			}
			for(i=0;i<meshes->nNormals;i++)
			{
				getValidRmdLine(f);
				sscanf(linebuffer, "n %f %f %f", &meshes->normals[3*i],
					&meshes->normals[3*i+1], &meshes->normals[3*i+2]);
			}
			for(i=0;i<nUVSets;i++)
			{
				uv = &meshes->uvSets[i];
				getValidRmdLine(f);
				sscanf(linebuffer, "uvn %hu", &uv->nUVs);
				uv->uvs = (f32*)memalign(32, sizeof(f32) * 2 * uv->nUVs);
				uv->uvList = (u16*)memalign(32, sizeof(u16) *3*meshes->nTris);
				for(j = 0; j < uv->nUVs; j++)
				{
					getValidRmdLine(f);
					sscanf(linebuffer, "uv %f %f", &uv->uvs[2*j], &uv->uvs[2*j+1]);
				}
				for(j = 0; j < meshes->nTris; j++)
				{
					getValidRmdLine(f);
					sscanf(linebuffer, "uvt %hu %hu %hu", &uv->uvList[3*j],
						&uv->uvList[3*j+1], &uv->uvList[3*j+2]);
				}
			}
			for(i = 0; i < meshes->nTris; i++)
			{
				getValidRmdLine(f);
				sscanf(linebuffer, "ft %hu/%hu %hu/%hu %hu/%hu",
					&meshes->vList[3*i], &meshes->nList[3*i],
					&meshes->vList[3*i+1], &meshes->nList[3*i+1],
					&meshes->vList[3*i+2], &meshes->nList[3*i+2]);
			}
			fclose(f);
			DCFlushRange(meshes->vertices, meshes->nVertices*sizeof(f32)*3);
			DCFlushRange(meshes->normals, meshes->nNormals*sizeof(f32)*3);
			DCFlushRange(uv->uvs, uv->nUVs*sizeof(f32)*2);
			break;
		}
		default:
		{
			type = MDT_BAD;
			return;
		}
	}
}

SMODEL::~SMODEL()
{
	for(u16 i = 0; i < nMeshes; i++)
	{
		SMESH * tMesh = &(meshes[i]);
		if(tMesh)
		{
			if(tMesh->vertices) free(tMesh->vertices);
			if(tMesh->vList) free(tMesh->vList);
			if(tMesh->normals) free(tMesh->normals);
			if(tMesh->nList) free(tMesh->nList);
			for(u16 j= 0; j < nUVSets; j++)
			{
				UVSET * uv = &(tMesh->uvSets[j]);
				if(uv)
				{
					if(uv->uvList) free(uv->uvList);
					if(uv->uvs) free(uv->uvs);
					free(uv);
				}
			}
			free(tMesh);
		}
	}
}

Vector SMODEL::getCenter()
{
	Vector c = nullVector;
	Vector t;
	for(int i = 0; i < nMeshes; i++)
	{
		SMESH * m = &meshes[i];
		f32 nV = 1.0f/m->nVertices;
		for(int j = 0; j < m->nVertices; j++)
		{
			t =  *((Vector*)(&m->vertices[j*3]));
			c.x+=t.x*nV;
			c.y+=t.y*nV;
			c.z+=t.z*nV;
		}
	}
	return c;
}

f32 SMODEL::getRadius()
{
	f32 r = 0.0f;
	f32 mod;
	Vector t;
	for(int i = 0; i < nMeshes; i++)
	{
		SMESH * m = &meshes[i];
		for(int j = 0; j < m->nVertices; j++)
		{
			t =  *((Vector*)(&m->vertices[j*3]));
			mod = t.x*t.x + t.y*t.y + t.z*t.z;
			if(mod > r) r = mod;
		}
	}
	return sqrt(r);
}

void SMODEL::scale(f32 factor)
{
	for(u8 i = 0; i<nMeshes; i++)
	{
		SMESH * mesh = &meshes[i];
		for(u16 j = 0; j < mesh->nVertices; j++)
		{
			mesh->vertices[j*3] *= factor;
			mesh->vertices[j*3+1] *= factor;
			mesh->vertices[j*3+2] *= factor;
		}
		DCFlushRange(mesh->vertices, mesh->nVertices*sizeof(f32)*3);
	}
}

void SMODEL::scale(Vector factor)
{
	for(u8 i = 0; i<nMeshes; i++)
	{
		SMESH * mesh = &meshes[i];
		for(u16 j = 0; j < mesh->nVertices; j++)
		{
			mesh->vertices[j*3] *= factor.x;
			mesh->vertices[j*3+1] *= factor.y;
			mesh->vertices[j*3+2] *= factor.z;
		}
		DCFlushRange(mesh->vertices, mesh->nVertices*sizeof(f32)*3);
	}
}

void SMODEL::shadowRender()
{
	//Vertex descriptors
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);//We only need positions of the vertices
	for(u8 i = 0; i < nMeshes; i++)
	{
		SMESH * mesh = &meshes[i];
		GX_SetArray(GX_VA_POS, mesh->vertices, 3*sizeof(f32));
		GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nTris);
		for(u16 j = 0; j<mesh->nTris; j++)
		{
			for(u8 k = 0; k<3; k++)
			{
				GX_Position1x16(mesh->vList[j*3+k]);
			}
		}
		GX_End();
	}
}

void SMODEL::render(Mtx absMtx, Vector camPos, u8 clrChannels, u8 texChannels, f32 specularity)
{
	GX_SetVtxDesc(GX_VA_POS, GX_INDEX16);
	GX_SetVtxDesc(GX_VA_NRM, GX_INDEX16);
	
	switch(clrChannels)
	{
		case CC_DIFF + CC_SPEC://Both channels
		{
			for(u8 i = 0; i < nMeshes; i++)
			{
				SMESH * mesh = &meshes[i];
				GX_SetArray(GX_VA_POS, mesh->vertices, 3*sizeof(f32));
				GX_SetArray(GX_VA_NRM, mesh->normals, 3*sizeof(f32));
				if(texChannels)
					GX_SetArray(GX_VA_TEX0, mesh->uvSets->uvs, 2*sizeof(f32));
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nTris);
				for(u16 j = 0; j<mesh->nTris; j++)
				{
					for(u8 k = 0; k<3; k++)
					{
						GX_Position1x16(mesh->vList[j*3+k]);
						GX_Normal1x16(mesh->nList[j*3+k]);
						specularLightsClr(absMtx, specularity, camPos, mesh, mesh->vList[j*3+k], mesh->nList[3*j+k]);
						if(texChannels)
						{
							GX_TexCoord1x16(mesh->uvSets->uvList[3*j+k]);
						}
					}
				}
				GX_End();
			}
			break;
		}
		case CC_SPEC://Specular channel
		{
			GX_SetChanCtrl(GX_COLOR0A0,GX_FALSE,GX_SRC_VTX,GX_SRC_VTX,GX_LIGHTNULL,GX_DF_NONE,GX_AF_NONE);
			for(u8 i = 0; i < nMeshes; i++)
			{
				SMESH * mesh = &meshes[i];
				GX_SetArray(GX_VA_POS, mesh->vertices, 3*sizeof(f32));
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nTris);
				for(u16 j = 0; j<mesh->nTris; j++)
				{
					for(u8 k = 0; k<3; k++)
					{
						GX_Position1x16(mesh->vList[j*3+k]);
						GX_Normal1x16(mesh->nList[j*3+k]);
						specularLightsClr(absMtx, specularity, camPos, mesh, mesh->vList[j*3+k], mesh->nList[3*j+k]);
					}
				}
				GX_End();
			}
			break;
		}
		case CC_DIFF://Diffuse channel
		{
			for(u8 i = 0; i < nMeshes; i++)
			{
				SMESH * mesh = &meshes[i];
				GX_SetArray(GX_VA_POS, mesh->vertices, 3*sizeof(f32));
				GX_SetArray(GX_VA_NRM, mesh->normals, 3*sizeof(f32));
				if(texChannels)
					GX_SetArray(GX_VA_TEX0, mesh->uvSets->uvs, 2*sizeof(f32));
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nTris);
				for(u16 j = 0; j<mesh->nTris; j++)
				{
					for(u8 k = 0; k<3; k++)
					{
						GX_Position1x16(mesh->vList[j*3+k]);
						GX_Normal1x16(mesh->nList[j*3+k]);
						if(texChannels)
						{
							GX_TexCoord1x16(mesh->uvSets->uvList[3*j+k]);
						}
					}
				}
				GX_End();
			}
			break;
		}
		case 0://No channel
		{
			for(u8 i = 0; i < nMeshes; i++)
			{
				SMESH * mesh = &meshes[i];
				GX_SetArray(GX_VA_POS, mesh->vertices, 3*sizeof(f32));
				GX_SetArray(GX_VA_NRM, mesh->normals, 3*sizeof(f32));
				if(texChannels)
					GX_SetArray(GX_VA_TEX0, mesh->uvSets->uvs, 2*sizeof(f32));
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nTris);
				for(u16 j = 0; j<mesh->nTris; j++)
				{
					for(u8 k = 0; k<3; k++)
					{
						GX_Position1x16(mesh->vList[j*3+k]);
						GX_Normal1x16(mesh->nList[j*3+k]);
						if(texChannels)
						{
							GX_TexCoord1x16(mesh->uvSets->uvList[3*j+k]);
						}
					}
				}
				GX_End();
			}
			break;
		}
	}
}

void SMODEL::specularLightsClr(Mtx absMtx, f32 specularity, Vector camPos, SMESH * mesh, u16 vIdx, u16 nIdx)
{
	LIGHT * auxLight = mainRoot->fstLight;
	f32 exp, cosA, r0 = 0,g0 = 0,b0 = 0,r1,g1,b1, a0;
	u8 a, r, g, b;
	Vector lDir, normal, tNormal, *list, vDir, pos, tPos, half;
	while(auxLight)
	{
		if(auxLight->flags & F_Active)
		{
		r1 = auxLight->clr.r;
		g1 = auxLight->clr.g;
		b1 = auxLight->clr.b;
		//Get Light Direction
		switch(auxLight->type)
		{
			case LT_DIR:
			{
				lDir = auxLight->getPos(RT_ABS);
				break;
			}
			default:
			{
				lDir = vector3(0,0,-1);
			}
		}
		//Get Normal
		list = (Vector*)mesh->normals;
		tNormal = list[nIdx];
		guVecMultiplySR(absMtx,&tNormal,&normal);
		//Compute view direction
		list = (Vector*)mesh->vertices;
		tPos = list[vIdx];
		guVecMultiply(absMtx,&tPos,&pos);
		guVecSub(&pos, &camPos, &vDir);
		guVecNormalize(&vDir);
		//Compute half-angle vector
		guVecAdd(&vDir, &lDir, &half);
		guVecNormalize(&half);
		if((cosA = -guVecDotProduct(&half,&normal)) > 0)
		{
			exp = pow(cosA, specularity);
			r0 += r1 * exp;
			g0 += g1 * exp;
			b0 += b1 * exp;
		}}
		/////
		auxLight = auxLight->nextL;
	}
	a0 = (r0+g0+b0)*0.333333f;//Have to improve this conversion
	if(a0 > 255) a = 255; else a = (u8)a0;
	if(r0 > 255) r = 255; else r = (u8)r0;
	if(g0 > 255) g = 255; else g = (u8)g0;
	if(b0 > 255) b = 255; else b = (u8)b0;
	GX_Color4u8/*(125,125,125,125);*/(r, g, b, a);
}

void getValidRmdLine(FILE * f)
{
	do
	{
		fgets(linebuffer, LINESIZE, f);
	}
	while(linebuffer[0] == '#');
}
