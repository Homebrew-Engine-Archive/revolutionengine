///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Nodes header file

#ifndef REV_NODES_H
#define REV_NODES_H

//Forward declarations
class BODY;

class NODE
{
public:
	//Constructors and destructors
	NODE	(u8 flags = F_Visible);
	~NODE	();
	//Children management functions
	void	attach			(NODE * parent, u8 coord = RT_ABS);
	void	addChild		(NODE * child, u8 coord = RT_ABS);//Keep abs coordinates?
	void	deattach		(u8 coord = RT_ABS);
	void	releaseChildren	(u8 coord = RT_ABS);
	void	clearChildren	();
	NODE	* getChild		()										{	return child;		}
	NODE	* getNext		()										{	return next;		}
	NODE	* getParent		()										{	return parent;		}
	//Displacement functions
	//references: himself, parent, root
	void	move			(Vector move, u8 reference = RT_SELF);
	void	rotate			(Vector axis, f32 angle, u8 coord = RT_ABS);
	void	setPos			(Vector pos, u8 reference = RT_ABS);//Just parent or absolute
	Vector	getPos			(u8 reference = RT_ABS);
	void	parentMtx		(Mtx m);
	void	absMtx			(Mtx m);
	void	lookAt			(Vector look);
	//Physics related
	void	(*react)		(NODE * self, NODE * n);//React to collisions pointer
	BODY	* body;
	f32		collide			(NODE * n);
	f32		getMass			()										{	return mass;		}
	void	setMass			(f32 mass);
	void	setSpeed		(Vector linear, Vector angular = nullVector);//Update to reference coordinates
	Vector	getSpeed		(u8 reference = RT_ABS)					{	return speed;		}
	//Other
	u8		getType			()										{	return type;		}
	virtual bool	isRenderable()									{	return false;		}
	void	* custom;
protected:
	virtual void render	(u32 lightMask);
	//Physics
	f32 mass;
	Mtx inertia;
	Vector speed, aSpeed;
	u8 type;
public:
	u8 flags;
	u8 mode;
private:
	NODE * parent;
	NODE * next;
	NODE * child;
	Mtx base;
	void srMtx			(Mtx m);//Copies scale and rotation absolute matrix into m
friend void render		(NODE * node, Vector camPos);
friend void clasify3D	(NODE * node);
friend void updatePhysicState();
friend class SKYBOX;
friend class VIEWPORT;
friend class R2TEXTURE;
};

class N2D
{
public:
	N2D(u8 flags = F_Visible);
	~N2D();
	void attach			(N2D * parent, u8 coord = RT_ABS);
	void addChild		(N2D * child, u8 coord = RT_ABS);
	void deattach		(u8 coord = RT_ABS);
	void releaseChildren(u8 coord = RT_ABS);
	void clearChildren	();
	N2D * getChild		()										{	return child;	}
	N2D * getNext		()										{	return next;	}
	N2D * getParent		()										{	return parent;	}
	//
	void move			(Vector move, u8 reference = RT_SELF);
	void rotate			(f32 angle);
	void setPos			(Vector v, u8 coord = RT_ABS);
	Vector getPos		(u8 coord = RT_ABS);
	void setAngle		(f32 angle, u8 reference = RT_ABS);
	f32 getAngle		(u8 coord = RT_ABS);
	void parentMtx		(Mtx m);
	void absMtx			(Mtx m);
	//Other
	u8 getType			()										{	return type;	}
private:
	N2D * parent;
	N2D * next;
	N2D * child;
	Mtx base;
	f32 angle;
	void srMtx			(Mtx m);
public:
	void * custom;
	u8 flags;
protected:
	u8 type;
friend void render2D	();
friend void parse2D		(N2D * node);
};

#endif
