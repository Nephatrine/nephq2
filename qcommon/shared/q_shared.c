/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2000-2002 Mr. Hyde and Mad Dog

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "q_shared.h"

vec3_t vec3_origin = {0,0,0};

//============================================================================

#ifdef _WIN32
#include <ctype.h>
#pragma optimize( "", off )
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

#ifdef __APPLE__
#include <ctype.h>
#endif

void RotatePointAroundVector( vec3_t dst, const vec3_t dir, const vec3_t point, float degrees )
{
	float	m[3][3];
	float	im[3][3];
	float	zrot[3][3];
	float	tmpmat[3][3];
	float	rot[3][3];
	int32_t	i;
	vec3_t vr, vup, vf;

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector( vr, dir );
	CrossProduct( vr, vf, vup );

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy( im, m, sizeof( im ) );

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset( zrot, 0, sizeof( zrot ) );
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	zrot[0][0] = cos( DEG2RAD( degrees ) );
	zrot[0][1] = sin( DEG2RAD( degrees ) );
	zrot[1][0] = -sin( DEG2RAD( degrees ) );
	zrot[1][1] = cos( DEG2RAD( degrees ) );

	R_ConcatRotations( m, zrot, tmpmat );
	R_ConcatRotations( tmpmat, im, rot );

	for ( i = 0; i < 3; i++ )
	{
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
	}
}

#ifdef _WIN32
#pragma optimize( "", on )
#endif



void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
	float		angle;
	static float		sr, sp, sy, cr, cp, cy;
	// static to help MS compiler fp bugs

	if (!angles)
		return;

	angle = angles[YAW] * (M_PI*2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI*2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI*2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1*sr*sp*cy+-1*cr*-sy);
		right[1] = (-1*sr*sp*sy+-1*cr*cy);
		right[2] = -1*sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy+-sr*-sy);
		up[1] = (cr*sp*sy+-sr*cy);
		up[2] = cr*cp;
	}
}

