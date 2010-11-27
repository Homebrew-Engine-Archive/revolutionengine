///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Md5 source file

/*#include "REV.h"

AMODEL::AMODEL(const char * fileName, u8 format)
{
	switch(format)
	{
		case FF_MD5:
		{
			FILE * f;
			char buff[512];
			u32 version = 0;
			u16 currMesh = 0;
			type = MDT_ANIM;
			f = fopen(fileName, "rb");
			if(!f)
			{
				type = MDT_ANIM;
				return;
			}
			while ( !feof ( f ) )
			{
				fgets ( buff, sizeof ( buff ), f);
				
				if ( sscanf ( buff, " MD5Version %d", &version ) == 1 )
				{
					if ( version != MD5_VERSION )
					{
						fprintf (stderr, "Error: version del modelo incorrecta\n");
						type = MDT_BAD;
						fclose (f);
						return;
					}
				}
				else if ( sscanf ( buff, " numJoints %hu", &nJoints) == 1 )
				{
					if (nJoints > 0 )
					{
						baseSkeleton = (JOINT*)malloc(sizeof(JOINT) * nJoints);
					}
				}
				else if ( sscanf ( buff, " numMeshes %hu", &nMeshes) == 1 )
				{
					if (nMeshes > 0)
					{
						meshes = (AMESH*)malloc(sizeof(AMESH) * nMeshes );
					}
				}
				else if ( strncmp ( buff, "joints {", 8) == 0 )
				{
					for (u16 i = 0; i < nJoints; ++i )
					{
						JOINT * joint = &baseSkeleton[i];
						fgets ( buff, sizeof(buff), f);
						if ( sscanf ( buff, "%s %hd ( %f %f %f ) ( %f %f %f )",
							joint->name, &joint->parent, &joint->pos.x,
							&joint->pos.y, &joint->pos.z, &joint->orient.x,
							&joint->orient.y, &joint->orient.z) == 8 )
						{
							// Calcula el componente w
							joint->orient.wCompute();
						}
					}
				}
				else if ( strncmp ( buff, "mesh {", 6) == 0 )
				{
					AMESH * mesh = &meshes[currMesh];
					u16 vIndex = 0;
					u16 fIndex = 0;
					u16 wIndex = 0;
					f32 fdata[4];
					u16 idata[3];
					
					while ((buff[0] != '}') && !feof(f) )
					{
						// Leemos una linea entera
						fgets (buff, sizeof(buff), f);
						
						if ( sscanf ( buff, " numverts %hu", &mesh->nVertices ) == 1 )
						{
							if ( mesh->nVertices > 0 )
							{
								// Reserva memoria para los vertices
								mesh->aVertices = (AVTX*)malloc(sizeof(AVTX) * mesh->nVertices);
							}
						}
						else if ( sscanf ( buff, " numtris %hu", &mesh->nFaces ) == 1 )
						{
							if (mesh->nFaces > 0)
							{
								// Reservamos memomria para los triangulos
								mesh->faces = (AFACE*)malloc(sizeof(AFACE) * mesh->nFaces);
							}
						}
						else if ( sscanf ( buff, " numweights %hu", &mesh->nWeights ) == 1 )
						{
							if ( mesh->nWeights > 0 )
							{
								// Reservamos memomria para los pesos
								mesh->weights = (AVWEIGHT*)malloc(sizeof(AVWEIGHT) * mesh->nWeights );
							}
						}
						else if ( sscanf ( buff, " vert %hu ( %f %f ) %hu %hu", &vIndex, &fdata[0], &fdata[1], &idata[0], &idata[1] ) == 5 )
						{
							// Copiamos los datos de los vertices
							mesh->aVertices[vIndex].s  = fdata[0];
							mesh->aVertices[vIndex].t  = fdata[1];
							mesh->aVertices[vIndex].start = idata[0];
							mesh->aVertices[vIndex].count  = idata[1];
						}
						else if ( sscanf ( buff, " tri %hu %hu %hu %hu", &fIndex, &idata[0], &idata[1], &idata[2] ) == 4 )
						{
							// Copiamos los datos de los triangulos
							mesh->faces[fIndex ].idx[0] = idata[0];
							mesh->faces[fIndex ].idx[1] = idata[1];
							mesh->faces[fIndex ].idx[2] = idata[2];
						}
						else if ( sscanf ( buff, " weight %hu %hu %f ( %f %f %f )", &wIndex, &idata[0], &fdata[3], &fdata[0], &fdata[1], &fdata[2] ) == 6 )
						{
							// Copiamos los datos de los vertices
							mesh->weights[wIndex].joint  = idata[0];
							mesh->weights[wIndex].bias     = fdata[3];
							mesh->weights[wIndex].pos.x   = fdata[0];
							mesh->weights[wIndex].pos.y   = fdata[1];
							mesh->weights[wIndex].pos.z   = fdata[2];
						}
					}
					currMesh++;
				}
			}
			for(u16 i=0; i < nMeshes; i++)
			{
				AMESH *mesh = &meshes[i];
				
				mesh->vertices 	= (f32*)memalign(32,sizeof(f32) * 3 * mesh->nVertices);
				mesh->coords	= (f32*)memalign(32,sizeof(f32) * 2 * mesh->nVertices);
				mesh->normals	= (f32*)memalign(32,sizeof(f32) * 3 * mesh->nVertices);
				mesh->diffuse	= (GXColor*)memalign(32, sizeof(GXColor) * mesh->nVertices);
				
				mesh->vList = (u16*)memalign(32,sizeof(u16) * 3 * mesh->nFaces);
				
				computeNormals(this, mesh);
			}
			
			fclose ( f );
			break;
		}
	}
}

void AMODEL::shadowRender()
{
}

void crossProduct(Vector a, Vector b, Vector *ab)
{
	ab->x = (a.y*b.z) - (a.z*b.y);
	ab->y = (a.z*b.x) - (a.x*b.z);
	ab->z = (a.x*b.y) - (a.y*b.x);
}

void computeNormals(AMODEL * modelo, AMESH * mesh)
{
	int i = 0, j = 0;
	//work out smooth shading pervertex normals
	mesh->sNormals = (Vector*)memalign(32, sizeof(Vector) * mesh->nVertices );
	Vector * vertices = (Vector*)memalign(32, sizeof(Vector) * mesh->nVertices );

	// Calculamos las posiciones finales de los vértices
	for ( i = 0; i < mesh->nVertices; ++i )
	{
		Vector Verticefinal = { 0.0f, 0.0f, 0.0f };

		// Calcula el vertice final para dibujar con pesos
		for ( j = 0; j < mesh->aVertices[i].count; ++j )
		{
			AVWEIGHT * peso = &mesh->weights[mesh->aVertices[i].start + j];
			JOINT * uniones = modelo->baseSkeleton;

			// Calcula el vertice transformado para este peso
			Vector wv;
			uniones->orient.rotatePoint(&peso->pos, &wv);

			// La suma de todos los peso->bias debe ser 1.0
			Verticefinal.x += (uniones->pos.x + wv.x) * peso->bias;
			Verticefinal.y += (uniones->pos.y + wv.y) * peso->bias;
			Verticefinal.z += (uniones->pos.z + wv.z) * peso->bias;
		}

		vertices[i] = Verticefinal;
	}

	// Reservamos memoria para las normales de las caras
	Vector * Fnormales = (Vector*)memalign(32, sizeof(Vector) * mesh->nFaces );

	for ( i = 0; i < mesh->nFaces; i++ )
	{
		Vector resultado;
		Vector ab;
		Vector ac;
		Vector a,b,c;

		// Vertice 1
		a.x = vertices[mesh->faces[i].idx[0]].x;
		a.y = vertices[mesh->faces[i].idx[0]].y;
		a.z = vertices[mesh->faces[i].idx[0]].z;

		// Verice 2
		b.x = vertices[mesh->faces[i].idx[1]].x;
		b.y = vertices[mesh->faces[i].idx[1]].y;
		b.z = vertices[mesh->faces[i].idx[1]].z;

		// Vertice 3 
		c.x = vertices[mesh->faces[i].idx[2]].x;
		c.y = vertices[mesh->faces[i].idx[2]].y;
		c.z = vertices[mesh->faces[i].idx[2]].z;

		// Calcula el vector normal al poligono
		guVecSub(&b, &a, &ab);
		guVecSub(&c, &b, &ac);

		crossProduct(ab, ac, &resultado);

		guVecNormalize(&resultado);//Maybe we can remove this

		Fnormales[i] = resultado;
	}

	for ( i=0; i < mesh->nVertices; i++)
	{
		Vector comp_normal;

		comp_normal.x = comp_normal.y = comp_normal.z = 0.0f;

		for ( j = 0; j < mesh->nFaces; j++)
		{
			// Buscamos si un triangulo tiene este vertice y añadimos su normal
			if((i == mesh->faces[j].idx[0]) || (i == mesh->faces[j].idx[1]) || (i == mesh->faces[j].idx[2]))
			{
				guVecAdd(&comp_normal, &Fnormales[j], &comp_normal);
			}
		}
		// Normalizamos
		guVecNormalize(&comp_normal);

		mesh->sNormals[i] = comp_normal;
	}

	// Liberamos la memoria temporal
	free(vertices);
	free(Fnormales);
}

void AMODEL::scale(f32 factor)
{
}

void AMODEL::scale(Vector factor)
{
}

void AMODEL::render(Mtx absMtx, Vector camPos, u8 clrChannels, u8 texChannels, f32 specularity)
{
	for(u16 i = 0; i < nMeshes; i++)
	{
		prepare(&meshes[i], baseSkeleton);
	}
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_NRM, GX_DIRECT);
	
	switch(clrChannels)
	{
		case CC_DIFF://diffuse
		{
			for(u8 i = 0; i < nMeshes; i++)
			{
				AMESH * mesh = &meshes[i];
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nFaces);
				if(texChannels)
					GX_SetArray(GX_VA_TEX0, mesh->coords, 2*sizeof(f32));
				for(u16 j = 0; j<mesh->nFaces; j++)
				{
					for(u8 k = 0; k<3; k++)
					{
						GX_Position3f32(mesh->vertices[3*(mesh->vList[j*3+k])+0],
										mesh->vertices[3*(mesh->vList[j*3+k])+1],
										mesh->vertices[3*(mesh->vList[j*3+k])+2]);
						GX_Normal3f32(mesh->normals[3*(mesh->vList[j*3+k])+0],
										mesh->normals[3*(mesh->vList[j*3+k])+1],
										mesh->normals[3*(mesh->vList[j*3+k])+2]);
						if(texChannels)
						{
							GX_TexCoord1x16(mesh->vList[3*j+k]);
						}
					}
				}
				GX_End();
			}
			break;
		}
		case CC_DIFF+CC_SPEC://diffuse, review this
		{
			for(u8 i = 0; i < nMeshes; i++)
			{
				AMESH * mesh = &meshes[i];
				GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3*mesh->nFaces);
				for(u16 j = 0; j<mesh->nFaces; j++)
				{
					for(u8 k = 0; k<3; k++)
					{
						GX_Position3f32(mesh->vertices[3*(mesh->vList[j*3+k])+0],
										mesh->vertices[3*(mesh->vList[j*3+k])+1],
										mesh->vertices[3*(mesh->vList[j*3+k])+2]);
						GX_Color4u8(mesh->diffuse[mesh->vList[3*j+k]].r,
									mesh->diffuse[mesh->vList[3*j+k]].g,
									mesh->diffuse[mesh->vList[3*j+k]].b,
									255);
						//GX_Color4u8(127,127,127,127);
						specularLightsClr(absMtx, specularity, camPos, mesh, j*3+k, j*3+k);
					}
				}
				GX_End();
			}
			break;
		}
	}
}

void AMODEL::prepare(AMESH * mesh, JOINT * skeleton)
{
	int i, j, k;

	// Configura los indices de vertices
	for ( k = 0, i = 0; i < mesh->nFaces; ++i )
	{
		for ( j = 0; j < 3; ++j, ++k )
			mesh->vList[k] = mesh->faces[i].idx[j];
	}

	// Configuramos los vertices
	for ( i = 0; i < mesh->nVertices; ++i )
	{
		Vector Verticefinal = { 0.0f, 0.0f, 0.0f };
		Vector new_norm = { 0.0f, 0.0f, 0.0f };
		Vector tmp_norm = mesh->sNormals[i];

		mesh->coords[(i*2)+0] = mesh->aVertices[i].s;
		mesh->coords[(i*2)+1] = 1.0-mesh->aVertices[i].t;

		// Calcula el vertice final para dibujar con pesos
		for ( j = 0; j < mesh->aVertices[i].count; ++j )
		{
			AVWEIGHT *peso = &mesh->weights[mesh->aVertices[i].start + j];
			JOINT *uniones = &skeleton[peso->joint];

			// Calcula el vertice transformado para este peso
			Vector wv;
			Vector wn;

			uniones->orient.rotatePoint(&peso->pos, &wv);
			uniones->orient.rotatePoint(&tmp_norm, &wn);

			// La suma de todos los peso->bias debe ser 1.0
			Verticefinal.x += (uniones->pos.x + wv.x) * peso->bias;
			Verticefinal.y += (uniones->pos.y + wv.y) * peso->bias;
			Verticefinal.z += (uniones->pos.z + wv.z) * peso->bias;

			new_norm.x += wn.x * peso->bias;
			new_norm.y += wn.y * peso->bias;
			new_norm.z += wn.z * peso->bias;
		}

		mesh->vertices[(i*3)+0] = Verticefinal.x;
		mesh->vertices[(i*3)+1] = Verticefinal.y;
		mesh->vertices[(i*3)+2] = Verticefinal.z;

		mesh->normals[(i*3)+0] = new_norm.x;
		mesh->normals[(i*3)+1] = new_norm.y;
		mesh->normals[(i*3)+2] = new_norm.z;
	}
}

void AMODEL::specularLightsClr(Mtx absMtx, f32 specularity, Vector camPos, AMESH * mesh, u16 vIdx, u16 nIdx)
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
	GX_Color4u8(r, g, b, a);
}*/
