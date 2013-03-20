#ifndef VECTOR3D_H_
#define VECTOR3D_H_

//#include <math.h>
#include <iostream>
#include <plugins/physics.h>

using namespace std;

class Vector3D
{
	dReal x, y, z;
	
public:
	Vector3D();
	Vector3D(dReal x, dReal y, dReal z);
	virtual ~Vector3D();
	
	friend ostream &operator << (ostream &out, Vector3D &v);
	
	Vector3D & operator= (const Vector3D & v);
	bool operator== (const Vector3D & v);
	bool operator!= (const Vector3D & v);
    
	Vector3D & operator+= (const Vector3D & v);
    Vector3D operator+ (const Vector3D & v) const;

    Vector3D & operator-= (const Vector3D & v);
    Vector3D operator- (const Vector3D & v) const;

    Vector3D & operator*= (const dReal a);
    Vector3D operator* (const dReal a)const;
    friend Vector3D operator* (const dReal a,const Vector3D & v);

    Vector3D & operator/= (const dReal a);
    Vector3D operator/ (const dReal a)const;

	/*
	 * compute the difference between two vector3D
	 */
	Vector3D diff(Vector3D &that);
	
	/*
	 * compute the angle between two vector3D
	 */
	dReal computeAngle(Vector3D &that);
	
	/*
	 * returns the normalised vector
	 */
	Vector3D normalise();
	
	/*
	 * returns the lenght of the vector
	 */
	 dReal length();
	
	/*
	 * returns the dot product of two vectors
	 */
	dReal dot(Vector3D &that);
	
	/*
	 * returns the cross product of two vectors
	 */
	 Vector3D cross(Vector3D &that);

	/*
	 * returns the magnitude of a vector
	 */
	 dReal magnitude();
	
	/*
	 * getters and setters
	 */
	void setX(dReal x);
	dReal getX();

	void setY(dReal y);
	dReal getY();

	void setZ(dReal z);
	dReal getZ();

};

#endif /*VECTOR3D_H_*/
