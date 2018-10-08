#pragma once

#include <math.h>


#define MAX_CHAR_LIMIT	20
#define MAX_DIGITS		18
#define MAX_DEPTH		1000000
namespace Post {
	typedef class Vector3D Point3D;
	typedef class Vector3D Direct3D;

	class Vector3D
	{
	public:
		Vector3D(void) : _x(0.0), _y(0.0), _z(0.0) {}
		Vector3D(double x, double y, double z) : _x(x), _y(y), _z(z) {}
		Vector3D(double coord[]) : _x(coord[0]), _y(coord[1]), _z(coord[2]) {}

		double X(void) const { return _x; }
		double Y(void) const { return _y; }
		double Z(void) const { return _z; }

		void X(double x) { _x = x; }
		void Y(double y) { _y = y; }
		void Z(double z) { _z = z; }

		void SetXYZ(double x, double y, double z) { _x = x; _y = y; _z = z; }
		void SetXYZ(double rhs[3]) { _x = rhs[0]; _y = rhs[1]; _z = rhs[2]; }

		double & operator[](int i) { return *(&_x + i); }

		Vector3D operator+(const Vector3D & rhs) {
			return Vector3D(_x + rhs._x, _y + rhs._y, _z + rhs._z);
		}
		Vector3D operator-(const Vector3D & rhs) {
			return Vector3D(_x - rhs._x, _y - rhs._y, _z - rhs._z);
		}
		Vector3D operator*(double d) {
			return Vector3D(_x * d, _y * d, _z * d);
		}
		Vector3D operator/(double d) {
			return Vector3D(_x / d, _y / d, _z / d);
		}
		// Dot product
		double operator*(Vector3D & d) {
			return (_x*d._x + _y * d._y + _z * d._z);
		}
		// Cross Product
		Vector3D operator^(Vector3D & d) {
			double i = _y * d._z - _z * d._y;
			double j = _z * d._x - _x * d._z;
			double k = _x * d._y - _y * d._x;
			return Vector3D(i, j, k);
		}
		bool operator==(const Vector3D & rhs)
		{
			return _x == rhs._x && _y == rhs._y && _z == rhs._z;
		}
		void operator=(const Vector3D & rhs) {
			_x = rhs._x; _y = rhs._y; _z = rhs._z;
		}

		double Size() const { return sqrt(_x*_x + _y * _y + _z * _z); }
		Vector3D Norm(void) { return Vector3D(_x, _y, _z) / Size(); }

	private:
		double _x, _y, _z;
	};


	enum LimitMode
	{
		OFFSET_LIMIT,
		UP_TO_NEXT_LIMIT,
		UP_TO_LAST_LIMIT,
		UP_TO_PLANE_LIMIT,
		UP_TO_SURFACE_LIMIT,
		UP_THRU_NEXT_LIMIT
	};
}