void AngleVectors2(vec3_t value1, vec3_t angles)
{
	float forward;
	float yaw, pitch;

	if ((value1[1] == 0) && (value1[0] == 0))
	{
		yaw = 0;

		if (value1[2] > 0)
		{
			pitch = 90;
		}

		else
		{
			pitch = 270;
		}
	}
	else
	{
		if (value1[0])
		{
			yaw = ((float)atan2(value1[1], value1[0]) * 180 / M_PI);
		}

		else if (value1[1] > 0)
		{
			yaw = 90;
		}

		else
		{
			yaw = 270;
		}

		if (yaw < 0)
		{
			yaw += 360;
		}

		forward = (float)sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = ((float)atan2(value1[2], forward) * 180 / M_PI);

		if (pitch < 0)
		{
			pitch += 360;
		}
	}

	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

void MakeNormalVectors (vec3_t forward, vec3_t right, vec3_t up)
{
	float		d;

	// this rotate and negat guarantees a vector
	// not colinear with the original
	right[1] = -forward[0];
	right[2] = forward[1];
	right[0] = forward[2];

	d = DotProduct (right, forward);
	VectorMA (right, -d, forward, right);
	VectorNormalize (right);
	CrossProduct (right, forward, up);
}

void VecToAngleRolled (vec3_t value1, float angleyaw, vec3_t angles)
{
	float	forward, yaw, pitch;

	yaw = (int32_t) (atan2(value1[1], value1[0]) * 180 / M_PI);
	forward = sqrt (value1[0]*value1[0] + value1[1]*value1[1]);
	pitch = (int32_t) (atan2(value1[2], forward) * 180 / M_PI);

	if (pitch < 0)
		pitch += 360;

	angles[PITCH] = -pitch;
	angles[YAW] =  yaw;
	angles[ROLL] = - angleyaw;
}

void ProjectPointOnPlane( vec3_t dst, const vec3_t p, const vec3_t normal )
{
	float d;
	vec3_t n;
	float inv_denom;

	inv_denom = 1.0F / DotProduct( normal, normal );

	d = DotProduct( normal, p ) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}

/*
** assumes "src" is normalized
*/
void PerpendicularVector( vec3_t dst, const vec3_t src )
{
	int32_t	pos;
	int32_t i;
	float minelem = 1.0F;
	vec3_t tempvec;

	/*
	** find the smallest magnitude axially aligned vector
	*/
	for ( pos = 0, i = 0; i < 3; i++ )
	{
		if ( fabs( src[i] ) < minelem )
		{
			pos = i;
			minelem = fabs( src[i] );
		}
	}
	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	/*
	** project the point onto the plane defined by src
	*/
	ProjectPointOnPlane( dst, tempvec, src );

	/*
	** normalize the result
	*/
	VectorNormalize( dst );
}

/*
================
RotationMatrix
================
*/
void RotationMatrix(vec_t angle, vec_t x, vec_t y, vec_t z, vec_t out[4][4])
{
	vec_t phi = DEG2RAD(angle);
    vec_t c = cosf(phi);    // cosine
    vec_t s = sinf(phi);    // sine
    vec_t xx = x * x;
    vec_t xy = x * y;
    vec_t xz = x * z;
    vec_t yy = y * y;
    vec_t yz = y * z;
    vec_t zz = z * z;

    // build rotation matrix
    memset(out,0,sizeof(vec_t) * 16);
    out[0][0] = xx * (1 - c) + c;
    out[1][0] = xy * (1 - c) - z * s;
    out[2][0] = xz * (1 - c) + y * s;
    out[0][1] = xy * (1 - c) + z * s;
    out[1][1] = yy * (1 - c) + c;
    out[2][1] = yz * (1 - c) - x * s;
    out[0][2] = xz * (1 - c) - y * s;
    out[1][2] = yz * (1 - c) + x * s;
    out[2][2] = zz * (1 - c) + c;
	out[3][3] = 1;
}

/*
================
RotationMatrix
================
*/
void RotationMatrix3(vec_t angle, vec_t x, vec_t y, vec_t z, vec_t out[3][3])
{
	vec_t phi = DEG2RAD(angle);
    vec_t c = cosf(phi);    // cosine
    vec_t s = sinf(phi);    // sine
    vec_t xx = x * x;
    vec_t xy = x * y;
    vec_t xz = x * z;
    vec_t yy = y * y;
    vec_t yz = y * z;
    vec_t zz = z * z;

    // build rotation matrix
    out[0][0] = xx * (1 - c) + c;
    out[1][0] = xy * (1 - c) - z * s;
    out[2][0] = xz * (1 - c) + y * s;
    out[0][1] = xy * (1 - c) + z * s;
    out[1][1] = yy * (1 - c) + c;
    out[2][1] = yz * (1 - c) - x * s;
    out[0][2] = xz * (1 - c) - y * s;
    out[1][2] = yz * (1 - c) + x * s;
    out[2][2] = zz * (1 - c) + c;
}


/*
================
TranslationMatrix
================
*/
void TranslationMatrix(vec_t x, vec_t y, vec_t z, vec_t out[4][4])
{
    // build rotation matrix
    memset(out,0,sizeof(vec_t) * 16);
	out[0][0] = out[1][1] = out[2][2] = 1;
    out[3][0] = x;
    out[3][1] = y;
    out[3][2] = z;
    out[3][3] = 1;
}

/*
================
MatrixMultiply
================
*/
void MatrixMultiply (vec_t in1[4][4], vec_t in2[4][4], vec_t out[4][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0] + in1[0][3] * in2[3][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1] + in1[0][3] * in2[3][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2] + in1[0][3] * in2[3][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3] * in2[3][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0] + in1[1][3] * in2[3][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1] + in1[1][3] * in2[3][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2] + in1[1][3] * in2[3][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3] * in2[3][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0] + in1[2][3] * in2[3][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1] + in1[2][3] * in2[3][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2] + in1[2][3] * in2[3][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3] * in2[3][3];
	out[3][0] = in1[3][0] * in2[0][0] + in1[3][1] * in2[1][0] + in1[3][2] * in2[2][0] + in1[3][3] * in2[3][0];
	out[3][1] = in1[3][0] * in2[0][1] + in1[3][1] * in2[1][1] + in1[3][2] * in2[2][1] + in1[3][3] * in2[3][1];
	out[3][2] = in1[3][0] * in2[0][2] + in1[3][1] * in2[1][2] + in1[3][2] * in2[2][2] + in1[3][3] * in2[3][2];
	out[3][3] = in1[3][0] * in2[0][3] + in1[3][1] * in2[1][3] + in1[3][2] * in2[2][3] + in1[3][3] * in2[3][3];
}


/*
================
R_ConcatRotations
================
*/
void R_ConcatRotations (float in1[3][3], float in2[3][3], float out[3][3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
}

/*
================
R_ConcatTransforms
================
*/
void R_ConcatTransforms (float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}


//============================================================================


float Q_fabs (float f)
{
#if 0
	if (f >= 0)
		return f;
	return -f;
#else
	int32_t tmp = * ( int32_t * ) &f;
	tmp &= 0x7FFFFFFF;
	return * ( float * ) &tmp;
#endif
}

/*
===============
LerpAngle

===============
*/
float LerpAngle (float a2, float a1, float frac)
{
	if (a1 - a2 > 180)
		a1 -= 360;
	if (a1 - a2 < -180)
		a1 += 360;
	return a2 + frac * (a1 - a2);
}


float	anglemod(float a)
{
#if 0
	if (a >= 0)
		a -= 360*(int32_t)(a/360);
	else
		a += 360*( 1 + (int32_t)(-a/360) );
#endif
	a = (360.0/65536) * ((int32_t)(a*(65536/360.0)) & 65535);
	return a;
}

	int32_t		i;
	vec3_t	corners[2];


// this is the slow, general version
int32_t BoxOnPlaneSide2 (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	int32_t		i;
	float	dist1, dist2;
	int32_t		sides;
	vec3_t	corners[2];

	for (i=0 ; i<3 ; i++)
	{
		if (p->normal[i] < 0)
		{
			corners[0][i] = emins[i];
			corners[1][i] = emaxs[i];
		}
		else
		{
			corners[1][i] = emins[i];
			corners[0][i] = emaxs[i];
		}
	}
	dist1 = DotProduct (p->normal, corners[0]) - p->dist;
	dist2 = DotProduct (p->normal, corners[1]) - p->dist;
	sides = 0;
	if (dist1 >= 0)
		sides = 1;
	if (dist2 < 0)
		sides |= 2;

	return sides;
}

/*
==================
BoxOnPlaneSide

Returns 1, 2, or 1 + 2
==================
*/
int32_t BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, struct cplane_s *p)
{
	float	dist1, dist2;
	int32_t		sides;

// fast axial cases
	if (p->type < 3)
	{
		if (p->dist <= emins[p->type])
			return 1;
		if (p->dist >= emaxs[p->type])
			return 2;
		return 3;
	}
	
// general case
	switch (p->signbits)
	{
	case 0:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 1:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
		break;
	case 2:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 3:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
		break;
	case 4:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 5:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
		break;
	case 6:
dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	case 7:
dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
		break;
	default:
		dist1 = dist2 = 0;		// shut up compiler
		assert( 0 );
		break;
	}

	sides = 0;
	if (dist1 >= p->dist)
		sides = 1;
	if (dist2 < p->dist)
		sides |= 2;

	assert( sides != 0 );

	return sides;
}

void ClearBounds (vec3_t mins, vec3_t maxs)
{
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}

void AddPointToBounds (vec3_t v, vec3_t mins, vec3_t maxs)
{
	int32_t		i;
	vec_t	val;

	for (i=0 ; i<3 ; i++)
	{
		val = v[i];
		if (val < mins[i])
			mins[i] = val;
		if (val > maxs[i])
			maxs[i] = val;
	}
}


int32_t VectorCompare (vec3_t v1, vec3_t v2)
{
	if (v1[0] != v2[0] || v1[1] != v2[1] || v1[2] != v2[2])
			return 0;
			
	return 1;
}


vec_t VectorNormalize (vec3_t v)
{
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt (length);		// FIXME

	if (length)
	{
        int32_t i;
		ilength = 1/length;
        for (i=0 ; i< 3 ; i++)
            v[i] *= ilength;
	}
		
	return length;
}

vec_t VectorNormalize2 (vec3_t v, vec3_t out)
{
	float	length, ilength;

	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	length = sqrt (length);		// FIXME

	if (length)
	{
        int32_t i;
		ilength = 1/length;
        for (i=0 ; i< 3 ; i++)
            out[i] = v[i]*ilength;
	}
		
	return length;
}

void VectorMA (vec3_t veca, float scale, vec3_t vecb, vec3_t vecc)
{
    int32_t i;
    for (i=0 ; i< 3 ; i++)
        vecc[i] = veca[i] + scale*vecb[i];
}


vec_t _DotProduct (vec3_t v1, vec3_t v2)
{
	return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void _VectorSubtract (vec3_t veca, vec3_t vecb, vec3_t out)
{
    int32_t i;
    for (i=0 ; i< 3 ; i++)
        out[i] = veca[i]-vecb[i];
}

void _VectorAdd (vec3_t veca, vec3_t vecb, vec3_t out)
{
    int32_t i;
    for (i=0 ; i< 3 ; i++)
        out[i] = veca[i]+vecb[i];
}

void _VectorCopy (vec3_t in, vec3_t out)
{
    memcpy(out, in, sizeof(vec3_t));
}

void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
{
	cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
	cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
	cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

double sqrt(double x);

vec_t VectorLength(vec3_t v)
{
	int32_t		i;
	float	length;
	
	length = 0;
	for (i=0 ; i< 3 ; i++)
		length += v[i]*v[i];
	length = sqrt (length);		// FIXME

	return length;
}

void VectorInverse (vec3_t v)
{
    int32_t i;
    for (i=0 ; i< 3 ; i++)
        v[i] = -v[i];
}

void VectorScale (vec3_t in, vec_t scale, vec3_t out)
{
	out[0] = in[0]*scale;
	out[1] = in[1]*scale;
	out[2] = in[2]*scale;
}

int32_t Q_log2(int32_t val)
{
	int32_t answer=0;
	while (val>>=1)
		answer++;
	return answer;
}

/*
=================
VectorRotate
From Q2E
=================
*/
void VectorRotate (const vec3_t v, const vec3_t matrix[3], vec3_t out)
{
	out[0] = v[0]*matrix[0][0] + v[1]*matrix[0][1] + v[2]*matrix[0][2];
	out[1] = v[0]*matrix[1][0] + v[1]*matrix[1][1] + v[2]*matrix[1][2];
	out[2] = v[0]*matrix[2][0] + v[1]*matrix[2][1] + v[2]*matrix[2][2];
}

/*
=================
AnglesToAxis
From Q2E
=================
*/
void AnglesToAxis (const vec3_t angles, vec3_t axis[3])
{
	static float	sp, sy, sr, cp, cy, cr;
	float			angle;

	angle = DEG2RAD(angles[PITCH]);
	sp = sin(angle);
	cp = cos(angle);
	angle = DEG2RAD(angles[YAW]);
	sy = sin(angle);
	cy = cos(angle);
	angle = DEG2RAD(angles[ROLL]);
	sr = sin(angle);
	cr = cos(angle);

	axis[0][0] = cp*cy;
	axis[0][1] = cp*sy;
	axis[0][2] = -sp;
	axis[1][0] = sr*sp*cy+cr*-sy;
	axis[1][1] = sr*sp*sy+cr*cy;
	axis[1][2] = sr*cp;
	axis[2][0] = cr*sp*cy+-sr*-sy;
	axis[2][1] = cr*sp*sy+-sr*cy;
	axis[2][2] = cr*cp;
}

/*
=================
AxisClear
From Q2E
=================
*/
void AxisClear (vec3_t axis[3])
{
    memset(axis,0, sizeof(vec3_t) * 3);
	axis[0][0] = 1;
	axis[1][1] = 1;
	axis[2][2] = 1;
}

/*
=================
AxisCopy
From Q2E
=================
*/
void AxisCopy (const vec3_t in[3], vec3_t out[3])
{
    memcpy(out, in, sizeof(vec3_t) * 3);
}

/*
=================
AxisCompare
From Q2E
=================
*/
qboolean AxisCompare (const vec3_t axis1[3], const vec3_t axis2[3])
{
	if (axis1[0][0] != axis2[0][0] || axis1[0][1] != axis2[0][1] || axis1[0][2] != axis2[0][2])
		return false;
	if (axis1[1][0] != axis2[1][0] || axis1[1][1] != axis2[1][1] || axis1[1][2] != axis2[1][2])
		return false;
	if (axis1[2][0] != axis2[2][0] || axis1[2][1] != axis2[2][1] || axis1[2][2] != axis2[2][2])
		return false;

	return true;
}


//====================================================================================

void AngleClamp(vec_t *angle)
{
	vec_t temp1;

	temp1 = floor((*angle + 180.0f) * (1.0f / 360.0f)) * 360.0f;
	
	temp1 = *angle - temp1;

	if (temp1 > 180.0f)
		temp1 -= 360.0f;

	*angle = temp1;
}

void VectorClamp(vec3_t angles)
{
	vec3_t temp1;
	
	temp1[0] = floor((angles[0] + 180.0f) * (1.0f / 360.0f)) * 360.0f;
	temp1[1] = floor((angles[1] + 180.0f) * (1.0f / 360.0f)) * 360.0f;
	temp1[2] = floor((angles[2] + 180.0f) * (1.0f / 360.0f)) * 360.0f;

	VectorSubtract(angles, temp1, temp1);

	if (temp1[0] > 180.0f)
		temp1[0] -= 360.0f;

	if (temp1[1] > 180.0f)
		temp1[1] -= 360.0f;

	if (temp1[2] > 180.0f)
		temp1[2] -= 360.0f;

	VectorCopy(temp1,angles);
}

void QuatNormalize(vec4_t quat)
{
	float mag = QuatMagnitude(quat);
	if (fabs(mag) > 0.00001f && fabs(mag - 1.0f) > 0.00001f) {
        float overmag = 1.0 / mag;
		quat[0] *= overmag;
		quat[1] *= overmag;
		quat[2] *= overmag;
		quat[3] *= overmag;
	}
}

void QuatMultiply(vec4_t q1, vec4_t q2, vec4_t out)
{
	vec_t temp;
	vec3_t temp1, temp2;
	vec4_t o;
	temp = _DotProduct(&q1[1],&q2[1]);

	o[0] = q1[0] * q2[0] - temp;
	VectorScale(&q2[1], q1[0], temp1);
	VectorScale(&q1[1], q2[0], temp2);

	CrossProduct(&q1[1], &q2[1], &o[1]);
    VectorAdd(temp1, temp2, temp1);
	_VectorAdd(&o[1], temp1, &o[1]);

    QuatCopy(o,out);

}

void QuatInverse(vec4_t q1, vec4_t out)
{
	vec_t mag = 1.0 / QuatMagnitude(q1);
	out[0] = -q1[0] * mag;
	out[1] = q1[1] * mag;
	out[2] = q1[2] * mag;
	out[3] = q1[3] * mag;
}

void QuatDifference(vec4_t q1, vec4_t q2, vec4_t out)
{
	vec4_t inv;
	QuatInverse(q1, inv);
	QuatMultiply(q2, inv, out);
}


void LerpQuat(vec4_t q1, vec4_t q2, vec_t fraction, vec4_t out)
{
	float a = 1 - fraction;
    int i;
    for (i = 0 ; i < 4 ; i++)
        out[i] = a * q1[i] + fraction * q2[i];
	QuatNormalize(out);
}


void SlerpQuat(vec4_t q1, vec4_t q2, vec_t fraction, vec4_t out)
{
	vec4_t temp;
	float cosOmega = q1[0] * q2[0] + q1[1] * q2[1] + q1[2] * q2[2] + q1[3] * q2[3];
	float k0, k1;
    int i;
	QuatCopy(q2, temp);
	if (cosOmega < 0.0f)
	{
		QuatNegate(temp, temp);
		cosOmega = -cosOmega;
	}

	if (cosOmega > 0.9999f)
	{
		k0 = 1.0f - fraction;
		k1 = fraction;
	}
	else {
		float sinOmega = sqrt(1.0f - cosOmega * cosOmega);
		float omega = atan2(sinOmega, cosOmega);
		float oneOverSinOmega = 1.0f / sinOmega;
		k0 = sin((1.0f - fraction) * omega) * oneOverSinOmega;
		k1 = sin(fraction * omega) * oneOverSinOmega;
	}
    for (i = 0 ; i < 4 ; i++)
        out[i] = q1[i] * k0 + temp[i] * k1;
}

void QuatToEuler(vec4_t q, vec3_t e)
{
	
	float sqw = q[0]*q[0];
	float sqx = q[1]*q[1];
	float sqy = q[2]*q[2];
	float sqz = q[3]*q[3];

	float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	float test = q[1]*q[2] + q[3]*q[0];

	if (test > 0.4999f*unit) {
		// singularity at north pole
		e[YAW] = 2 * atan2(q[1], q[0]);
		e[ROLL] = M_PI / 2;
		e[PITCH] = 0;
	} else if (test < -0.4999f*unit) {
		// singularity at south pole
		e[YAW] = -2 * atan2(q[1], q[0]);
		e[ROLL] = -M_PI / 2;
		e[PITCH] = 0;
	} else {
		e[YAW] = atan2(2 * q[2] * q[0] - 2 * q[1] * q[3], sqx - sqy - sqz + sqw);
		e[ROLL] = asin(2 * test / unit);
		e[PITCH] = atan2(2 * q[1] * q[0] - 2 * q[2] * q[3], -sqx + sqy - sqz + sqw);
	}
	/*
	vec4_t temp;
	QuatCopy(q,temp);
	QuatNormalize(q,q);
	e[ROLL] = atan2(2.0f * (temp[1] * temp[2] + temp[0] * temp[3]), temp[0] * temp[0] + temp[1] * temp[1] - temp[2] * temp[2] - temp[3] * temp[3]);
	e[PITCH] = atan2(2.0f * (temp[2] * temp[3] + temp[0] * temp[1]), temp[0] * temp[0] - temp[1] * temp[1] - temp[2] * temp[2] + temp[3] * temp[3]);
	e[YAW] = asin(-2.0f * (temp[1] * temp[3] - temp[0] * temp[2]));
	*/

	e[PITCH] = RAD2DEG(e[PITCH]);
	e[YAW] = RAD2DEG(e[YAW]);
	e[ROLL] = RAD2DEG(e[ROLL]);

}

void EulerToQuat(vec3_t in, vec4_t out)
{
	vec3_t s, c, t;

	VectorScale(in, 0.5 * M_PI / 180.0f, t);
	VectorSet(s, sin(t[0]), sin(t[1]), sin(t[2]));
	VectorSet(c, cos(t[0]), cos(t[1]), cos(t[2]));

	/*
	out[0] = 2 * acos(cos(temp[ROLL] / 2)*cos(temp[PITCH] / 2)*cos(temp[YAW] / 2) + sin(temp[ROLL] / 2)*sin(temp[PITCH] / 2)*sin(temp[YAW] / 2));
	out[1] = acos((sin(temp[ROLL] / 2)*cos(temp[PITCH] / 2)*cos(temp[YAW] / 2) - cos(temp[ROLL] / 2)*sin(temp[PITCH] / 2)*sin(temp[YAW] / 2)) / sin(out[0] / 2));
	out[2] = acos((cos(temp[ROLL] / 2)*sin(temp[PITCH] / 2)*cos(temp[YAW] / 2) + sin(temp[ROLL] / 2)*cos(temp[PITCH] / 2)*sin(temp[YAW] / 2)) / sin(out[0] / 2));
	out[3] = acos((cos(temp[ROLL] / 2)*cos(temp[PITCH] / 2)*sin(temp[YAW] / 2) - sin(temp[ROLL] / 2)*sin(temp[PITCH] / 2)*cos(temp[YAW] / 2)) / sin(out[0] / 2));
	*/


	out[0] = c[YAW] * c[PITCH] * c[ROLL] + s[YAW] * s[PITCH] * s[ROLL];
	out[1] = c[YAW] * s[PITCH] * c[ROLL] + s[YAW] * c[PITCH] * s[ROLL];
	out[2] = s[YAW] * c[PITCH] * c[ROLL] - c[YAW] * s[PITCH] * s[ROLL];
	out[3] = c[YAW] * c[PITCH] * s[ROLL] - s[YAW] * s[PITCH] * c[ROLL];
	QuatNormalize(out);
}

void QuatToRotation(vec4_t q, vec_t out[4][4])
{
	vec_t xx = q[1] * q[1];
	vec_t xy = q[1] * q[2];
	vec_t xz = q[1] * q[3];
	vec_t xw = q[1] * q[0];

	vec_t yy = q[2] * q[2];
	vec_t yz = q[2] * q[3];
	vec_t yw = q[2] * q[0];

	vec_t zz = q[3] * q[3];
	vec_t zw = q[3] * q[0];

    memset(out, 0, sizeof(vec_t)* 16);
	out[0][0] = 1 - 2 * (yy + zz);
	out[1][0] = 2 * (xy - zw);
	out[2][0] = 2 * (xz + yw);

	out[0][1] = 2 * (xy + zw);
	out[1][1] = 1 - 2 * (xx + zz);
	out[2][1] = 2 * (yz - xw);

	out[0][2] = 2 * (xz - yw);
	out[1][2] = 2 * (yz + xw);
	out[2][2] = 1 - 2 * (xx + yy);

	out[3][3] = 1;


}

//====================================================================================

/*
============
COM_SkipPath
============
*/
char *COM_SkipPath (char *pathname)
{
	char	*last;
	
	last = pathname;
	while (*pathname)
	{
		if (*pathname=='/')
			last = pathname+1;
		pathname++;
	}
	return last;
}

/*
============
COM_StripExtension
============
*/
void COM_StripExtension (char *in, char *out)
{
	while (*in && *in != '.')
		*out++ = *in++;
	*out = 0;
}

/*
============
COM_FileExtension
============
*/
char *COM_FileExtension (char *in)
{
	static char exten[8];
	int32_t		i;

	while (*in && *in != '.')
		in++;
	if (!*in)
		return "";
	in++;
	for (i=0 ; i<7 && *in ; i++,in++)
		exten[i] = *in;
	exten[i] = 0;
	return exten;
}

/*
============
COM_FileBase
============
*/
void COM_FileBase (char *in, char *out)
{
	char *s, *s2;
	
	s = in + strlen(in) - 1;
	
	while (s != in && *s != '.')
		s--;
	
	for (s2 = s ; s2 != in && *s2 != '/' ; s2--)
	;
	
	if (s-s2 < 2)
		out[0] = 0;
	else
	{
		s--;
		strncpy (out,s2+1, s-s2);
		out[s-s2] = 0;
	}
}

/*
============
COM_FilePath

Returns the path up to, but not including the last /
============
*/
void COM_FilePath (char *in, char *out)
{
	char *s;
	
	s = in + strlen(in) - 1;
	
	while (s != in && *s != '/')
		s--;

	strncpy (out,in, s-in);
	out[s-in] = 0;
}


/*
==================
COM_DefaultExtension
==================
*/
void COM_DefaultExtension (char *path, char *extension)
{
	char    *src;
//
// if path doesn't have a .EXT, append extension
// (extension should include the .)
//
	src = path + strlen(path) - 1;

	while (*src != '/' && src != path)
	{
		if (*src == '.')
			return;                 // it has an extension
		src--;
	}

	strcat (path, extension);
}

/*
============================================================================

					BYTE ORDER FUNCTIONS

============================================================================
*/

qboolean	bigendien;

// can't just use function pointers, or dll linkage can
// mess up when qcommon is included in multiple places
// Knightmare- made these static
static int16_t	(*_BigShort) (int16_t l);
static int16_t	(*_LittleShort) (int16_t l);
static int32_t		(*_BigLong) (int32_t l);
static int32_t		(*_LittleLong) (int32_t l);
static float	(*_BigFloat) (float l);
static float	(*_LittleFloat) (float l);

int16_t	BigShort(int16_t l){return _BigShort(l);}
int16_t	LittleShort(int16_t l) {return _LittleShort(l);}
int32_t		BigLong (int32_t l) {return _BigLong(l);}
int32_t		LittleLong (int32_t l) {return _LittleLong(l);}
float	BigFloat (float l) {return _BigFloat(l);}
float	LittleFloat (float l) {return _LittleFloat(l);}

int16_t   ShortSwap (int16_t l)
{
	byte    b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

int16_t	ShortNoSwap (int16_t l)
{
	return l;
}

int32_t    LongSwap (int32_t l)
{
	byte    b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int32_t)b1<<24) + ((int32_t)b2<<16) + ((int32_t)b3<<8) + b4;
}

int32_t	LongNoSwap (int32_t l)
{
	return l;
}

float FloatSwap (float f)
{
	union
	{
		float	f;
		byte	b[4];
	} dat1, dat2;
	
	
	dat1.f = f;
	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];
	return dat2.f;
}

float FloatNoSwap (float f)
{
	return f;
}

/*
================
Swap_Init
================
*/
void Swap_Init (void)
{
	byte	swaptest[2] = {1,0};

// set the byte swapping variables in a portable manner	
	if ( *(int16_t *)swaptest == 1)
	{
		bigendien = false;
		_BigShort = ShortSwap;
		_LittleShort = ShortNoSwap;
		_BigLong = LongSwap;
		_LittleLong = LongNoSwap;
		_BigFloat = FloatSwap;
		_LittleFloat = FloatNoSwap;
	}
	else
	{
		bigendien = true;
		_BigShort = ShortNoSwap;
		_LittleShort = ShortSwap;
		_BigLong = LongNoSwap;
		_LittleLong = LongSwap;
		_BigFloat = FloatNoSwap;
		_LittleFloat = FloatSwap;
	}

}


/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
FIXME: make this buffer size safe someday
============
*/
char	*va(char *format, ...)
{
	va_list		argptr;
	static char		string[1024];
	
	va_start (argptr, format);
//	vsprintf (string, format, argptr);
	Q_vsnprintf (string, sizeof(string), format, argptr);
	va_end (argptr);

	return string;	
}

/*
=======================================================================

TEXT PARSING

=======================================================================
*/

char	com_token[MAX_TOKEN_CHARS];

/*
=================
COM_SkipWhiteSpace
=================
*/
char *COM_SkipWhiteSpace (char *data_p, qboolean *hasNewLines)
{
	int32_t	c;

	while ((c = *data_p) <= ' ')
	{
		if (!c)
			return NULL;

		if (c == '\n') {
		//	com_parseLine++;
			*hasNewLines = true;
		}

		data_p++;
	}

	return data_p;
}


/*
=================
COM_SkipBracedSection

Skips until a matching close brace is found.
Internal brace depths are properly skipped.
=================
*/
void COM_SkipBracedSection (char **data_p, int32_t depth)
{
	char	*tok;

	do
	{
		tok = COM_ParseExt (data_p, true);
		if (tok[1] == 0)
		{
			if (tok[0] == '{')
				depth++;
			else if (tok[0] == '}')
				depth--;
		}
	} while (depth && *data_p);
}


/*
=================
COM_SkipRestOfLine

Skips until a new line is found
=================
*/
void COM_SkipRestOfLine (char **data_p)
{
	char	*tok;

	while (1) {
		tok = COM_ParseExt (data_p, false);
		if (!tok[0])
			break;
	}
}


/*
==============
COM_Parse

Parse a token out of a string
==============
*/
char *COM_Parse (char **data_p)
{
	int32_t		c;
	int32_t		len;
	char	*data;

	data = *data_p;
	len = 0;
	com_token[0] = 0;
	
	if (!data)
	{
		*data_p = NULL;
		return "";
	}
		
// skip whitespace
skipwhite:
	while ( (c = *data) <= ' ')
	{
		if (c == 0)
		{
			*data_p = NULL;
			return "";
		}
		data++;
	}
	
// skip // comments
	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}

// handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
			if (c=='\"' || !c)
			{
				com_token[len] = 0;
				*data_p = data;
				return com_token;
			}
			if (len < MAX_TOKEN_CHARS)
			{
				com_token[len] = c;
				len++;
			}
		}
	}

// parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
		{
			com_token[len] = c;
			len++;
		}
		data++;
		c = *data;
	} while (c>32);

	if (len == MAX_TOKEN_CHARS)
	{
//		Com_Printf ("Token exceeded %i chars, discarded.\n", MAX_TOKEN_CHARS);
		len = 0;
	}
	com_token[len] = 0;

	*data_p = data;
	return com_token;
}


/*
=================
Com_ParseExt

Parse a token out of a string
From Quake2Evolved
=================
*/
char *COM_ParseExt (char **data_p, qboolean allowNewLines)
{
	int32_t			c, len = 0;
	char		*data;
	qboolean	hasNewLines = false;

	data = *data_p;
	com_token[0] = 0;

	// Make sure incoming data is valid
	if (!data) {
		*data_p = NULL;
		return com_token;
	}

	while (1)
	{	// Skip whitespace
		data = COM_SkipWhiteSpace (data, &hasNewLines);
		if (!data) {
			*data_p = NULL;
			return com_token;
		}

		if (hasNewLines && !allowNewLines) {
			*data_p = data;
			return com_token;
		}

		c = *data;
	
		// Skip // comments
		if (c == '/' && data[1] == '/') {
			while (*data && *data != '\n')
				data++;
		}
		else if (c == '/' && data[1] == '*')	// Skip /* */ comments
		{
			data += 2;

			while (*data && (*data != '*' || data[1] != '/')) {
			//	if (*data == '\n')
			//		com_parseLine++;
				data++;
			}

			if (*data)
				data += 2;
		}
		else	// An actual token
			break;
	}

	// Handle quoted strings specially
	if (c == '\"')
	{
		data++;
		while (1)
		{
			c = *data++;
		//	if (c == '\n')
		//		com_parseLine++;

			if (c == '\"' || !c)
			{
				if (len == MAX_TOKEN_CHARS)
					len = 0;

				com_token[len] = 0;

				*data_p = data;
				return com_token;
			}

			if (len < MAX_TOKEN_CHARS)
				com_token[len++] = c;
		}
	}

	// Parse a regular word
	do
	{
		if (len < MAX_TOKEN_CHARS)
			com_token[len++] = c;

		data++;
		c = *data;
	} while (c > 32);

	if (len == MAX_TOKEN_CHARS)
		len = 0;

	com_token[len] = 0;

	*data_p = data;
	return com_token;
}


