// Vector3D.h
// 3D vector class
//
// from http://stackoverflow.com/questions/12872981/physical-vector-representation
//

#include "Vector3D.h"

namespace geom {

bool Vector3D::operator == (Vector3D rhs) const
{
    return ((x_==rhs.x()) && (y_==rhs.y()) && (z_==rhs.z()));
}

Vector3D& Vector3D::operator +=(const Vector3D& rhs)
{
    x_ += rhs.x_;
    y_ += rhs.y_;
    z_ += rhs.z_;
    return *this;
}

Vector3D& Vector3D::operator -=(const Vector3D& rhs)
{
    x_ -= rhs.x_;
    y_ -= rhs.y_;
    z_ -= rhs.z_;
    return *this;
}

Vector3D& Vector3D::operator *=(const float& sc)
{
    x_ *= sc;
    y_ *= sc;
    y_ *= sc;
    return *this;
}

Vector3D operator + (Vector3D& lhs, Vector3D& rhs)
{
    return Vector3D(lhs.x()+rhs.x(), lhs.y()+rhs.y(), lhs.z()+rhs.z());
}

Vector3D operator - (Vector3D& lhs, Vector3D& rhs)
{
    return Vector3D(lhs.x()-rhs.x(), lhs.y()-rhs.y(), lhs.z()-rhs.z());
}

template <class Scalar>
Vector3D operator * (Scalar& sc, Vector3D& vect)
{
    return Vector3D(sc*vect.x(), sc*vect.y(), sc*vect.z());
}

Vector3D operator * (float& sc, Vector3D& vect)
{
    return Vector3D(sc*vect.x(), sc*vect.y(), sc*vect.z());
}

template <class Scalar>
Vector3D operator * (Vector3D& vect, Scalar& sc)
{
    return sc*vect;
}

float dot(Vector3D& lhs, Vector3D& rhs) 
{ 
    return (lhs.x()*rhs.x()+lhs.y()*rhs.y()+lhs.z()*rhs.z());
}

float dotSqr(Vector3D v)
{ return dot(v, v); }

//Vector3D cross(const Vector3D& lhs, const Vector3D& rhs) { /* implement cross product */ }

} // namespace

// vim:foldmethod=syntax:foldlevel=1
