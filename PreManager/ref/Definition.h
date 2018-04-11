#pragma once
#include <strstream>
#include <iostream>
using std::ostrstream;
using std::endl;    
using std::ends;
using std::cerr;

#include <math.h>

//--------------  Types ----------------//
//-------------- Struct ----------------//
//-------------- Etc... ----------------//
namespace Pre {
typedef class Vector3D Point3D;
typedef class Vector3D Direct3D;

	class Vector3D {
	public:

		Vector3D(void) : _x(0.0), _y(0.0), _z(0.0) {}
		Vector3D(double x, double y, double z) : _x(x), _y(y), _z(z) {}
		Vector3D(double coord[]) : _x(coord[0]), _y(coord[1]), _z(coord[2]) {}

		double X(void) const { return _x; }
		double Y(void) const { return _y; }
		double Z(void) const { return _z; }
		void SetX(double x) { _x = x; }
		void SetY(double y) { _y = y; }
		void SetZ(double z) { _z = z; }
		void SetXYZ(double sz[3]) { _x = sz[0]; _y = sz[1]; _z = sz[2]; }
		void SetXYZ(double x, double y, double z) { _x = x; _y = y; _z = z; }

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
			return (_x*d._x + _y*d._y + _z*d._z);
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

		double Size() const { return sqrt(_x*_x + _y*_y + _z*_z); }
		Vector3D Norm(void) { return Vector3D(_x, _y, _z) / Size(); }

	private:
		double _x, _y, _z;
	};
}

	enum EntityType {
		Entity_VERTEX,
		Entity_EDGE,
		Entity_FACE,
		Entity_FEATURE,
		Entity_SKETCHSEGMENT,
		Entity_SURFACE,
		Entity_PLANE,
		Entity_HDirection,
		Entity_VDirection,
		Entity_Origin
	};


	enum EndType {
		End_Dimension,
		End_ThroughAll,
		End_ThroughNext,
		End_UpToVertex,
		End_UpToSurface,
		End_UpToOffsetFromSurface,
		End_MidPlane,
		End_UpToPlane,
		End_UpToLast
	};


	enum PropagationType {
		Minimal,
		Tangency
	};

	//-------------- Functions ----------------//

	extern void Map(const double A[12], double B[3]);
	extern void Cir_Map(const double A[12], double Center[3]);
	extern void ValRndPnt(double C[3]);
	extern double ValRnd(double tmp);
	extern void ValRndPnt6(double C[3]);
	extern double ValRnd6(double tmp);
	extern int report_error(char *file, int line, char *call, int irc);
	extern int WaitGetEnter(void);


	enum FType {
		FEATURE,
		DATUM_PLANE,	// not in the draft
		SKETCH,			// not in the draft
		SKETCH_Create_2D_Line_2Points,
		SKETCH_Create_2D_Centerline,
		SKETCH_Create_2D_Point,
		SKETCH_Create_2D_Arc_CenterEnds,
		SKETCH_Create_2D_Arc_3Points,
		SKETCH_Create_2D_Circle_CenterPoint,
		SOLID_Create_Protrusion_Extrude,
		SOLID_Create_Protrusion_Revolve,
		SOLID_Create_Protrusion_Sweep,
		SOLID_Operate_Filleting_Fillet_Constant,
		SOLID_Operate_Pattern_Rectangular
	};



	struct FaceInfo
	{
		string _featureName;
		string _sketchName1;
		string _sketchItemName1;
		string _sketchName2;
		string _sketchItemName2;
		string _featureType;
	};

	typedef vector<FaceInfo> FaceInfoList;
	typedef vector<FaceInfoList> EdgeInfo;


	struct WireInfo
	{
		string _sketchName;
		string _sketchItemName;
	};
