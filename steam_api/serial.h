#include <string>
#include <sstream>
#include "md5.h"
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <comdef.h>
#include <Wbemidl.h>

#define _WIN32_DCOM

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wbemuuid.lib")

using namespace std;


DWORD get_volume_serial()
{
	DWORD serial = 0;
	GetVolumeInformationA("C:\\", 0, 0, &serial, 0, 0, 0, 0);
	return serial;
}

string Get_CPUID(){

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

std::string& BstrToStdString(const BSTR bstr, std::string& dst, int cp = CP_UTF8)
{
    if (!bstr)
    {
        // define NULL functionality. I just clear the target.
        dst.clear();
        return dst;
    }

    // request content length in single-chars through a terminating
    //  nullchar in the BSTR. note: BSTR's support imbedded nullchars,
    //  so this will only convert through the first nullchar.
    int res = WideCharToMultiByte(cp, 0, bstr, -1, NULL, 0, NULL, NULL);
    if (res > 0)
    {
        dst.resize(res);
        WideCharToMultiByte(cp, 0, bstr, -1, &dst[0], res, NULL, NULL);
    }
    else
    {    // no content. clear target
        dst.clear();
    }
    return dst;
}

string BstrToStdString(BSTR bstr, int cp = CP_UTF8)
{
    std::string str;
    BstrToStdString(bstr, str, cp);
    return str;
}


string getMID()
{

    HRESULT hres;

    hres =  CoInitializeEx(0, COINIT_MULTITHREADED); 
    if (FAILED(hres))
    {
		Com_Error(0,"something bad happened here");
    }

    hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );

                      
    if (FAILED(hres))
    {
		Com_Error(0,"something bad happened here");
    }

    IWbemLocator *pLoc = NULL;

    hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
 
    if (FAILED(hres))
    {
		Com_Error(0,"something bad happened here");
    }

    IWbemServices *pSvc = NULL;

    hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (for example, Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
    
    if (FAILED(hres))
    {
		Com_Error(0,"something bad happened here");
    }

    hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );

    if (FAILED(hres))
    {
		Com_Error(0,"something bad happened here");
    }

    IEnumWbemClassObject* pEnumerator = NULL;
    hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_BaseBoard"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
    
    if (FAILED(hres))
    {
		Com_Error(0,"something bad happened here");
    }
 
    IWbemClassObject *pclsObj;
    ULONG uReturn = 0;
   
    while (pEnumerator)
    {
        HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, 
            &pclsObj, &uReturn);

        if(0 == uReturn)
        {
            break;
        }

        VARIANT vtProp;

        // Get the value of the Name property
        hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0); //get motherboard serial number

		string mid = BstrToStdString(vtProp.bstrVal); //convert bstr to std::string

		//clear value
        VariantClear(&vtProp);
        pclsObj->Release();

		return mid;
    }
}

string getHWID()
{
		ostringstream ostream;
		string combination_hashed;

		DWORD  volume_serial = get_volume_serial();
		ostream << volume_serial;

		string mid = getMID();

		string combination = mid + ostream.str();
		combination_hashed = md5(combination);

		return combination_hashed;
}