#ifndef cuda_math_basicH
#define cuda_math_basicH






__device__ char	dbSign(float value)
{
	return value < 0?-1:(value>0?1:0);
}

__device__ char	dbSign(int value)
{
	return value < 0?-1:(value>0?1:0);
}

__device__ char	dbSign(long long value)
{
	return value < 0?-1:(value>0?1:0);
}

__device__ float dbMin(float a, float b)
{
	return fminf(a,b);
}

__device__ unsigned  dbMin(unsigned  a, unsigned  b)
{
	return min(a,b);
}

__device__ int dbMin(int a, int b)
{
	return min(a,b);
}

__device__ long long dbMin(long long a, long long b)
{
	return a<b?a:b;
}

__device__ unsigned long long dbMin(unsigned long long a, unsigned long long b)
{
	return a<b?a:b;
}

__device__ float	dbPow(float b, float e)
{
	return powf(b,e);
}


__device__ float dbClamped(float x, float a, float b)
{
	if (x < (float)a)
		return (float)a;
	if (x > (float)b)
		return (float)b;
	return x;
}

__device__ float dbClamped(float x, int a, int b)
{
	if (x < (float)a)
		return (float)a;
	if (x > (float)b)
		return (float)b;
	return x;
}

__device__ int dbClamped(int x, int a, int b)
{
	if (x < a)
		return a;
	if (x > b)
		return b;
	return x;
}

__device__ double dbClamped(double x, float a, float b)
{
	if (x < a)
		return a;
	if (x > b)
		return b;
	return x;
}
__device__ double dbClamped(double x, double a, double b)
{
	if (x < a)
		return a;
	if (x > b)
		return b;
	return x;
}

__device__ double dbClamped(double x, int a, int b)
{
	if (x < (double)a)
		return (double)a;
	if (x > (double)b)
		return (double)b;
	return x;

}


// convert floating point rgb color to 8-bit integer
__device__ int dbRgbToInt(float r, float g, float b)
{
    r = dbClamped(r*255.0f, 0.0f, 255.0f);
    g = dbClamped(g*255.0f, 0.0f, 255.0f);
    b = dbClamped(b*255.0f, 0.0f, 255.0f);
    return 0xFF<<24 | (int(r)<<16) | (int(g)<<8) | int(b);
}

__device__ float dbAbs(float v)
{
	return v < 0?-v:v;
}



__device__ float dbSqr(float v)
{
	return v*v;
}

__device__ double dbSqr(double v)
{
	return v*v;
}

__device__ INT64 dbSqr(INT32 v)
{
	return (INT64)v*(INT64)v;
}

__device__ INT64 dbSqr(INT64 v)
{
	return v*v;
}

__device__ UINT64 dbSqr(UINT32 v)
{
	return (UINT64)v*(UINT64)v;
}


__device__ UINT64 dbSqr(UINT64 v)
{
	return v*v;
}



__device__ unsigned char      dbSolveSqrEquation(float a, float b, float c, float rs[2])
{
	float root = b*b - 4*a*c;
	if (root < 0)
		return 0;
	if (fabsf(a) <= 0.0001f)
	{
		if (fabsf(b) <= 0.0001f)
			return 0;
		rs[0] = __fdividef(-c,b);
		return 1;
	}
	root = sqrtf(root);
	rs[0] = __fdividef(-b-root,2*a);
	rs[1] = __fdividef(-b+root,2*a);
	return rs[0] != rs[1]?2:1;
}


__device__ float           dbSmallestPositiveResult(const float rs[2],unsigned char num_res)
{
	if (num_res == 1)
		return rs[0];
	if (rs[0] < rs[1] && rs[0] >= 0)
		return rs[0];
	if (rs[1] < rs[0] && rs[1] >= 0)
		return rs[1];
	if (rs[0] >= 0)
		return rs[0];
	return rs[1];
}


__device__ 	float     dbCosFactor(float f)
{
	return (__cosf((1-f)*3.14159265)+1)/2;
}

__device__	float	dbCubicFactor(float x)
{
	return -2*x*x*x + 3*x*x;
}


__device__ float      dbLinearStep(float v, float lower, float upper)
{
	if (upper<=lower)
		return 0.5;
	return dbClamped(__fdividef(v-lower,upper-lower),0,1);
}

__device__	float	dbCosStep(float value, float lower, float upper)
{
	return dbCosFactor(dbLinearStep(value,lower,upper));
}

__device__	float	dbCubicStep(float value, float lower, float upper)
{
	return dbCubicFactor(dbLinearStep(value,lower,upper));
}



#endif
