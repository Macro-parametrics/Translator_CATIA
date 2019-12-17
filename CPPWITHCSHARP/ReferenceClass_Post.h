#pragma once
#include "ReferenceClass.h"

namespace ReferenceClass
{
	public delegate void SelectedDelegateCSharp(Object ^j);//For c# Raise
	public delegate void SelectedDelegateCplus(int i); //for C++ Raise

	public ref class ref_Post
	{


	public:
		ref_Post();
		virtual ~ref_Post();

	protected:
		Post::CATIAReferenceManager	*m_pReferenceManager;

	private:
		int GetPartNum_From_PartName(TransCAD::IComponentPtr u_spComp, string PartName, int _index);
		int GetProductNum_From_SubAssemName(TransCAD::IAssemPtr u_spAssem, string SubAssemName);
		string T2C_ReferenceName_From_Part(TransCAD::IComponentPtr u_spComp, int partNum, string persistentName);
		string T2C_ReferenceName_From_Assembly(TransCAD::IAssemPtr u_spAssem, int constype, string Pack, string PartName, string assembly_persistentName);
		String^ GetConstType(int type);
	public:
		////////////실제로 구현할 메서드 추가//////////////
		/*외부로 노출시킬 함수를 만들고 이 안에서 m_pReferenceManager를 통해 실제 함수를 사용한다*/
		//::Part * pPart;
		refCommand ConvertRefPost(refCommand w);
		void AssemblyPartT2C(int CompNum, int PartNum, String^ path);
		static void ref_Post::ExportPart(String^ path);
		PreStack _tstack;

		int Set_stack_info(PreStack^ _stack) {
			_tstack.Clear();
			if (_stack->GetSize() < 1)return 0;

			for (int i = 0; i < _stack->GetSize(); i++) {
			
				_tstack.StackItem(_stack->Getitem_from_index(0,i), _stack->Getitem_from_index(1, i), _stack->Getitem_from_index(2, i));
			}
			return 1;
			
		}


	};



}