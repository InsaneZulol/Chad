#pragma once
#include "pch.h"

#include <mmdeviceapi.h>

namespace util {
	
	class ConsoleController {
		
		std::vector<std::wstring> vec_render_dev_name_;
		std::vector<std::wstring> vec_capture_dev_name_;
	private:
		int GetDeviceNames(EDataFlow dir);
	public:
		ConsoleController();
		int SetOutputMode(int code_page, int translation_mode);
		void HandleInput(int argc, char* argv[]);
		void PrintEndpointsColumn() const;
		void PrintEndpointNames(EDataFlow dir) const;
	};

}