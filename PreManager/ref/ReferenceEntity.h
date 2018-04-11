#pragma once
#include "Part.h"
#include "FSketch.h"
namespace Pre {
	class Part;
	class FSketch;

	class ReferenceEntity
	{
	public:
		ReferenceEntity(Part * pPart, int type, string name);

		string GetReferName(char *& context);
		virtual ~ReferenceEntity(void);

		virtual void GetInfo(ifstream &is);
		virtual	void ToTransCAD();

		virtual int GetReferenceType() { return type; }
		virtual string GetName() { return name; }
		virtual string GetFeatureName() { return feature; }
		virtual string GetAxisName() { return axis; }

		virtual TransCAD::IReferencePtr GetReferencePtr() { return spReferencePtr; }

		FaceInfoList GetFaceInfo() { return pFace; }
		EdgeInfo GetEdgeInfo() { return pEdge; }
		WireInfo GetWireInfo() { return pWire; }

	public:
		Part * _pPart;

	protected:
		string GetFullFaceName(char *& buffer);
		string GetBasicFaceName(char *& buffer);

		string name;
		string feature;
		string axis;

		int type;		// 0=none ; 1=default datum plane; 2=edge; 3=surface; 4=object; 5=point

		string TransCADFaceName;
		string TransCADEdgeName;
		TransCAD::IReferencePtr spReferencePtr;

	private:
		FSketch * pSketchManager;
		FaceInfoList pFace;
		EdgeInfo pEdge;
		WireInfo pWire;
	};

}