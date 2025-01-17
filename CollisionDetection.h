#pragma once

#include <vectormath/cpp/vectormath_aos.h>
#include "Body.h"

class Sphere_c 
{
public:
	Sphere_c(void)
	{
		m_pos = Vector3(0, 0, 0);
		m_radius = 0;
	}
	Sphere_c(const Vector3& pos, float radius)
	{
		m_pos = pos;
		m_radius = radius;
	}
	Vector3	m_pos;
	float	m_radius;
};

class CollisionData_c 
{
public:
	Vector3	m_point;
	Vector3	m_normal;
	float	m_penetration;
	CollisionData_c(void) {
		m_point = Vector3(0,0,0);
		m_normal = Vector3(0,0,0);
		m_penetration = 0;
	}
};

/* 
	Does sphere-sphere collision detection test
	returns 
		true - collision
		false - no-collision 
	optional 
		CollisionData is a pointer, so if you want the 
		collision information (contact point etc), you pass 
		it in, and it will be filled in 
	warnings collisionData will only be filled in, if there is a collision 
*/
inline bool SphereSphereCollision( const Sphere_c& s0,
							const Sphere_c& s1, 
							CollisionData_c* collisionData
							)
{
	const Vector3 dist = s0.m_pos - s1.m_pos;

	const float distSq = dot( dist , dist );
	
	const float sumRadius = (s0.m_radius + s1.m_radius);

	if(distSq < sumRadius*sumRadius)
	{
		if(collisionData)
		{
			collisionData->m_penetration = sumRadius - sqrtf(distSq);

			Vector3 dist = s0.m_pos - s1.m_pos;
			collisionData->m_normal = normalize(dist);
			collisionData->m_point = s0.m_pos - collisionData->m_normal * (s0.m_radius - collisionData->m_penetration*0.5f);
		}
		return true;
	}
	return false;
}

class Cube_c {
public:
	Cube_c(const Vector3& pos, const Vector3& dims ) { m_pos=pos; m_dims=dims; }
	Vector3 m_pos;
	Vector3 m_dims;
	Matrix4 transform;
};


inline bool CubeCubeAABB( const Cube_c& c0, const Cube_c& c1 ) {
	const Vector3 dist = (c0.m_pos - c1.m_pos); 
	const Vector3 e = (c0.m_dims + c1.m_dims) * 0.5f; 
	if ( ( abs(dist.getX()) <= e.getX() ) && ( abs(dist.getY()) <= e.getY() ) && ( abs(dist.getZ()) <= e.getZ() ) ) { 
		return true; // Collision 
	} 
	return false; // No Collision 
}

class Box_c {
public:
	Box_c(void) {};
	Box_c(const Vector3& pos, const Vector3& dims ) { m_pos=pos; m_dims=dims; }
	Vector3 m_pos;
	Vector3 m_dims;
	Matrix4 transform;
};

inline bool CubeSphere(const Box_c& box, 
					   const Sphere_c& sphere, 
					   CollisionData_c* collisionData) 
{
	Vector3 relCenter = (inverse(box.transform) * Point3(sphere.m_pos)).getXYZ();
	if( abs(relCenter.getX()) - sphere.m_radius > box.m_dims.getX() ||
		abs(relCenter.getY()) - sphere.m_radius > box.m_dims.getY() ||
		abs(relCenter.getZ()) - sphere.m_radius > box.m_dims.getZ() )
	{
		return 0;
	}
	
	Vector3 closestPoint;
	float dist;

	dist = relCenter.getX();
	if(dist > box.m_dims.getX() / 2) 
	{
		dist = box.m_dims.getX() / 2;
	}
	if(dist < -box.m_dims.getX() / 2)
	{
		dist = -box.m_dims.getX() / 2;
	}
	closestPoint.setX(dist);

	dist = relCenter.getY();
	if(dist > box.m_dims.getY() / 2)
	{
		dist = box.m_dims.getY() / 2;
	}
	if(dist < -box.m_dims.getY() / 2)
	{
		dist = -box.m_dims.getY() / 2;
	}
	closestPoint.setY(dist);

	dist = relCenter.getZ();
	if(dist > box.m_dims.getZ() / 2)
	{
		dist = box.m_dims.getZ() / 2;
	}
	if(dist < -box.m_dims.getZ() / 2)
	{
		dist = -box.m_dims.getZ() / 2;
	}
	closestPoint.setZ(dist);

	dist = dot(closestPoint - relCenter, closestPoint - relCenter);
	if(dist > sphere.m_radius * sphere.m_radius)
	{
		return 0;
	}

	Vector3 closestPointWorld = (box.transform * Point3(closestPoint)).getXYZ();

	if(collisionData)
	{
		Vector3 distance = sphere.m_pos - closestPointWorld;
		collisionData->m_penetration = sphere.m_radius - length(distance);
		collisionData->m_point = closestPointWorld;
		collisionData->m_normal = normalize(distance);
	}

	return 1;
}