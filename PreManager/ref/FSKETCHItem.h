#pragma once
#include "FSketch.h"
namespace Pre {
	class FSketch;

	class FSKETCHItem
	{
	public:
		FSKETCHItem(FSketch * pFSketch, int type, string name);
		~FSKETCHItem(void);

		FSketch * GetFeatureSketch() { return _pFSketch; }
		string	GetSketchItemName() { return _name; }
		int	GetSketchItemType() { return _type; }
		void SetCenterLineInfo(int centerLineInfo) { _centerLineInfo = centerLineInfo; }

		virtual void GetInfo(ifstream & is);
		virtual void ToTransCAD();

		virtual void Modify(char * buffer);

	protected:
		FSketch * _pFSketch;
		string _name;
		int _type;
		int _id;  //Report Name을 저장하는 루틴
		int _centerLineInfo;
	};
}