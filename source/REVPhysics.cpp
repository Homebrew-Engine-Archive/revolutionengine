///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Physics

#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ogcsys.h>
#include <gccore.h>
#include <math.h>
#include <wiiuse/wpad.h>
#include <fat.h>
#include <pngu.h>
#include <time.h>
#include <ogc/lwp_watchdog.h>
#include <sys/dir.h>
#include <sys/statvfs.h>
#include <wiikeyboard/keyboard.h>
#include <unistd.h>

#include "REVUtils.h"
#include "REVTime.h"
#include "REVPhysics.h"
#include "REVNodes.h"

#include "REV.h"//Will be replaced b REVRoot.h

typedef struct _tracker{
	NODE * target;
	struct _tracker * next;
}TRACKER;

//Global internal data
TRACKER * queue = NULL;

const Vector& TBBox::getSize()const
{
	return vector3((max.x-min.x)*0.5f,(max.y-min.y)*0.5f,(max.z-min.z)*0.5f);
}

const Vector& TBBox::getCenter()const
{
	return vector3((max.x+min.x)*0.5f,(max.y+min.y)*0.5f,(max.z+min.z)*0.5f);
}

void TBBox::reset()
{
	max = nullVector;
	min = nullVector;
	valid = false;
}

void TBBox::adjust(Vector& v)
{
	if(valid)
	{
		max.x = revMax(max.x, v.x);
		max.y = revMax(max.y, v.y);
		max.z = revMax(max.z, v.z);

		min.x = revMin(min.x, v.x);
		min.y = revMin(min.y, v.y);
		min.z = revMin(min.z, v.z);
	}
	else
	{
		max = v;
		min = v;
		valid = true;
	}
}

void updatePhysicState()
{
	TRACKER * t = queue;
	NODE * n;
	while(t)
	{
		n = t->target;
		t = t->next;
		Vector move = vector3(n->speed.x*gameTime, n->speed.y*gameTime, n->speed.z*gameTime);
		n->move(move, RT_ABS);
	}
}

void addNodeToPhysiscs(NODE * n)
{
	NODE * child = n->getChild();
	while(child)
	{
		addNodeToPhysiscs(child);
		child = child->getNext();
	}
	if(n->mode)
	{
		TRACKER * t = (TRACKER*)malloc(sizeof(TRACKER));
		t->target = n;
		t->next = queue;
		queue = t;
	}
}

void updatePhysics()
{
	//Create the queue
	queue = NULL;
	addNodeToPhysiscs(mainRoot->rootNode);
	//Update state
	updatePhysicState();
	//Check collisions
	TRACKER * aux = queue, * aux2;
	while(queue)
	{
		aux = queue->next;
		//Check collisions with the queue
		aux2 = queue;
		while(aux)
		{
			aux->target->collide(aux2->target);
			aux = aux->next;
		}
		aux = queue->next;
		free(queue);
		queue = aux;
	}
}

//Body types
SPHEREBD::SPHEREBD(f32 radius)
{
	this->radius = radius;
	this->type = BD_SPHERE;
}

void SPHEREBD::setInertia(Mtx dst, f32 mass)
{
	f32 I = 0.4*mass*radius*radius;//(2*m*r^2)/5, inertia of a solid sphere
	guMtxScale(dst, I, I, I);
}

INFPLANEBD::INFPLANEBD()
{
	type = BD_INFPLANE;
}

f32 INFPLANEBD::getRadius()
{
	return INFINITY;//Infinite plane, infinite radius
}

void INFPLANEBD::setInertia(Mtx dst, f32 mass)
{//Since the plane is infinite, it can not be moved (or stopped if it's moving)
	f32 I = INFINITY;
	guMtxScale(dst, I, I, I);
}