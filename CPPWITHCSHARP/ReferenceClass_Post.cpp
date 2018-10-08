
#include "stdafx.h"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#include "ReferenceClass_Post.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;



/** Post Process Reference 관리
**************** Puclic
* //하나의 Constraint를 하기 위한 Master Part와 Slave Part의 참조 ReferenceName을 리턴
* // TransCAD Assembly에서 사용하는 Part들을.CATScript로 변환하기 위한 함수
**************** Private
* //하나의 Component를 받아와서 Part이름에 따라 그 Component의 몇번째 Part인지 번호를 리턴
* //하나의 Assem을 받아와서 SubAssembly이름을 통하여 몇번째 CATIA의 Product에 해당하는지 알아냄
* //Type을 인자로 받아서 CATIA에서 사용하는 consist Name으로 바꿔줌
* //Part 정보를 통해서 TransCAD ReferenceName을 CATIA ReferenceName으로 변환
* //TransCAD Assembly의 Reference 이름을 CATIA Reference 이름으로 변한
**/
namespace ReferenceClass
{

	
	ref_Post::ref_Post() :m_pReferenceManager(new Post::CATIAReferenceManager) {	}///Constructor(생성자) : Connect with C++ Dll
	ref_Post::~ref_Post() { delete m_pReferenceManager;	m_pReferenceManager = 0; }///Destructor(소멸자)


	/**********************************              Public Function             ***********************************************/





