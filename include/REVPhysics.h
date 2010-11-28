///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Physics
#ifndef REV_PHYSICS_H
#define REV_PHYSICS_H

class TBBox//Bounding box
{
public:
	TBBox()					{	reset();		}
	bool			isValid		()const	{	return valid;	}
	const Vector&	getMax		()const	{	return max;		}
	const Vector&	getMin		()const	{	return min;		}
	const Vector&	getSize		()const;
	const Vector&	getCenter	()const;
	void			reset		();
	void			adjust		(Vector& v);
private:
	Vector		max;
	Vector		min;
	bool		valid;
};

class BODY
{
public:
	virtual f32 getRadius()=0;
	virtual void setInertia(Mtx dst, f32 mass)=0;
protected:
	u8 type;
friend class NODE;
};

class SPHEREBD:public BODY
{
public:
	SPHEREBD(f32 radius);
	f32 getRadius() {return radius;}
	void setInertia(Mtx dst, f32 mass);
	f32 radius;
};

class INFPLANEBD:public BODY
{
public:
	INFPLANEBD();
	f32 getRadius();// {return -1;}//Infinite radius
	void setInertia(Mtx dst, f32 mass);
};

void updatePhysics();

#endif
