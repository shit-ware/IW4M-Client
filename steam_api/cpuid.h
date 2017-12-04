#include <string>
#include <sstream>
#include <intrin.h>
#include <iostream>
using namespace std;

std::string Get_CPUID(){

	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0);
	if (CPUInfo[0] < 4)
		return "";

	stringstream st;
	for (int i = 0; i <= 3; i++){
		st << CPUInfo[i];
	}

	return st.str();

}