/*
===============
Com_PageInMemory

===============
*/
int32_t	paged_total;

void Com_PageInMemory (byte *buffer, int32_t size)
{
	int32_t		i;

	for (i=size-1 ; i>0 ; i-=4096)
		paged_total += buffer[i];
}



/*
============================================================================

					LIBRARY REPLACEMENT FUNCTIONS

============================================================================
*/


int32_t Q_strncasecmp (char *s1, char *s2, int32_t n)
{
#if 1
    return strncasecmp(s1, s2, n);
#else
    int32_t		c1, c2;

    do
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--)
			return 0;		// strings are equal until end point
		
		if (c1 != c2)
		{
			if (c1 >= 'a' && c1 <= 'z')
				c1 -= ('a' - 'A');
			if (c2 >= 'a' && c2 <= 'z')
				c2 -= ('a' - 'A');
		}
	} while ((c1 && c2) && (c1 == c2));
	
    if (c1 == c2)
        return 0;		// strings are equal
    else
        return ((c1 > c2) ? 1 : -1);		// strings not equal
#endif

}

int32_t Q_strcasecmp (char *s1, char *s2)
{
    // do this to ensure termination
    return Q_strncasecmp (s1, s2, 99999);
}

size_t Q_strlcpy(char *dest, char* src, size_t size)
{
	strncpy(dest,src,size);
	return size;
}

