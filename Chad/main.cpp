// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <comdef.h>  // _com_error
#include <system_error>

#include "config.h"
#include "console_controller.h"
#include "devices.h"

int main(const int argc, char* argv[])
{
	// initialize COM
	try {
		ThrowOnFail(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE));

		Devices devices; // greater
		const util::ConsoleController console(devices);
		console.HandleInput(argc, argv);
		
	} catch(_com_error& err) {
		std::wcerr << "COM error occured: " << err.ErrorMessage() << std::endl; // a little more todo here
	} catch (const std::out_of_range & err) {
		std::wcerr << "Out of Range error: " << err.what() << std::endl;
	} catch (std::exception& err) {
		std::wcerr << err.what() << std::endl;
	} catch (std::runtime_error& err) {
		std::wcerr << err.what() << std::endl;
	} catch (...) {
		std::wcerr << "Unknown exception " << std::endl;
	}
	CoUninitialize();
	return 0;
}
