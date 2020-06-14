#include "pch.h"
#include "console_utilities.h"

#include <iomanip>
namespace utilities {
	constexpr auto SPACING = 3;
	ConsoleUtilities::ConsoleUtilities() {
		SetConsoleOutputCP(CP_UTF8);
		// from now on remember to either:
		// a) only use wcout 
		// b) setmode to O_TEXT before couting
		_setmode(_fileno(stdout), _O_U8TEXT);
	}

	// Prints two columns of endpoints.
	// Remember to pass render devices as first parameter so it's displayed under correct label in console.
	void ConsoleUtilities::PrintEndpointNames(const std::vector<Endpoint>& render_devices, const std::vector<Endpoint>& recording_devices) {

		size_t longest_name_len = 0;
		for (const auto& i : render_devices) {
			if (longest_name_len < i.device_name.length()) longest_name_len = i.device_name.length();
		}

		const std::wstring str_render_dev = L"Playback devices: ";
		const std::wstring str_capture_dev = L"Recording devices: ";
		const auto num_render_dev = render_devices.size();
		const auto num_capture_dev = recording_devices.size();
		const auto more_render = recording_devices.size() < render_devices.size();
		std::wcout << str_render_dev << std::setw(longest_name_len + SPACING + 1) << str_capture_dev << std::endl;
		for (auto i = 0; i < more_render ? num_render_dev : num_capture_dev; i++) {
			if (i < num_render_dev) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left << render_devices.at(i).device_name;
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << recording_devices.at(i).device_name << std::endl;
			}
		}
	}

	// Prints a list of endpoints
	void ConsoleUtilities::PrintEndpointNames(const std::vector<Endpoint>& input) {
		for (const auto& i : input) {
			std::wcout << i.device_name << std::endl;
		}
		fflush(stdout);
	}
}