char *Q_strlwr (char *string)
{
#ifdef _WIN32
    return _strlwr(string);
#else
    char	*s = string;

	while (*s) {
		*s = tolower(*s);
		s++;
	}
	return string;
#endif
}


void Com_sprintf (char *dest, int32_t size, char *fmt, ...)
{
	int32_t		len;
	va_list		argptr;
	char	bigbuffer[0x10000];

	va_start (argptr, fmt);
//	len = vsprintf (bigbuffer, fmt, argptr);
	len = Q_vsnprintf (bigbuffer, sizeof(bigbuffer), fmt, argptr);
	va_end (argptr);
	if (len >= size)
		Com_Printf ("Com_sprintf: overflow of %i in %i\n", len, size);
	strncpy (dest, bigbuffer, size-1);
}

/*
=====================================================================

  INFO STRINGS

=====================================================================
*/

/*
===============
Info_ValueForKey

Searches the string for the given
key and returns the associated value, or an empty string.
===============
*/
char *Info_ValueForKey (char *s, char *key)
{
	char	pkey[512];
	static	char value[2][512];	// use two buffers so compares
								// work without stomping on each other
	static	int32_t	valueindex;
	char	*o;
	
	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			return "";
		s++;
	}
}

void Info_RemoveKey (char *s, char *key)
{
	char	*start;
	char	pkey[512];
	char	value[512];
	char	*o;

	if (strstr (key, "\\"))
	{
//		Com_Printf ("Can't use a key with a \\\n");
		return;
	}

	while (1)
	{
		start = s;
		if (*s == '\\')
			s++;
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value;
		while (*s != '\\' && *s)
		{
			if (!*s)
				return;
			*o++ = *s++;
		}
		*o = 0;

		if (!strcmp (key, pkey) )
		{
			strcpy (start, s);	// remove this part
			return;
		}

		if (!*s)
			return;
	}

}


