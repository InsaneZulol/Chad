#pragma once
#include "pch.h"

#include <mmdeviceapi.h>
#include "endpoint.h"
class Devices;

namespace util {
	
	class ConsoleController {
		
	public:
		ConsoleController();
		static int SetOutputMode(int code_page, int translation_mode);
		void HandleInput(int argc, char* argv[], Devices& devices) const;
		void PrintEndpointsColumn(const std::vector<Endpoint>& ren_col, const std::vector<Endpoint>& cap_col) const;
		void PrintEndpointNames(EDataFlow dir, const std::vector<Endpoint>& dev_col) const;
	private:
		void PrintOnChangedEndpoint(const std::wstring& device_name) const;
	};

}
