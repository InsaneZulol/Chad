// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <comdef.h>  // _com_error
#include <system_error>
#include "console_controller.h"
#include "devices.h"

int main(int argc, char* argv[])
{
	// initialize COM
	try {
		ThrowOnFail(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE));

		Devices devices;
		util::ConsoleController console;
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
