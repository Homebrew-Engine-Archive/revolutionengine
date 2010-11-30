///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Nodes source file
#include "REV.h"

NODE::NODE(u8 flags)
{
	this->flags = flags;
	type = NT_NODE;
	guMtxIdentity(base);
	child = NULL;
	if(mainRoot->rootNode)
	{
		next = mainRoot->rootNode->child;
		mainRoot->rootNode->child = this;
		parent = mainRoot->rootNode;
	}
	else
	{
		next = NULL;
		parent = NULL;
	}
	custom = NULL;
	//Physics
	mass = 0;
	guMtxIdentity(inertia);
	speed = nullVector;
	aSpeed = nullVector;
	body = NULL;
	mode = PX_SOLID;
}

NODE::~NODE()
{
	if(!parent)//This is root node
	{
		clearChildren();
		return;
	}
	releaseChildren();
	if(parent->child == this)
	{
		parent->child = next;
	}
	else
	{
		NODE * aux = parent->child;
		while(aux->next != this)
		{
			aux = aux->next;
		}
		aux->next = next;
	}
}

void NODE::attach(NODE * parent, u8 coord)
{
	deattach(coord);
	mainRoot->rootNode->child = next;
	next = parent->child;
	parent->child = this;
	this->parent = parent;
	if(coord)
	{
		Mtx m;
		parentMtx(m);
		guMtxInverse(m, m);
		guMtxConcat(m, base, base);
	}
}

void NODE::addChild(NODE * child, u8 coord)
{
	child->attach(this, coord);
}

void NODE::deattach(u8 coord)
{
	if(coord)
	{
		Mtx m;
		parentMtx(m);
		guMtxConcat(m, base, base);
	}
	if(parent->child == this)
	{
		parent->child = next;
	}
	else
	{
		NODE * aux = parent->child;
		while(aux->next != this)
		{
			aux = aux->next;
		}
		aux->next = next;
	}
	parent = mainRoot->rootNode;
	next = mainRoot->rootNode->child;
	mainRoot->rootNode->child = this;
}

void NODE::releaseChildren(u8 coord)
{
	while(child)
	{
		child->deattach(coord);
	}
}

void NODE::clearChildren()
{
	while(child)
	{
		child->clearChildren();
		delete child;
	}
}

void NODE::move(Vector move, u8 reference)
{
	Vector v;
	switch(reference)
	{
		case RT_SELF:
		{
			guVecMultiplySR(base, &move, &v);
			break;
		}
		case RT_PAR:
		{
			v = move;
			break;
		}
		case RT_ABS:
		{
			Mtx m;
			parentMtx(m);
			guVecMultiplySR(m, &move, &v);
			break;
		}
		default:
		{
			v = nullVector;
		}
	}
	guMtxTransApply(base, base, v.x, v.y, v.z);
}

void NODE::rotate(Vector axis, f32 angle, u8 reference)
{
	Vector pos = getPos(RT_PAR);
	guMtxTransApply(base, base, -pos.x, -pos.y, -pos.z);
	Mtx aux;
	switch(reference)
	{
		case RT_SELF:
		{
			guVecMultiply(base, &axis, &axis);
			break;
		}
		case RT_ABS://Absolute
		{
			parentMtx(aux);
			guVecMultiply(aux, &axis, &axis);
			break;
		}
	}
	guMtxRotAxisDeg(aux, &axis, angle);
	guMtxConcat(aux , base, base);
	guMtxTransApply(base, base, pos.x, pos.y, pos.z);
}

void NODE::setPos(Vector pos, u8 reference)
{
	Mtx aux;
	if(!reference)
	{
		parentMtx(aux);
		guMtxInverse(aux, aux);
		guVecMultiply(aux, &pos, &pos);
	}
	base[0][3] = pos.x;
	base[1][3] = pos.y;
	base[2][3] = pos.z;
}

Vector NODE::getPos(u8 reference)
{
	Vector v = {base[0][3], base[1][3], base[2][3]};
	if(!reference)
	{
		Mtx aux;
		parentMtx(aux);
		guVecMultiply(aux, &v, &v);
	}
	return v;
}

void NODE::parentMtx(Mtx m)
{
	guMtxIdentity(m);
	NODE * aux = parent;
	while(aux)
	{
		guMtxConcat(aux->base, m, m);
		aux = aux->parent;
	}
}

void NODE::absMtx(Mtx m)
{
	parentMtx(m);
	guMtxConcat(m, base, m);
}

void NODE::srMtx(Mtx m)
{
	absMtx(m);
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
}

void NODE::lookAt(Vector look)
{
	base[0][1] = look.x;
	base[1][1] = look.y;
	base[2][1] = look.z;
	//
	base[0][0] = look.y;
	base[1][0] = -look.x;
	base[2][0] = 0;
	//
	base[0][2] = -look.x*look.z;
	base[1][2] = -look.y*look.z;
	base[2][2] = look.x*look.x;
}

