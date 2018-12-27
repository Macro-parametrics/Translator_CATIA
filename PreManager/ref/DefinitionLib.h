#pragma once
#include <stdio.h>
#include <strstream>
#include <iostream>

using std::string;
using std::ostrstream;



#define f_Getline_Custom(s,b,n) strcpy_s(b,stringLibrary::_Getline_Custom(s,n).c_str());


namespace Pre {

	class stringLibrary {

	public:
		static string LtoU(string source, int index) {

			string src = source;
			char f = src.at(index);

			if (2 == islower(f)) { // 소문자이면

				src[index] = f - ('a' - 'A');

			}

			return src;

		}

		static string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
			size_t start_pos = 0; //string처음부터 검사
			while ((start_pos = str.find(from, start_pos)) != std::string::npos)  //from을 찾을 수 없을 때까지
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length(); // 중복검사를 피하고 from.length() > to.length()인 경우를 위해서
			}
			return str;
		}

		static string _Getline_Custom(std::ifstream &_is, int n) {

			char* s_buff = new char[n];

			_is.getline(s_buff, 500);

			return ReplaceAll(s_buff, "sketch", "Sketch");//sketch를 Sketch로 변환 (TransCAD 내부에서 Sketch로 데이터를 처리하는듯)

		}


	};
}