#pragma once
#include "..\PostManager\ref\Part.h"
#include "..\PostManager\ref\CATIApost_DLL.h"
#include "..\PostManager\ref\Definition.h"
#include "..\PreManager\ref\Part.h"
#include "..\PreManager\ref\CATIApre_DLL.h"
#include "..\PreManager\ref\ReferenceEntity.h"
#import "C:\\Program Files (x86)\\MACRO\\TransCAD\\TransCAD.exe"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;

string Stos(String^ k);
String^ stoS(string k);

public value struct refCommand {
	int command;			//Constraint Type(CTYPE)
	String^ param;			//TransCAD->CATIA의 경우 pack에 대한 정보를 받고//Result로는 catname에 대한 정보를 전달
	String^ MasterPart;
	String^ SlavePart;
	String^ MasterPart_Ref;
	String^ SlavePart_Ref;
	double option;
};

std::string replace_all(
	__in const std::string &message,
	__in const std::string &pattern,
	__in const std::string &replace
);

public ref class PreStack {
private:
	List<String^>^ _dir = gcnew List<String^>();
	List<String^>^ _part_catia_name = gcnew List<String^>();
	List<String^>^ _part_transcad_name = gcnew List<String^>();
public:
	void StackItem(String^ dir, String^ part_catia_name, String^ part_transcad_name) {

		_dir->Add(dir);
		_part_catia_name->Add(part_catia_name);
		_part_transcad_name->Add(part_transcad_name);

	}
	int GetSize() { return _dir->Count; }
	void Clear() { _dir->Clear(); _part_catia_name->Clear(); _part_transcad_name->Clear(); }
	int Getindex_from_name(String^ part_catia_name, int order) {
		int cnt = order;
		for (int i = 0; i < _dir->Count; i++) {
			if (_part_catia_name[i] == part_catia_name) {
				if (--cnt == 0) return i;
			};
		}
		return -1;
	}
	String^ Getitem_from_index(int item, int index) {
		if (item == 0) { return _dir[index]; }
		else if (item == 1) { return _part_catia_name[index]; }
		else if (item == 2) { return _part_transcad_name[index]; }
		else { return ""; }
	}
	//[a,a,s,f,a,a,d,d,e,e,s,s] index = 4 -> return 3
	int HowManySameItems(int item, int index) {
		string _sourceString = Stos(_part_catia_name[index]);
		int itemNum = 0;
		for (int i = 0; i < index; i++) {
			if (Stos(_part_catia_name[i]) == _sourceString) {
				itemNum++;
			}
		}
		return itemNum+1;
	
	}
};

public struct parsing {

	string assem_product = "";			//product1
	int product_num = -1;				//1
	string assem_part = "";				//Part1.2
	int part_num = -1;					//2
	string assem_geometry = "";			//"Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))"

	string Transcad_subAssemName = "";
	string Transcad_partName = "";
	string Transcad_geometry = "";

	void clear() {
		assem_product = ""; assem_part = ""; assem_geometry = "";
		Transcad_subAssemName = ""; Transcad_partName = ""; Transcad_geometry = "";
		product_num = -1; part_num = -1;

	}


};


/*

*/

//////////////////////////////////Read 0////////////////////////////////////////
/*
구현 방법
1 : C++ lib 를 추가
2 : 외부(C#)로 노출시킬 namespaceA, ClassA 생성
3 : 외부(C#)로 노출시킬 functionA
3 : protected로 C++ lib 내부 ClassB 변수를 생성 (<protected 에서>::ClassB *m_c)
                                                (<생성자에서> ClassA::ClassA() :m_c(new ClassB){})
4 : functionA 에서 m_c를 통해 lib내부 functionB를 사용
*/

//////////////////////////////////Read 1////////////////////////////////////////
/*  CLI 변수
C#자료구조형이랑 C++ 자료구조형이 다르기 때문에 변환이 필요

1. (C++ string) std:string = (C# string) String^
	String^ -> string 변환 = std::string c = msclr::interop::marshal_as< std::string >(p);  //p = String^변수
	string -> String^ 변환 = p = msclr::interop::marshal_as<String^>(c);					//p = String^변수

*/
//////////////////////////////////Read 2////////////////////////////////////////
/*
여기서 구현된 함수(functionA)를 C#에서 사용하는 방법
0 : C# 프로젝트에서 참조 추가로 이거 참조해주고
1 : namespaceA.ClassA m_refer = new namespaceA.ClassA();
2 : m_refer.functionA(함수인자)
*/
