///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Maths header file

#ifndef REV_MATHS_H
#define REV_MATHS_H

#include "REVUtils.h"

typedef f32	Mtx23[2][3];//2D matrix
typedef f32 (*Mtx23P)[3];//2D matrix pointer

class QUATERNION
{
	public:
	QUATERNION(f32 x = 0.0f, f32 y = 0.0f, f32 z = 0.0f, f32 w = 0.0f);
	f32 x, y, z, w;
	void wCompute();
	void normalize();
	void rotatePoint(Vector * src, Vector * dst);
	f32 dotProduct(const QUATERNION q);
	QUATERNION operator * (const QUATERNION q);
	QUATERNION operator * (Vector v);
friend QUATERNION interpolate(QUATERNION a, QUATERNION b, f32 t);
};

QUATERNION interpolate(QUATERNION a, QUATERNION b, f32 t);

void mtx23Identity(Mtx23 m);
void mtx23Copy(Mtx src,Mtx dst);
void mtx23Concat(Mtx23 a, Mtx23 b, Mtx23 ab);
u8 mtx23Inverse(Mtx23 src, Mtx23 dst);
void mtx23RotRad(Mtx23 m, f32 angle);
#define mtx23RotDeg(m, a) (mtx23RotRad(m, DegToRad(a)))
void mtx23VecMultiply(f32 x0, f32 y0, Mtx23 m, f32 * x, f32 * y);
void mtx23VecMultiplySR(f32 x0, f32 y0, Mtx23 m, f32 * x, f32 * y);

f32 fastInverseSqrt(f32 a);

#endif
