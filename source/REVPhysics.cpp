///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Physics

#include "REV.h"

typedef struct _tracker{
	NODE * target;
	struct _tracker * next;
}TRACKER;

//Global internal data
TRACKER * queue = NULL;

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