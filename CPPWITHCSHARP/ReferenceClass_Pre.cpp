
#include "stdafx.h"
#include <vcclr.h>
#include <msclr/marshal_cppstd.h>

#include "ReferenceClass_Pre.h"
/** Pre Process Reference 관리
*
*
**/
namespace ReferenceClass
{
	//생성자 C++ 코드 Dll 연결시켜주는 곳

	//TestEnvironment가 필요 없다면 이 생성자 사용
	ref_Pre::ref_Pre() {

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
		//*_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		*_spAssem = (*_spAssemDocument)->GetAssem();

		pre_data = new parsing();
	}

	//TestEnvironment가 필요하면 이 생성자 사용
	ref_Pre::ref_Pre(PreStack^ buffer) {

		TestEnvironment(buffer);

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

	void ref_Pre::TestEnvironment(PreStack^ buffer) {
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



		int part_size = buffer->GetSize();
		TransCAD::IPartPtr *_tPart = new TransCAD::IPartPtr[part_size];

		for (int i = 0; i < part_size; i++) {

			//파트 Script 파일 경로 얻어오기 
			string pPath = Stos(buffer->Getitem_from_index(0, i)); //0 : _dir
			pPath = replace_all(pPath, "CATPart", "CATScript"); //.CATPart -> .CATScript

																//파트 Script 파일 번역
			_spPartDocument = Pre::g_spApplication->GetDocuments()->AddPartDocument();
			Pre::Part *pConstrained = new Pre::Part(pPath, _spPartDocument);
			pConstrained->GetInfo();
			pConstrained->ToTransCAD();

			_tPart[i] = pConstrained->_spPart;
			printf("Part %d is success translated", i);
		}

		//어셈블리 다큐먼트 생성 및 파트 파일 추가
		_spAssemDocument = Pre::g_spApplication->GetDocuments()->AddAssemDocument();
		_spAssemDocument = Pre::g_spApplication->ActiveDocument;
		_spAssem = _spAssemDocument->GetAssem();
		_spComp = _spAssem->CreateComponent();//SubAssembly1
		_spComp->set_Name("Component1");

		for (int i = 0; i < part_size; i++) {
			_spComp->AddPart(_tPart[i]);
		}
		_spAssem->AddComponent(_spComp);
		_spAssemDocument->Update();


	}

	void ref_Pre::PreTest(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref)
	{


	}

	refCommand ref_Pre::SetConstraint(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref, String^ _option, int^ _option_int)
	{
		// SetConstraint(PreStack 인스턴스 , int 파트개수 , string Const타입 , string MasterRef, string SlaveRef, string option, int optionNum)

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
		string full_ref[(int)TARGETPART::REFERENCE_SIZE] = { Stos(_master_ref), Stos(_slave_ref) };

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
		string param = Constraint_to_TransCAD(_spConstraints, CstType, TargetPart[(int)TARGETPART::MASTER], _spConstrainedGeometry, TargetPart[(int)TARGETPART::SLAVE], _spReferGeometry, Stos(_option), _option_int);


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



		string temp = Stos(ref);																								//Product1/Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))
		string assem_product = temp.substr(0, temp.find('/'));																	//Product1

		temp = temp.substr(temp.find('/') + 1, temp.size());																	//Part1.1/!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))
		string assem_part = temp.substr(0, temp.find('/'));																		//Part1.1

		temp = temp.substr(temp.find('/') + 1, temp.size());																	//!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))
		string assem_geometry = temp.substr(0, temp.find('/'));																	//!Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))
		assem_geometry = "\"" + assem_geometry.substr(assem_geometry.find('!') + 1, assem_geometry.size()) + "\"";				//"Axis:(Selection_RSur:(Face:(Brp:(Pocket.1;0:(Brp:(Sketch.2;4)));None:();Cf11:());Pocket.1_ResultOUT;Z0;G3563))"


		*product = assem_product;
		*part = assem_part;
		*geometry = assem_geometry;
	}

	int ref_Pre::GetTransCADName_from_buffer(PreStack^ buffer, string* Transcad_subAssemName, string* Transcad_partName, string* Transcad_geometry) {

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

		if (geo.substr(1, 6) == "Axis:(") { geo = "\"" + geo.substr(7, geo.size()); }//축을 선택한 것이라면 맨 앞부분 Axis 제거
		char* ptr = (char*)geo.c_str();
		*Transcad_geometry = *Transcad_subAssemName + "," + *Transcad_partName + "," + _refer->GetReferName(ptr);

		delete _refer;
		//delete pPart;

		pre_data->product_num = comp_num;
		pre_data->part_num = part_num;



		return buffer_index;
	}

	string ref_Pre::Constraint_to_TransCAD(TransCAD::IStdAssemConstraintsPtr f, string type, TransCAD::IPartPtr master, TransCAD::IReferencePtr master_ref, TransCAD::IPartPtr slave, TransCAD::IReferencePtr slave_ref, string option, int^ option_int) {
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
			re = "None";

		}

		return re;

	}

}