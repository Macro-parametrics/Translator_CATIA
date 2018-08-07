#pragma once
#include "ReferenceClass.h"

namespace ReferenceClass
{




	public ref class ref_Pre
	{
		enum class TARGETPART : int { MASTER, SLAVE, REFERENCE_SIZE };//REFERENCE_SIZE는 항상 맨뒤에 있어야한다.

	public:
		ref_Pre();
		ref_Pre(PreStack^ buffer);
		virtual ~ref_Pre();

	private:
		//::CATIAReferenceManager	*m_pReferenceManager;
	public:
		TransCAD::IAssemDocumentPtr* _spAssemDocument;
		TransCAD::IAssemPtr* _spAssem;
		TransCAD::ICompPtr* _spComp;

		parsing* pre_data;

	public:
		////////////실제로 구현할 메서드 추가//////////////
		/*외부로 노출시킬 함수를 만들고 이 안에서 m_pReferenceManager를 통해 실제 함수를 사용한다*/
		void PreTest(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref);
		void TestEnvironment(PreStack^ buffer);
		refCommand ref_Pre::SetConstraint(PreStack^ buffer, int totnum, String^ _CstType, String^ _master_ref, String^ _slave_ref, String^ _option, int^ _option_int);

		void Parsing(String^ ref, string* product, string* part, string* geometry);
		int GetTransCADName_from_buffer(PreStack^ buffer, string* Transcad_subAssemName, string* Transcad_partName, string* Transcad_geometry);
		string Constraint_to_TransCAD(TransCAD::IStdAssemConstraintsPtr f, string type, TransCAD::IPartPtr master, TransCAD::IReferencePtr master_ref, TransCAD::IPartPtr slave, TransCAD::IReferencePtr slave_ref, string option, int^ option_int);

	};
}