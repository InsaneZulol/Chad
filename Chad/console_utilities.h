#pragma once
#include "pch.h"


#include <fcntl.h> // setmode
#include <io.h> // setmode
#include <mmdeviceapi.h>
#include "endpoint.h"
#include "devices.h"
namespace utilities {
	class ConsoleUtilities {
		const std::wstring str_render_dev_ = L"Playback devices: ";
		const std::wstring str_capture_dev_ = L"Recording devices: ";
		std::vector<std::wstring> vec_render_dev_name_;
		std::vector<std::wstring> vec_capture_dev_name_;
	private:
		int GetDeviceNames(EDataFlow dir);
	public:
		ConsoleUtilities();
		int SetOutputMode(int code_page, int translation_mode);
		void HandleInput(int argc, char* argv[]);
		void PrintEndpointsColumn() const;
		void PrintEndpointNames(EDataFlow dir) const;
	};

}