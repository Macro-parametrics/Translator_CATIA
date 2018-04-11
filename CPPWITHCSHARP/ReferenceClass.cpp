
#include "stdafx.h"
#include "ReferenceClass.h"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;

string Stos(String^ k) { return  msclr::interop::marshal_as< std::string >(k); }
String^ stoS(string k) { return  msclr::interop::marshal_as<String^>(k); }

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
	
	//생성자 C++ 코드 Dll 연결시켜주는 곳
	ref_Post::ref_Post() :m_pReferenceManager(new CATIAReferenceManager) {	}

	//소멸자
	ref_Post::~ref_Post() { delete m_pReferenceManager;	m_pReferenceManager = 0; }


	/**********************************아래부터는 Public 함수들***********************************************/


	//하나의 Constraint를 하기 위한 Master Part와 Slave Part의 참조 ReferenceName을 리턴
	refCommand ref_Post::ConvertRefPost(refCommand w) {		   //w.command : Constraint Type(int), 
															   //w.Param : Reference Pack;
															   //w.MasterPart : Master Part Name
															   //w.SlavePart : Slave Part Name
															   //w.MasterPartRef : Master Part Reference Name
															   //w.SlavePartRef : Slave Part Reference Name
		#pragma region Code _ Initialize For Get Parts Information

		//dll을 통해서 Part Document에 접근
		TransCAD::IApplicationPtr g_spApplication;
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		//_spAssem 아래에 _spComp(SubAssembly정보=Parts정보)들과 _spConstraints(Constraint정보)가 들어있다
		//_spComp(product) 아래에 Part정보가 들어 있다
		
		
		//COM initialize
		int status = false;
		HRESULT hr = ::CoInitialize(NULL);
		//TransCAD instance 생성
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TransCAD initialize error check
		if (FAILED(hr))status = 0;
		//Assembly document 생성
		_spAssemDocument = g_spApplication->ActiveDocument;
		//Assem 객체 생성
		_spAssem = _spAssemDocument->GetAssem();

		#pragma endregion



		#pragma region Code _ Translate Reference Name TransCAD to CATIA
		//Translate MasterPartReferenceName
		string master = T2C_ReferenceName_From_Assembly(_spAssem, w.command, Stos(w.param), Stos(w.MasterPart), Stos(w.MasterPart_Ref));

		//Translate SlavePartReferenceName
		string slave = T2C_ReferenceName_From_Assembly(_spAssem, w.command, Stos(w.param), Stos(w.SlavePart), Stos(w.SlavePart_Ref));
		#pragma endregion



		#pragma region Code _ Get Result And Return
		::CoUninitialize();						// COM Automation API Uninitialization

		refCommand result;
		result.command = w.command;              //Constraint Type id
		result.param = GetConstType(w.command);  //constraint type name
		result.MasterPart = w.MasterPart;        //Master Part
		result.SlavePart = w.SlavePart;          //Slave Part
		result.MasterPart_Ref = stoS(master);    //Master Reference
		result.SlavePart_Ref = stoS(slave);      //Slave Reference
		#pragma endregion
		return result;
	}

	//TransCAD Assembly에서 사용하는 Part들을 .CATScript로 변환하기 위한 함수
	void ref_Post::ExtentionFuc(int CompNum,int PartNum,String^ path) {
	
		//dll을 통해서 Part Document에 접근
		TransCAD::IApplicationPtr g_spApplication;
		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		//_spAssem 아래에 _spComp(SubAssembly정보=Parts정보)들과 _spConstraints(Constraint정보)가 들어있다
		//_spComp(product) 아래에 Part정보가 들어 있다


		//COM initialize
		int status = false;
		HRESULT hr = ::CoInitialize(NULL);
		//TransCAD instance 생성
		hr = g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TransCAD initialize error check
		if (FAILED(hr))status = 0;
		//Assembly document 생성
		_spAssemDocument = g_spApplication->ActiveDocument;
		//Assem 객체 생성
		_spAssem = _spAssemDocument->GetAssem();
		
		TransCAD::ICompPtr _spComp = _spAssem->GetComponent(CompNum);
		TransCAD::IPartPtr part = _spComp->GetPart(PartNum);

		g_pRefManager = new CATIAReferenceManager();
		
		Part *exPart;
		exPart = new Part(part,Stos(path));
		exPart->ExternModeToggle(0);//0이면 정상적으로 Part를 GetInfo 한다
		                            //1이면 Assembly를 하기위한 Part를 GetInfo하는 모드로 바뀜
		exPart->GetInfo();
		exPart->ToCATIA();
		exPart->InitReferManager(); //파트를 변환하였으면 사용했던 변수들을 모두 다시 초기화한다(다음 파트들도 변환하기 위해서...)
		delete exPart;
		//::CoUninitialize();						// COM Automation API Uninitialization
		//g_pRefManager->~CATIAReferenceManager();
				
	
	}






	/**************************************** 여기부터는 Private 함순들 ********************************************/
	//하나의 Component를 받아와서 Part이름에 따라 그 Component의 몇번째 Part인지 번호를 리턴  //다시 만들어야됨 Assem을 받아와서 몇번째 Component인지에 따라 PartNum이 +되야되는 값이 있다//현재는 그냥 하나의 컴퍼넌트 안에서 몇번째 파트인지만 리턴한다//전체 파트에서 몇번 째 파트인지는 리턴하지 않는다
	int ref_Post::GetPartNum_From_PartName(TransCAD::ICompPtr u_spComp, string PartName) {

		int partNum = 0;

		int partCnt = u_spComp->GetSize();
		//string a = (string)u_spComp->get_Name();
		for (int i = 0; i < partCnt; ++i) {
			string partName;
			TransCAD::IPartPtr part = u_spComp->GetPart(i); //cout << "Name of part [" << i << "] in component1" << part->Name << endl;
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

		g_pRefManager = new CATIAReferenceManager();
		Part *exPart;
		exPart = new Part(part, 0);
		exPart->ExternModeToggle(1);
		exPart->GetInfo();
		std::string parsing = g_pRefManager->ConvertToBRepFace(part_persistentName);
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



/** Pre Process Reference 관리
*
*
**/
namespace ReferenceClass
{
	//생성자 C++ 코드 Dll 연결시켜주는 곳
	ref_Pre::ref_Pre() {	

		TestEnvironment();

		//COM Initialize
		int status = false;
		HRESULT hr = CoInitialize(NULL);
		hr = Pre::g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		if (FAILED(hr))status = 0;
		if (!Pre::g_spApplication->Visible) { Pre::g_spApplication->Visible = VARIANT_TRUE; }

		_spAssemDocument = new TransCAD::IAssemDocumentPtr();
		_spAssem = new TransCAD::IAssemPtr();
	
		
		//현재 Assem Document 가져오기
		//TransCAD::IAssemDocumentPtr set = Pre::g_spApplication->ActiveDocument;
		*_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		*_spAssem = (*_spAssemDocument)->GetAssem();

		pre_data = new parsing();
	}



	//소멸자
	ref_Pre::~ref_Pre() { ::CoUninitialize(); }

	void ref_Pre::TestEnvironment() {
		//COM Initialize
		int status = false;
		HRESULT hr = CoInitialize(NULL);
		//TransCAD instance 생성
		hr = Pre::g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		//TrnasCAD initialzie error check
		if (FAILED(hr))status = 0;

		//TransCAD 가시화
		if (!Pre::g_spApplication->Visible) {

			Pre::g_spApplication->Visible = VARIANT_TRUE;
		}

		TransCAD::IAssemDocumentPtr _spAssemDocument;
		TransCAD::IAssemPtr _spAssem;
		TransCAD::ICompPtr _spComp;
		TransCAD::IPartDocumentPtr _spPartDocument;


		//파트 파일 번역

		char* asfe1 = "D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATScript";
		char* asfe2 = "D:\\project\\Macro\\TestAssem\\SubAssembly1\\Body.CATScript";

		_spPartDocument = Pre::g_spApplication->GetDocuments()->AddPartDocument();
		Pre::Part _spConstraintedPart(asfe1, _spPartDocument);
		_spConstraintedPart.GetInfo();
		_spConstraintedPart.ToTransCAD();

		_spPartDocument = Pre::g_spApplication->GetDocuments()->AddPartDocument();
		Pre::Part _spReferPart(asfe2, _spPartDocument);
		_spReferPart.GetInfo();
		_spReferPart.ToTransCAD();


		//어셈블리 다큐먼트 생성 및 파트 파일 추가
		_spAssemDocument = Pre::g_spApplication->GetDocuments()->AddAssemDocument();

		_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		_spAssem = _spAssemDocument->GetAssem();
		_spComp = _spAssem->CreateComponent();//SubAssembly1
		_spComp->set_Name("Component1");

		_spComp->AddPart(_spConstraintedPart._spPart);
		_spComp->AddPart(_spReferPart._spPart);

		_spAssem->AddComponent(_spComp);
		_spAssemDocument->Update();
	
	}

	void ref_Pre::PreTest(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref) {

	
	}

	refCommand ref_Pre::SetConstraint(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref, String^ _option, int^ _option_int) {
																											/*Input 값
																											1 : Total Part Num
																											2 : [Rotation,Body], [Part1,Part1], [D:\\project\\Macro\\TestAssem\\SubAssembly1\\RotationPart.CATScript, "D:\project\Macro\TestAssem\SubAssembly1\Body.CATPart"]
																											(TransCAD이름/파일_이름, CATIA이름, CATIA이름에 따른 경로)
																											3 : catCstTypeOn,
																											4 : "Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))"
																											5 : "Product1/Part1.2/!Axis:(Selection_RSur:(Face:(Brp:(Pad.2;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pad.2_ResultOUT;Z0;G3563))"
																											*/

		//Step1 : 초기화 - TrnasCAD 및 Pre 변수 초기화
#pragma region Step1 : 초기화 - TrnasCAD 및 Pre 변수 초기화

		string CstType = Stos(_CstType);
		string full_ref[(int)TARGETPART::REFERENCE_SIZE] = { Stos(_master_ref), Stos(_slave_ref)};

		//현재 Assem Document 가져오기
		//Pre::g_spApplication.CreateInstance(__uuidof(TransCAD::Application));
		*_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		*_spAssem = (*_spAssemDocument)->GetAssem();


#pragma endregion 
 
#pragma region Step2 :parsing & Translate

			

		TransCAD::ICompPtr _IspComp;
		TransCAD::IPartPtr TargetPart[(int)TARGETPART::REFERENCE_SIZE]; //MASTER,SLAVE
		int index[(int)TARGETPART::REFERENCE_SIZE];
		string trans_ref[(int)TARGETPART::REFERENCE_SIZE];

		


		/********Step2_1 : cn = 0 : Master Part와 Master_geometry 찾기********/
		/********Step2_1 : cn = 1 : Slave Part와 Slave_geometry 찾기********/
		for (int cn = 0; cn < 2; cn++) {

			Parsing(stoS(full_ref[cn]), &pre_data->assem_product, &pre_data->assem_part, &pre_data->assem_geometry);
			index[cn] = GetTransCADName_from_buffer(buffer, &pre_data->Transcad_subAssemName, &pre_data->Transcad_partName, &pre_data->Transcad_geometry);
			trans_ref[cn] = pre_data->Transcad_geometry;
			
			//transcad name 을 통해 TransCAD Part 인스턴스를 가져옴
			_IspComp = (*_spAssem)->GetComponent(pre_data->product_num);
			for (int i = 0; i < _IspComp->GetSize(); i++) {
				if ((string)(_IspComp->GetPart(i)->Name) == pre_data->Transcad_partName) {
					TargetPart[cn] = _IspComp->GetPart(i);
					break;
				}
			}
		
		}


#pragma endregion

#pragma region Step3 : TransCAD에 Constraint 적용
		//Constraints관리자를 가져오고, //각 Contratint의 Master/Slave Part의 Reference Name을 저장할 ReferencePtr을 생성 
		TransCAD::IStdAssemConstraintsPtr _spConstraints;
		_spConstraints = (*_spAssem)->GetConstraints();//Constraint 관리자

		TransCAD::IReferencePtr _spConstrainedGeometry, _spReferGeometry;

		//각 Constraint의 Master Part의 Reference Name을 _spConstrainedGeometry에 저장
		//각 Constraint의 Slave Part의 Reference Name을 _spReferGeometry에 저장
		_spConstrainedGeometry = TargetPart[(int)TARGETPART::MASTER]->SelectBrepByName(trans_ref[(int)TARGETPART::MASTER].c_str());
		_spReferGeometry = TargetPart[(int)TARGETPART::SLAVE]->SelectBrepByName(trans_ref[(int)TARGETPART::SLAVE].c_str());

		//Constraint를 하기 위해 Constraint 관리자에게 Constraint 정보를 넣어 준다
		//"Constraint Type", "Master Part", "Master Reference Name", "Slave Part", "Slave ReferenceName"

		//_spConstraints->AddNewAssemblyCoaxialConstraint("Coaxial", MasterPart, _spConstrainedGeometry, SlavePart, _spReferGeometry);
		//or AddNewAssemblyIncidenceConstraint "Incidence"

		//_spConstraints->AddNewAssemblyIncidenceConstraint("Incidence", TargetPart[(int)TARGETPART::MASTER], _spConstrainedGeometry, TargetPart[(int)TARGETPART::SLAVE], _spReferGeometry, TransCAD::StdAssemblyIncidenceType::Opposite);
		string fjeiakso = trans_ref[(int)TARGETPART::MASTER];
		cout << fjeiakso << endl;
		fjeiakso = trans_ref[(int)TARGETPART::SLAVE];
		cout << fjeiakso << endl;
		string param = Constraint_to_TransCAD(_spConstraints, CstType, TargetPart[(int)TARGETPART::MASTER], _spConstrainedGeometry, TargetPart[(int)TARGETPART::SLAVE], _spReferGeometry,Stos(_option),_option_int);
		
		
		(*_spAssemDocument)->Update();
#pragma endregion

#pragma region Step3 : 결과 내보내기
		refCommand result;
		result.command = TRUE;
		result.param = stoS(param);
		result.MasterPart = buffer->Getitem_from_index(2, index[(int)TARGETPART::MASTER]);
		result.MasterPart_Ref = stoS(trans_ref[(int)TARGETPART::MASTER]);
		result.SlavePart = buffer->Getitem_from_index(2, index[(int)TARGETPART::SLAVE]);
		result.SlavePart_Ref = stoS(trans_ref[(int)TARGETPART::SLAVE]);

		cout << "Contraint Translate Result" << endl;
		cout << Stos(result.MasterPart) << " : " << Stos(result.MasterPart_Ref) << endl;
		cout << Stos(result.SlavePart) << " : " << Stos(result.SlavePart_Ref) << endl;


#pragma endregion
		

		

		return result;
	}

	void ref_Pre::Parsing(String^ ref, string* product, string* part, string* geometry) {

		

		string temp = Stos(ref);
		string assem_product = temp.substr(0, temp.find('/'));

		temp = temp.substr(temp.find('/') + 1, temp.size());
		string assem_part = temp.substr(0, temp.find('/'));

		temp = temp.substr(temp.find('/') + 1, temp.size());
		string assem_geometry = temp.substr(0, temp.find('/'));
		assem_geometry = "\"" + assem_geometry.substr(assem_geometry.find('!') + 1, assem_geometry.size()) + "\"";


		*product = assem_product; 
		*part = assem_part; 
		*geometry = assem_geometry;
	}

	int ref_Pre::GetTransCADName_from_buffer(PreStack^ buffer,string* Transcad_subAssemName,string* Transcad_partName,string* Transcad_geometry) {

		TransCAD::ICompPtr _spComp;
		string product = pre_data->assem_product;
		string part = pre_data->assem_part;
		string geo = pre_data->assem_geometry;

		//Step1 : Product1 를 통해서 TransCAD의 Component(SubAssembly) Name과 Number를 가져옴
		int comp_num = stoi(product.substr(product.find("t") + 1, product.size()));//Product 숫자 Parsing
		_spComp = (*_spAssem)->GetComponent(comp_num);
		*Transcad_subAssemName = (string)_spComp->get_Name();
		

		//Step2-1 : Part1.1 을 통해서 Part1 과 Part1의 Number를 가져옴
		int part_num = stoi(part.substr(part.find(".") + 1, part.size()));
		part = part.substr(0, part.find("."));
		//Step2-2 : buffer에서 Number 번째 Part1의 index를 가져옴
		int buffer_index = buffer->Getindex_from_name(stoS(part), part_num);
		//Step2-3 : buffer에서 index에 해당하는 transcad name을 가져옴
		*Transcad_partName = Stos(buffer->Getitem_from_index(2, buffer_index));


		//Step3 : CATScript로부터 Geometry를 변환
		 string _path = Stos(buffer->Getitem_from_index(0, buffer_index));
		_path = _path.substr(0, _path.find_last_of('.')) + ".CATScript";

		Pre::Part* pPart = new Pre::Part(_path, 1);
		pPart->GetInfo();															//Part 정보 읽기
		Pre::ReferenceEntity* _refer = new Pre::ReferenceEntity(pPart, 0, "a");		//Part에서 Feature정보 추출

		if (geo.substr(1, 6) == "Axis:(") { geo  = "\"" + geo.substr(7, geo.size()); }//축을 선택한 것이라면 맨 앞부분 Axis 제거
		char* ptr = (char*)geo.c_str();
		*Transcad_geometry = *Transcad_subAssemName + "," + *Transcad_partName + "," + _refer->GetReferName(ptr);

		delete _refer;
		//delete pPart;

		pre_data->product_num = comp_num;
		pre_data->part_num = part_num;

		

		return buffer_index;
	}

	string ref_Pre::Constraint_to_TransCAD(TransCAD::IStdAssemConstraintsPtr f,string type, TransCAD::IPartPtr master,TransCAD::IReferencePtr master_ref, TransCAD::IPartPtr slave, TransCAD::IReferencePtr slave_ref,string option, int^ option_int ) {
		string re;
		if (option == "move") {
			TransCAD::ICompPtr _lomp = (*_spAssem)->GetComponent(pre_data->product_num);
			_lomp->SetPartPlacement(master, 10, 100, 100, 0, 0, 1, 1, 0, 0);
			(*_spAssemDocument)->Update();
		}
		

		if (type == "catCstTypeOn") {

			f->AddNewAssemblyCoaxialConstraint("Coaxial", master, master_ref, slave, slave_ref);
			re = "Coaxial";

		}
		else if (type == "catCstTypeSurfContact") {
			if (option == "same") {
				f->AddNewAssemblyIncidenceConstraint("Incidence", master, master_ref, slave, slave_ref, TransCAD::StdAssemblyIncidenceType::Same);
			}
			else {
			
				f->AddNewAssemblyIncidenceConstraint("Incidence", master, master_ref, slave, slave_ref, TransCAD::StdAssemblyIncidenceType::Opposite);
			}
			
			re = "Incidence";
			
		}
		else {
			re =  "None";
		
		}

		return re;
		
	}

}