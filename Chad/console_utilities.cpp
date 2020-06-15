#include "pch.h"
#include "console_utilities.h"
#include <iomanip>
#include <array>

namespace utilities {
	constexpr auto SPACING = 6;
	constexpr auto AVAILABLE_COMMANDS_PRINT_TEXT = "list, li, l, print,  - to list available audio devices";
	constexpr auto AVAILABLE_COMMANDS_CHANGE_TEXT = "set, change, select, sel - to set selected audio device as default system one";
	std::vector<std::string> VEC_COMMANDS_PRINT{"list", "li", "print", "l"};
	std::vector<std::string> VEC_COMMANDS_CHANGE{"set", "change", "select", "sel"};

	struct WordMatch {
		const std::string s_;
		WordMatch(std::string const& s) : s_(s) {}
		bool operator()(std::string const& s) const { return s == s_; }
	};

	ConsoleUtilities::ConsoleUtilities() {
		SetConsoleOutputCP(CP_UTF8);
		// from now on remember to either:
		// a) only use wcout 
		// b) setmode to O_TEXT before couting
		_setmode(_fileno(stdout), _O_U8TEXT);
	}
	
	int ConsoleUtilities::GetDeviceNames(EDataFlow dir) {
		Devices devices;

		if(dir == eRender) {
			const auto dev_list = devices.render_list_;
			for (auto i = 0; i < dev_list.size(); i++) {
				vec_render_dev_name_[i] = dev_list.at(i).id_num + L": " + dev_list.at(i).device_name;
			}
			return vec_render_dev_name_.size();
		}
		if (dir == eCapture) {
			const auto dev_list = devices.capture_list_;
			for (auto i = 0; i < dev_list.size(); i++) {
				vec_capture_dev_name_[i] = dev_list.at(i).id_num + L": " + dev_list.at(i).device_name;
			}
			return vec_capture_dev_name_.size();
		}
		if (dir == eAll) {
			const auto render_bigger = devices.render_list_.size() > devices.capture_list_.size();
			const auto dev_list = render_bigger ? devices.render_list_ : devices.capture_list_;
			for (auto i = 0; i < dev_list.size(); i++) {
				if(devices.render_list_.size() > i) vec_render_dev_name_[i] = devices.render_list_.at(i).id_num + L": " + devices.render_list_.at(i).device_name;
				if(devices.capture_list_.size() > i) vec_capture_dev_name_[i] = devices.capture_list_.at(i).id_num + L": " + devices.capture_list_.at(i).device_name;
			}
			return vec_render_dev_name_.size() + vec_capture_dev_name_.size();
		}
		return -1;
	}

	int ConsoleUtilities::SetOutputMode(const int code_page, const int translation_mode) {
		fflush(stdout);
		SetConsoleOutputCP(code_page);
		return  _setmode(_fileno(stdout), translation_mode);
	}

	void ConsoleUtilities::HandleInput(int argc, char* argv[]) {
		// if program was started without any params:
		if (argc <= 1) {
			std::string input;
			std::wcout << "Welcome in Chad.\n" << "Type help to list available commands." << std::endl;
			std::cin >> input;
		}
		// if program was started with one param:
		if (argc == 2) {
			const std::string action(argv[1]);
			if (action == "help") {
				std::wcout << "Welcome in Chad. Available commands:" << std::endl
					<< AVAILABLE_COMMANDS_PRINT_TEXT << std::endl
					<< AVAILABLE_COMMANDS_CHANGE_TEXT << std::endl;
				return;
			}
			if(std::any_of(VEC_COMMANDS_PRINT.begin(), VEC_COMMANDS_PRINT.end(), WordMatch(action))) {
				std::wcout << "PRINTER GO BRRR \n";
				GetDeviceNames(eAll);
				//PrintEndpointNames(devices.render_list_, devices.capture_list_);
				return;
			}
			if (std::any_of(VEC_COMMANDS_CHANGE.begin(),
				VEC_COMMANDS_CHANGE.end(), WordMatch(action))) {
				std::wcout << "Please repeat the command and specify the device" << std::endl;
			} else {
					std::wcout << "Command unrecognized" << std::endl;
			}
		}
	}

	// Prints two columns of endpoints.
	// Remember to pass render devices as first parameter so it's displayed under correct label in console.
	void ConsoleUtilities::PrintEndpointsColumn(const std::vector<Endpoint>& render_devices, const std::vector<Endpoint>& recording_devices) const {
		// Get Devices names;
		// save Devices names to own Vector of strings,
		// Print Own vector  of strings
		//
		//
		size_t longest_name_len = 0;
		for (const auto& i : render_devices) {
			if (longest_name_len < i.device_name.length()) longest_name_len = i.device_name.length();
		}
		const auto num_render_dev = render_devices.size();
		const auto num_capture_dev = recording_devices.size();
		const auto more_render = recording_devices.size() < render_devices.size();
		std::wcout << str_render_dev_ << std::setw(longest_name_len + SPACING + 1) << str_capture_dev_ << std::endl;
		for (auto i = 0; i < (more_render ? num_render_dev : num_capture_dev); i++) {
			if (i < num_render_dev) {
				std::wcout << render_devices.at(i).id_num << ": " << std::setw(longest_name_len + SPACING/2) << std::left  << render_devices.at(i).device_name;
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << recording_devices.at(i).id_num << ": " << recording_devices.at(i).device_name << std::endl;
			}
		}
	}

	// Prints a list of endpoints
	void ConsoleUtilities::PrintEndpointNames(const std::vector<Endpoint>& input, EDataFlow dir) const {

		dir == eRender ? (std::wcout << str_render_dev_ << std::endl) : (std::wcout << str_capture_dev_ << std::endl);

		for (const auto& i : input) {
			std::wcout << i.device_name << std::endl;
		}
		fflush(stdout);
	}
}