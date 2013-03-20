#include "Vector3D.h"

using namespace std;

Vector3D::Vector3D()
{
}

Vector3D::Vector3D(dReal x, dReal y, dReal z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3D::~Vector3D()
{
}

ostream &operator <<(ostream &out, Vector3D &v)
{
	//out << "<" << this->x << ";" << this->y << ";" << this->z << ">";
	//out << "<" << v.getX() << ";" << v.getY() << ";" << v.getZ() << ">";
	//out << "asdasda";
	return out << "<" << v.getX() << ";" << v.getY() << ";" << v.getZ() << ">";
}


Vector3D & Vector3D::operator= (const Vector3D & v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

bool Vector3D::operator== (const Vector3D & v)
{
    return x == v.x && y == v.y && z == v.z;
}

bool Vector3D::operator!= (const Vector3D & v)
{
	return x != v.x || y != v.y || z != v.z;
}
Vector3D & Vector3D::operator+= (const Vector3D & v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3D Vector3D::operator+ (const Vector3D & v) const
{
    Vector3D t = *this;
    t += v;
    return t;
}

Vector3D & Vector3D::operator-= (const Vector3D & v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

Vector3D Vector3D::operator- (const Vector3D & v) const
{
    Vector3D t = *this;
    t -= v;
    return t;
}

Vector3D & Vector3D::operator*= (const dReal a)
{
    x *= a;
    y *= a;
    z *= a;
    return *this;
}

Vector3D Vector3D::operator* (const dReal a)const
{
    Vector3D t = *this;
    t *= a;
    return t;
}

Vector3D operator* (const dReal a,const Vector3D & v)
{
    return Vector3D(v.x*a,v.y*a,v.z*a);
}

Vector3D & Vector3D::operator/= (const dReal a)
{
    x /= a;
    y /= a;
    z /= a;
    return *this;
}

Vector3D Vector3D::operator/ (const dReal a)const
{
    Vector3D t = *this;
    t /= a;
    return t;
}

Vector3D Vector3D::diff(Vector3D &that){
	return Vector3D(this->x-that.getX(),this->y-that.getY(),this->z-that.getZ());
}

dReal Vector3D::computeAngle(Vector3D &that){
	return acos(this->dot(that)/(this->length()*that.length())); 
}

dReal Vector3D::length()
{
	return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

Vector3D Vector3D::normalise()
{
	dReal my_length = length();
	if (my_length > 0) {
		this->x /= my_length;
		this->y /= my_length;
		this->z /= my_length;
	}
	return Vector3D(this->x, this->y, this->z);
}

// Is equal to the norm
dReal Vector3D::magnitude()
{
	return sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
}

Vector3D Vector3D::cross(Vector3D &that)
{
	return Vector3D(this->y*that.getZ() - this->z*that.getY(), this->z*that.getX() - this->x*that.getZ(), this->x*that.getY() - this->y*that.getX());
}

dReal Vector3D::dot(Vector3D &that)
{
	return this->x*that.getX() + this->y*that.getY() + this->z*that.getZ();
}
 
void Vector3D::setX(dReal x)
{
	this->x = x;
}

dReal Vector3D::getX()
{
	return this->x;
}

void Vector3D::setY(dReal y)
{
	this->y = y;
}

dReal Vector3D::getY()
{
	return this->y;
}

void Vector3D::setZ(dReal z)
{
	this->z = z;
}

dReal Vector3D::getZ()
{
	return this->z;
}