//Physics about nodes
f32 NODE::collide(NODE * n)
{
char msg[16];
	if(!body)//We consider this node as a dimensionless point
	{
		if(!n->body)//Point to point collision
			return  -1;//We assume that non-dimensional points do not collide since space is too big for that
		else return n->collide(this);
	}
	else switch(body->type)
	{
		case BD_SPHERE:
		{
			if(!n->body)//Point to sphere collision
			{//Not supported yet
				Vector pos0 = getPos(RT_ABS), pos1 = n->getPos(RT_ABS);
				Vector spd0 = getSpeed(RT_ABS), spd1 = n->getSpeed(RT_ABS);
				guVecSub(&spd1, &spd0, &spd0);
				guVecSub(&pos1, &pos0, &pos0);
				f32 radius = this->body->getRadius();
				f32 a = guVecDotProduct(&spd0, &spd0);
				f32 b = 2*guVecDotProduct(&pos0, &spd0);
				f32 c = guVecDotProduct(&pos0, &pos0);
				f32 d = radius*radius;
				f32 t = gameTime;
				if(c+t*t*a-t*b <= d)
				{
					//Commit to the time these bodies collided
					/*if((t = (2*a*(c-d))))
					{
						sprintf(msg, "%f", t);
						REVConsole->write(msg);
						t = (b-sqrt(b*b-4*a*(c-d)))/t;//Simple 2nd degree ecuation, then change sign
						//Commit the sphere back to collision
						spd0 = vector3(5,0,0);//getSpeed();
						guVecScale(&spd0, &spd0, t);
						move(spd0, RT_ABS);
						spd0 = n->getSpeed();
						guVecScale(&spd0, &spd0, t);
						n->move(spd0, RT_ABS);
					}*/
					return 1;
				}
				else return -1;
				break;
			}
			else switch(n->body->type)
			{
				case BD_SPHERE://Sphere to sphere collision
				{//Assume quasy-static behaviour
					Vector pos1 = n->getPos(), pos0 = this->getPos(), pos;
					f32 rad = ((SPHEREBD*)n->body)->radius + ((SPHEREBD*)body)->radius;
					guVecSub(&pos1, &pos0, &pos);
					f32 dst = pos.x*pos.x+pos.y*pos.y+pos.z*pos.z;//guVecDotProduct(&pos, &pos);//Square distance
					if(dst <= (rad*rad)) return 1;
					else return -1;
					break;
				}
				default:
				{
					return n->collide(this);//Evvery new body added implements collisions for previously defined ones
				}
			}
			break;
		}
		case BD_INFPLANE:
		{
			if(!n->body)
			{//Plane to point collision
				Mtx abs;
				absMtx(abs);
				Vector up = vector3(0,0,1);
				guVecMultiply(abs, &up, &up);
				Vector pos0 = getPos(RT_ABS), pos1 = n->getPos(RT_ABS);
				Vector spd0 = getSpeed(RT_ABS), spd1 = n->getSpeed(RT_ABS);
				guVecSub(&spd1, &spd0, &spd0);
				guVecSub(&pos1, &pos0, &pos0);
				f32 spd = guVecDotProduct(&spd0, &up);
				f32 pos = guVecDotProduct(&pos0, &up);
				f32 tmp, dst = spd * gameTime;
				tmp = dst*pos;
				if(dst*pos > 0)//It may collide
				{
					if(dst*dst > pos*dst) return 1;
					else return -1;
				}
				else return -1;
			}
			else switch(n->body->type)
			{
				case BD_SPHERE:
				{
					Vector pos0 = getPos(RT_ABS), pos1 = n->getPos(RT_ABS);
					guVecSub(&pos1, &pos0, &pos0);
					Vector up = AXIS_Z;
					Mtx tmp;
					absMtx(tmp);
					guVecMultiply(tmp, &up, &up);
					f32 dst2, dst = guVecDotProduct(&up, &pos0);
					if(dst < 0) dst2 = -dst;
					else dst2 = dst;
					if(dst2 < n->body->getRadius())
					{
						//Commit to boundaries
						if(dst > 0) dst -= n->body->getRadius();
						else dst += n->body->getRadius();
						guVecScale(&up, &up, -dst);
						n->move(up, RT_ABS);
						//Return effective collision
						return 1;
					}
					else return -1;
					//We assume infinites plane's
					break;
				}
				case BD_INFPLANE:
				{
					Mtx tmp;
					Vector up1 = vector3(0,0,1);
					Vector up2 = vector3(0,0,1);
					absMtx(tmp);
					guVecMultiply(tmp, &up1, &up1);
					n->absMtx(tmp);
					guVecMultiply(tmp, &up2, &up2);
					f32 cosine = guVecDotProduct(&up1, &up2);
					if((cosine <= 0.99f)&&(cosine >= -0.99f))//some tolerance
					return 1;
					else return -1;
					break;
					//No commitment
				}
			}
			break;
		}
	}
	return -1;//No collision detected
}

void NODE::setMass(f32 mass)
{
	if(this->mass)
	{
		f32 factor = mass/this->mass;
		Mtx factorM;
		guMtxScale(factorM, factor, factor, factor);
		guMtxConcat(factorM, inertia, inertia);
	}
	this->mass = mass;
}

