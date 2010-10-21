///////////////////////////////////
//Technik 2009
//Revolution Engine 0.5
///////////////////////////////////
//Maths source file
#include "REV.h"

void mtx23Identity(Mtx23 m)
{
	u8 i,j;
	for(i = 0; i < 2; i++)
		for(j = 0; j < 3; j++)
			if(i==j) m[i][j] = 1.0f;
			else m[i][j] = 0.0f;
}

void mtx23Copy(Mtx23 src,Mtx23 dst)
{
	if(src == dst) return;
	u8 j;
	for(u8 i = 0; i < 2; i++)
	for(j = 0; j < 3; j++)
	dst[i][j] = src[i][j];
}

void mtx23Concat(Mtx23 a, Mtx23 b, Mtx23 ab)
{
	Mtx23 tmp;
	Mtx23P m;
	
	if(ab==b || ab==a)
		m = tmp;
	else
		m = ab;

    m[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0];
	m[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1];
	m[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2];
	
	m[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0];
	m[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1];
	m[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2];

	if(m==tmp)
		mtx23Copy(tmp,ab);
}

u8 mtx23Inverse(Mtx23 src, Mtx23 dst)
{
	Mtx23 mTmp;
    Mtx23P m;
    f32 det;

    if(src==dst)
        m = mTmp;
	else 
        m = dst;

	// compute the determinant of the upper 3x3 submatrix
    det =   src[0][0]*src[1][1] - src[0][1]*src[1][0];


    // check if matrix is singular
    if(det==0.0f)return 0;


    // compute the inverse of the upper submatrix:

    // find the transposed matrix of cofactors of the upper submatrix
    // and multiply by (1/det)

    det = 1.0f / det;


    m[0][0] = src[1][1] * det;
    m[0][1] = src[0][1] * det;
    m[0][2] = (src[0][1]*src[1][2] - src[1][1]*src[0][2]) * det;

    m[1][0] = src[1][0] * det;
    m[1][1] = src[0][0] * det;
    m[1][2] = (src[0][0]*src[1][2] - src[1][0]*src[0][2]) * det;

    // copy back if needed
    if( m == mTmp )
        mtx23Copy(mTmp,dst);

    return 1;
}

void mtx23RotRad(Mtx23 m, f32 angle)
{
	m[0][2] = m[1][2] = 0.0f;
	m[0][0] = m[1][1] = cosf(angle);
	m[1][0] = sinf(angle);
	m[0][1] = -m[1][0];
}

void mtx23VecMultiply(f32 x0, f32 y0, Mtx23 m, f32 * x, f32 * y)
{
	*x = x0 * m[0][0] + y0 * m[0][1] + m[0][2];
	*y = x0 * m[0][0] + y0 * m[0][1] + m[0][2];
}

void mtx23VecMultiplySR(f32 x0, f32 y0, Mtx23 m, f32 * x, f32 * y)
{
	*x = x0 * m[0][0] + y0 * m[0][1];
	*y = x0 * m[0][0] + y0 * m[0][1];
}

QUATERNION::QUATERNION(f32 x, f32 y, f32 z, f32 w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void QUATERNION::wCompute()
{
	f32 t = 1.0f -(x*x)-(y*y)-(z*z);

	if ( t < 0.0f )
		w = 0.0f;
	else
		w = -sqrt (t);
}

void QUATERNION::normalize()
{
	f32 mag = sqrt ((x*x) + (y*y) + (z*z) + (w*w));
	
	if ( mag != 0.0f )
	{
		f32 overMag = 1.0f / mag;

		x *= overMag;
		y *= overMag;
		z *= overMag;
		w *= overMag;
	}
}

QUATERNION QUATERNION::operator * (QUATERNION q)
{
	QUATERNION temp;
	temp.w = (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z);
	temp.x = (x * q.w) + (w * q.x) + (y * q.z) - (z * q.y);
	temp.y = (y * q.w) + (w * q.y) + (z * q.x) - (x * q.z);
	temp.z = (z * q.w) + (w * q.z) + (x * q.y) - (y * q.x);
	return temp;
}

QUATERNION QUATERNION::operator * (Vector v)
{
	QUATERNION temp;
	temp.w = - (x * v.x) - (y * v.y) - (z * v.z);
	temp.x =   (w * v.x) + (y * v.z) - (z * v.y);
	temp.y =   (w * v.y) + (z * v.x) - (x * v.z);
	temp.z =   (w * v.z) + (x * v.y) - (y * v.x);
	return temp;
}

void QUATERNION::rotatePoint(Vector * src, Vector * dst)
{
	QUATERNION tmp, inv, final;

	inv.x = -x;
	inv.y = -y;
	inv.z = -z;
	inv.w =  w;

	inv.normalize();

	// R = Q.P.Q*

	tmp = this->operator*(*src);
	final = tmp * inv;

	dst->x = final.x;
	dst->y = final.y;
	dst->z = final.z;
}

f32 QUATERNION::dotProduct(const QUATERNION q)
{
	return x*q.x + y*q.y + z*q.z + w*q.w;
}

QUATERNION interpolate(QUATERNION a, QUATERNION b, f32 t)
{
	if ( t <= 0.0 )
	{
		return a;
	}

	if ( t >= 1.0 )
	{
		return b;
	}
	
	QUATERNION q;
	
	f32 cosOmega = a.dotProduct(b);

	f32 q1w = b.w;
	f32 q1x = b.x;
	f32 q1y = b.y;
	f32 q1z = b.z;

	if ( cosOmega < 0.0f )
	{
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}
	
	f32 k0, k1;

	if ( cosOmega > 0.99f )
	{
		k0 = 1.0f - t;
		k1 = t;
	}
	else
	{
		f32 sinOmega = sqrt (1.0f - ( cosOmega * cosOmega ) );
		f32 omega = atan2 ( sinOmega, cosOmega );
		f32 oneOverSinOmega = 1.0f / sinOmega;
		k0 = sin ((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sin (t * omega) * oneOverSinOmega;
	}

	q.w = (k0 * a.w) + (k1 * q1w);
	q.x = (k0 * a.x) + (k1 * q1x);
	q.y = (k0 * a.y) + (k1 * q1y);
	q.z = (k0 * a.z) + (k1 * q1z);
	return q;
}

f32 fastInverseSqrt(f32 a)
{
	union{
	 f32 f;
	 s32 i;
	 }tmp;
	 tmp.f = a;
	 tmp.i = 0x5f3759df - (tmp.i >> 1);//FISR magic number
	 f32 y = tmp.f;
	 return y*(1.5f - 0.5f*a*y*y);
}

void REVVectorNormalize(Vector * v)
{
	f32 invMod = fastInverseSqrt(v->x*v->x+v->y*v->y+v->z*v->z);
	v->x*=invMod;
	v->y*=invMod;
	v->z*=invMod;
}
