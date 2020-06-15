// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <comdef.h>  // _com_error
#include <system_error>
#include "console_utilities.h"
#include "devices.h"
#include "ipolicyconfig.h"


void SetDefaultDevice(__in PCWSTR wszDeviceId, __in::ERole Role) {

	const IID CLSID_PolicyConfig = __uuidof(CPolicyConfigClient);
	const IID IID_IPolicyConfig = __uuidof(IPolicyConfig);
	CComPtr<IPolicyConfig> ptr_PolicyConfig;
	HRESULT Result = ptr_PolicyConfig.CoCreateInstance(CLSID_PolicyConfig);
	if (S_OK != Result)
		std::wcout<<"Error occured while creating CoCreateInstance CPolicyConfig" << std::endl;

	Result = ptr_PolicyConfig->SetDefaultEndpoint(wszDeviceId, Role);
	if (S_OK != Result)
		std::wcout << "Error ptr_PolicyConfig->SetDefaultEndpoint" << std::endl;
}


int main(int argc, char* argv[])
{
	// initialize COM
	try {
		ThrowOnFail(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE));

		util::ConsoleUtilities console;
		Devices devices;
		console.HandleInput(argc, argv);

	} catch(_com_error& err) {
		std::wcout << "Exception occured while initializing com: " << err.ErrorMessage() << std::endl; // a little more todo here
	} catch (const std::out_of_range & err) {
		std::cerr << "Out of Range error: " << err.what() << '\n';
	}
	catch (...) {
		std::wcout << "Unknown exception " << std::endl;
	}
	CoUninitialize();
	return 0;
}
