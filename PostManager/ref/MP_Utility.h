#ifndef ___MP_UTILITY____ 
#define ___MP_UTILITY____ 

#include "PN_TYPE.h"
namespace Post {
	string Print_F(EN_F f);
	string Print_E(EN_E e);
	string Print_V(EN_V v);

	string Print_BN(basic_naming bn);
	string Print_OSI(OSI osi);
	string Print_SN(secondary_name sn);


	void Parsing_F(string str, EN_F & f);

	void Parsing_E(string str, EN_E & e);

	void Parsing_V(string str, EN_V & v);

	void Parsing_BN(string str, basic_naming & bn);

	void Parsing_OSI(string str, OSI & osi);

	void Parsing_SN(string str, secondary_name & sn);

	void Delim_V(string input, string & f1, string & f2, string & f3, string & osi); // string ==> F, F, F, OSI로 분리

	void Delim_E(string input, string & f1, string & f2, string & osi); // string ==> F, F, OSI로 분리

	void Delim_F(string input, string & bn, string & osi, string & sn); // string ==> BN, OSI, Secondary Name으로 분리
}


#endif