	//Refer refCommand value to translate persistentname
	refCommand ref_Post::ConvertRefPost(refCommand w) {						///w.command : Constraint Type(int), 
																			///w.Param : Reference Pack;
																			///w.MasterPart : Master Part Name
																			///w.SlavePart : Slave Part Name
																			///w.MasterPartRef : Master Part Reference Name(TransCAD name)
																			///w.SlavePartRef : Slave Part Reference Name(TransCAD name)
		
		//Step 1 : Code _ Initialize TransCAD values For Get Parts Information

		TransCAD::IApplicationPtr g_spApplication;
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;										///"_spAssem" has "_spComp(SubAssembly information)" and "_spConstraints(Constraint information)"
																			///_spComp(SubAssembly) has Part information
																			
		int status = false;
		HRESULT hr = ::CoInitialize(NULL);
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application)); ///Generate TransCAD instance
		if (FAILED(hr))status = 0;											  ///Generate TransCAD initialize error check

		_spAssemDocument = g_spApplication->ActiveDocument;				      ///Get Activated Assembly document(TransCAD)
		_spAssem = _spAssemDocument->GetAssem();							  ///Generate Assem Object (TransCAD)




		//Step2 : Code _ Translate Persistent Name TransCAD to CATIA
		///Translate MasterPartReferenceName
		string master = T2C_ReferenceName_From_Assembly(_spAssem, w.command, Stos(w.param), Stos(w.MasterPart), Stos(w.MasterPart_Ref));

		///Translate SlavePartReferenceName
		string slave = T2C_ReferenceName_From_Assembly(_spAssem, w.command, Stos(w.param), Stos(w.SlavePart), Stos(w.SlavePart_Ref));




		//Step3 : Code _ Get Result And Return
		::CoUninitialize();						/// COM Automation API Uninitialization

		refCommand result;
		result.command = w.command;              ///Constraint Type id
		result.param = GetConstType(w.command);  ///constraint type name
		result.MasterPart = w.MasterPart;        ///Master Part
		result.SlavePart = w.SlavePart;          ///Slave Part
		result.MasterPart_Ref = stoS(master);    ///Translated Master Reference(CATIA name)
		result.SlavePart_Ref = stoS(slave);      ///Translated Slave Reference(CATIA name)

		return result;
	}






	//Function : Translate Part files ( TransCAD Assembly Part -> .CATScript )
	void ref_Post::AssemblyPartT2C(int CompNum, int PartNum, String^ path) {

		
		//Step 1 : Code _ Initialize TransCAD values For Get Parts Information

		TransCAD::IApplicationPtr g_spApplication;
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;								///"_spAssem" has "_spComp(SubAssembly information)" and "_spConstraints(Constraint information)"
																	///_spComp(SubAssembly) has Part information

		int status = false;
		HRESULT hr = ::CoInitialize(NULL);
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application)); ///Generate TransCAD instance
		if (FAILED(hr))status = 0;											  ///Generate TransCAD initialize error check

		_spAssemDocument = g_spApplication->ActiveDocument;				      ///Get Activated Assembly document(TransCAD)
		_spAssem = _spAssemDocument->GetAssem();							  ///Get Assem Object (TransCAD)
		TransCAD::ICompPtr _spComp = _spAssem->GetComponent(CompNum);		  ///Get Components from Assem (TransCAD)
		TransCAD::IPartPtr part = _spComp->GetPart(PartNum);				  ///Get Part from Components by partnumber (TransCAD)



		//Step 2 : Code _ Get Part infomation
		Post::g_pRefManager = new Post::CATIAReferenceManager();

		Post::Part *exPart;
		exPart = new Post::Part(part, Stos(path));
		exPart->ExternModeToggle(0);								///Mode 0 : To Translate and Export to CATScript
																	///Mode 1 : To Translate but not Export to CATScript, Just Translate naming
		exPart->GetInfo();											///Get Feature Information



		//Step 3 : Code _ Create CATScript
		exPart->ToCATIA();											///Translate and Export to CATScript



		//Step 4 : Initialize translator values
		exPart->InitReferManager();									///Initialize value is important after process
		delete exPart;
		///::CoUninitialize();										//COM Automation API Uninitialization
		///g_pRefManager->~CATIAReferenceManager();
	}






	/**********************************              Private Function             ***********************************************/
	//하나의 Component를 받아와서 Part이름에 따라 그 Component의 몇번째 Part인지 번호를 리턴  //다시 만들어야됨 Assem을 받아와서 몇번째 Component인지에 따라 PartNum이 +되야되는 값이 있다//현재는 그냥 하나의 컴퍼넌트 안에서 몇번째 파트인지만 리턴한다//전체 파트에서 몇번 째 파트인지는 리턴하지 않는다
	int ref_Post::GetPartNum_From_PartName(TransCAD::ICompPtr u_spComp, string PartName) {

		int partNum = 0;

		int partCnt = u_spComp->GetSize();
		
		for (int i = 0; i < partCnt; ++i) {
			string partName;
			TransCAD::IPartPtr part = u_spComp->GetPart(i);
			if ((string)(part->Name) == PartName) {
				partNum = i;
				break;
			}
		}
		return partNum;


	}

	//하나의 Assem을 받아와서 SubAssembly이름을 통하여 몇번째 CATIA의 Product에 해당하는지 알아냄
	int ref_Post::GetProductNum_From_SubAssemName(TransCAD::IAssemPtr u_spAssem, string SubAssemName)
	{

		int SuAssemNum = 0;

		int assemCnt = u_spAssem->GetSize();
		//string a = (string)u_spComp->get_Name();
		for (int i = 0; i < assemCnt; ++i) {
			string partName;
			TransCAD::ICompPtr subassem = u_spAssem->GetComponent(i); //cout << "Name of part [" << i << "] in component1" << part->Name << endl;
			if ((string)(subassem->get_Name()) == SubAssemName) {
				SuAssemNum = i;
				break;
			}
		}
		return SuAssemNum;


		return 0;
	}

	//Type을 인자로 받아서 CATIA에서 사용하는 consist Name으로 바꿔줌
	String^ ref_Post::GetConstType(int type) {
		switch (type) {
		case 0:
			return "catCstTypeOn";
			break;
		case 1:
			return "catCstTypeSurfContact";
			break;
		default:
			return "ANY";
			break;



		}

	}

	//Part 정보를 통해서 TransCAD ReferenceName을 CATIA ReferenceName으로 변환
	string ref_Post::T2C_ReferenceName_From_Part(TransCAD::ICompPtr u_spComp, int partNum, string part_persistentName) {

		string partName;

		TransCAD::IPartPtr part = u_spComp->GetPart(partNum);

		Post::g_pRefManager = new Post::CATIAReferenceManager();
		Post::Part *exPart;
		exPart = new Post::Part(part, 0);
		exPart->ExternModeToggle(1);
		exPart->GetInfo();
		std::string parsing = Post::g_pRefManager->ConvertToBRepFace(part_persistentName);
		cout << "ChangedName : [ " << parsing << "]" << endl;
		exPart->InitReferManager();
		//delete exPart;



		return parsing;

	}

	//TransCAD Assembly의 Reference 이름을 CATIA Reference 이름으로 변한
	string ref_Post::T2C_ReferenceName_From_Assembly(TransCAD::IAssemPtr u_spAssem, int constype, string Pack, string PartName, string assembly_persistentName) {
		//u_spComp(SubAssembly1안의 Parts들에 관한 정보를 담고 있음) 안에 있는 PartName을 검색해서 assembly_persistentName을 T2C로 변환하는 작업

		string temp = assembly_persistentName;											  //SubAssembly1,RotationPart,Cut1,Sketch2,Circle1,0,0,0,ExtrudeFeature:0,0:0:0

		string refer_product = temp.substr(0, temp.find(",") - 0);						  //SubAssembly1 추출    C#::string refer_product = refer.Substring(0, refer.IndexOf(","));
		int SubAssemNum = GetProductNum_From_SubAssemName(u_spAssem, refer_product);      //Product Number 가져오기
		refer_product = "Product" + to_string(SubAssemNum);								  //Product1 변환        C#::refer_product = refer_product.Substring(11);refer_product = "Product" + refer_product;

		TransCAD::ICompPtr u_spComp = u_spAssem->GetComponent(SubAssemNum);
		int partNum = GetPartNum_From_PartName(u_spComp, PartName);						  //PartNum 가져오기
		string refer_Part = "Part1." + to_string(partNum + 1);							  //Part1.1(Part이름.똑같은Part이름우선순위)          C#::string refer_Part = "Part1." + (partNum + 1).ToString();
		string refer_pack = Pack;                                                         //Axis:(           C#::string refer_coaxial = "Axis" + ":(";
		string refer_from_part = temp.substr(temp.find(",", temp.find(",") + 1) + 1);     //Cut1,Sketch2,Circle1,0,0,0,ExtrudeFeature:0,0:0:0  C#::string refer_from_part = refer.Substring(refer.IndexOf(",") + 1);refer_from_part = refer_from_part.Substring(refer_from_part.IndexOf(",") + 1);refer_from_part = m_refer.EX(m_param);
		refer_from_part = T2C_ReferenceName_From_Part(u_spComp, partNum, refer_from_part);


		string result = "\"" + refer_product + "/" + refer_Part + "/!" + refer_pack + refer_from_part + ")\"";

		return result;

	}




}