//-------------------------------------------------------------------------------------
void NODE::setSpeed(Vector linear, Vector angular)
{
	this->speed = linear;
	this->aSpeed = angular;
}

//-------------------------------------------------------------------------------------
void NODE::render(u32 _lightMask)
{
	//TODO: Add debug code to draw xyz axes
}
//-------------------------------------------------------------------------------------
N2D::N2D(u8 flags)
{
	this->flags = flags;
	type = NT_NODE;
	guMtxIdentity(base);
	this->angle = 0;
	//now Queuing
	child = NULL;
	if(mainRoot->rootN2D)
	{
		next = mainRoot->rootN2D->child;
		mainRoot->rootN2D->child = this;
		parent = mainRoot->rootN2D;
	}
	else
	{
		next = NULL;
		parent = NULL;
	}
	custom = NULL;
}

N2D::~N2D()
{
	releaseChildren();
	if(parent->child == this)
	{
		parent->child = next;
	}
	else
	{
		N2D * aux = parent->child;
		while(aux->next != this)
		{
			aux = aux->next;
		}
		aux->next = next;
	}
}

void N2D::attach(N2D * parent, u8 coord)
{
	deattach(coord);
	mainRoot->rootN2D->child = next;
	next = parent->child;
	parent->child = this;
	this->parent = parent;
	if(coord == RT_ABS)
	{
		Mtx m;
		parent->absMtx(m);
		guMtxInverse(m, m);
		guMtxConcat(m, base, base);
	}
}

void N2D::addChild(N2D * child, u8 coord)
{
	child->attach(this, coord);
}

void N2D::deattach(u8 coord)
{
	if(coord == RT_ABS)
	{
		Mtx m;
		parentMtx(m);
		guMtxConcat(m, base, base);
	}
	if(parent->child == this)
	{
		parent->child = next;
	}
	else
	{
		N2D * aux = parent->child;
		while(aux->next != this)
		{
			aux = aux->next;
		}
		aux->next = next;
	}
	parent = mainRoot->rootN2D;
	next = mainRoot->rootN2D->child;
	mainRoot->rootN2D->child = this;
}

void N2D::releaseChildren(u8 coord)
{
	while(child)
	{
		child->deattach(coord);
	}
}

void N2D::clearChildren()
{
	while(child)
	{
		child->clearChildren();
		delete child;
	}
}

void N2D::move(Vector move, u8 reference)//buggy?
{
	Vector v;
	switch(reference)
	{
		case RT_SELF:
		{
			guVecMultiplySR(base, &move, &v);
			break;
		}
		case RT_PAR:
		{
			v = move;
			break;
		}
		case RT_ABS:
		{
			Mtx m;
			parentMtx(m);
			guVecMultiplySR(m, &move, &v);
			break;
		}
		default:
		{
			v = nullVector;
		}
	}
	guMtxTransApply(base, base, v.x, v.y, v.z);
}

void N2D::rotate(f32 angle)//buggy?
{
	Mtx rot;
	guMtxRotDeg(rot, 'z', angle);
	f32 x0 = base[0][3];
	f32 y0 = base[1][3];
	base[0][3] = base[1][3] = 0.0f;
	guMtxConcat(rot, base, base);
	base[0][3] = x0;
	base[1][3] = y0;
	this->angle += angle;
}

void N2D::setPos(Vector pos, u8 reference)
{
	Mtx aux;
	if(reference == RT_ABS)
	{
		parentMtx(aux);
		guMtxInverse(aux, aux);
		guVecMultiply(aux, &pos, &pos);
	}
	base[0][3] = pos.x;
	base[1][3] = pos.y;
	base[2][3] = pos.z;
}

Vector N2D::getPos(u8 coord)//
{
	Mtx aux;
	Vector v = {base[0][3], base[1][3], base[2][3]};
	if(coord == RT_ABS)
	{
		parentMtx(aux);
		guVecMultiply(aux, &v, &v);
	}
	return v;
}

void N2D::setAngle(f32 angle, u8 reference)
{
	f32 x= base[0][3];
	f32 y= base[1][3];
	f32 layer = base[2][3];
	guMtxRotDeg(base, 'z', angle);
	if(!reference)
	{
		Mtx m;
		parentMtx(m);
		guMtxInverse(m, m);
		guMtxConcat(m, base, base);
	}
	base[0][3] = x;
	base[1][3] = y;
	base[2][3] = layer;
	this->angle = angle;
}

f32 N2D::getAngle(u8 coord)
{
	if(coord != RT_ABS)
		return this->angle;
	N2D * aux = this;
	f32 ang = 0;
	while(aux->parent)
	{
		ang += aux->angle;
		aux = aux->parent;
	}
	return ang;
}

void N2D::parentMtx(Mtx m)
{
	guMtxIdentity(m);
	N2D * aux = parent;
	while(aux)
	{
		guMtxConcat(aux->base, m, m);
		aux = aux->parent;
	}
}

void N2D::absMtx(Mtx m)
{
	parentMtx(m);
	guMtxConcat(m, base, m);
}

void N2D::srMtx(Mtx m)
{
	absMtx(m);
	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
}