/*
==================
Info_Validate

Some characters are illegal in info strings because they
can mess up the server's parsing
==================
*/
qboolean Info_Validate (char *s)
{
	if (strstr (s, "\""))
		return false;
	if (strstr (s, ";"))
		return false;
	return true;
}

void Info_SetValueForKey (char *s, char *key, char *value)
{
	char	newi[MAX_INFO_STRING], *v;
	int32_t		c;
	int32_t		maxsize = MAX_INFO_STRING;

	if (strstr (key, "\\") || strstr (value, "\\") )
	{
		Com_Printf ("Can't use keys or values with a \\\n");
		return;
	}

	if (strstr (key, ";") )
	{
		Com_Printf ("Can't use keys or values with a semicolon\n");
		return;
	}

	if (strstr (key, "\"") || strstr (value, "\"") )
	{
		Com_Printf ("Can't use keys or values with a \"\n");
		return;
	}

	if (strlen(key) > MAX_INFO_KEY-1 || strlen(value) > MAX_INFO_KEY-1)
	{
		Com_Printf ("Keys and values must be < 64 characters.\n");
		return;
	}
	Info_RemoveKey (s, key);
	if (!value || !strlen(value))
		return;

	Com_sprintf (newi, sizeof(newi), "\\%s\\%s", key, value);

	if (strlen(newi) + strlen(s) > maxsize)
	{
		Com_Printf ("Info string for key \"%s\" length exceeded\n", key);
		return;
	}

	// only copy ascii values
	s += strlen(s);
	v = newi;
	while (*v)
	{
		c = *v++;
		c &= 127;		// strip high bits
		if (c >= 32 && c < 127)
			*s++ = c;
	}
	*s = 0;
}

//====================================================================


