#include "pch.h"
#include "console_controller.h"
#include <codecvt>
#include "devices.h"
#include <iomanip>
#include <fcntl.h> // setmode
#include <io.h> // setmode
#include "string_literals.h"

namespace util {
	
	struct WordMatch {
		const std::string s_;
		WordMatch(std::string const& s) : s_(s) {}
		bool operator()(std::string const& s) const { return s == s_; }
	};

	// compares the first character of a wstring, returns bool
	struct CharIdMatch {
		const std::wstring s_;
		//const int pos_;
		CharIdMatch(const std::wstring s) : s_(s) {}
		bool operator()(const std::wstring s) const { return s[0] == s_[0]; }
	};


	ConsoleController::ConsoleController() {
		SetConsoleOutputCP(CP_UTF8);
		// from now on remember to either:
		// a) only use wcout 
		// b) setmode to O_TEXT before couting
		_setmode(_fileno(stdout), _O_U8TEXT);
		GetDeviceNames(eAll);
	}

	// Updates internal string list of endpoint names
	// Return value is number of devices added to internal list
	int ConsoleController::GetDeviceNames(EDataFlow dir) {
		Devices devices;

		// clear vector to update
		if (!vec_render_dev_name_.empty()) vec_render_dev_name_.clear();
		if (!vec_capture_dev_name_.empty()) vec_capture_dev_name_.clear();
		
		if(dir == eRender) {
			const auto dev_list = devices.render_list_;
			for (auto i = 0; i < dev_list.size(); i++) {
				vec_render_dev_name_[i] = dev_list.at(i).num_id + L": " + dev_list.at(i).device_name;
			}
			return vec_render_dev_name_.size();
		}
		if (dir == eCapture) {
			const auto dev_list = devices.capture_list_;
			for (auto i = 0; i < dev_list.size(); i++) {
				vec_capture_dev_name_[i] = dev_list.at(i).num_id + L": " + dev_list.at(i).device_name;
			}
			return vec_capture_dev_name_.size();
		}
		if (dir == eAll) {
			const auto render_bigger = devices.render_list_.size() > devices.capture_list_.size();
			const auto dev_list = render_bigger ? devices.render_list_ : devices.capture_list_;
			for (auto i = 0; i < dev_list.size(); i++) {
				if (devices.render_list_.size() > i) {
					std::wstring ren_dev_text = std::to_wstring(devices.render_list_.at(i).num_id) + L": " + devices.render_list_.at(i).device_name;
					vec_render_dev_name_.push_back(ren_dev_text);
				};
				if (devices.capture_list_.size() > i) {
					std::wstring cap_dev_text = std::to_wstring(devices.capture_list_.at(i).num_id) + L": " + devices.capture_list_.at(i).device_name;
					vec_capture_dev_name_.push_back(cap_dev_text);
				};
			}
			return vec_render_dev_name_.size() + vec_capture_dev_name_.size();
		}
		return -1;
	}

	// Sets console output translation mode
	// use _O_U8TEXT for Unicode, O_TEXT for narrow charset
	// Returns _setmode() success code
	int ConsoleController::SetOutputMode(const int code_page, const int translation_mode) {
		fflush(stdout);
		SetConsoleOutputCP(code_page);
		return  _setmode(_fileno(stdout), translation_mode);
	}

	void ConsoleController::HandleInput(int argc, char* argv[]) {
		// if program was started without params:
		if (argc <= 1) {
			std::string input;
			std::wcout << WELCOME_TEXT << std::endl;
			std::cin >> input;
		}
		// one param:
		if (argc == 2) {
			const std::string action(argv[1]);
			if (action == "help") {
				std::wcout << WELCOME_WHELP_TEXT << std::endl
					<< AVAILABLE_COMMANDS_PRINT_TEXT << std::endl
					<< AVAILABLE_COMMANDS_CHANGE_TEXT << std::endl;
				return;
			}
			// list, li, l, print
			if(std::any_of(COMMANDS_PRINT.begin(), COMMANDS_PRINT.end(), WordMatch(action))) {
				GetDeviceNames(eAll);
				PrintEndpointsColumn();
				return;
			}
			// set, change, select, sel
			if (std::any_of(COMMANDS_CHANGE.begin(), COMMANDS_CHANGE.end(), WordMatch(action))) {
				std::wcout << "Please repeat the command and specify the device" << std::endl;
			} else {
					std::wcout << "Command unrecognized" << std::endl;
			}
		}
		// two params
		if (argc == 3) {
			const std::string action(argv[1]); // main param set, print
			const std::string param(argv[2]); // additional param cap, capture, rec, recording, mic, ren, render, play, playback / [device]
			// print
			if (std::any_of(COMMANDS_PRINT.begin(), COMMANDS_PRINT.end(), WordMatch(action))) {
				// print ren/render/play/playback
				if (std::any_of(PARAMS_PLAYBACK.begin(), PARAMS_PLAYBACK.end(), WordMatch(param))) {
					GetDeviceNames(eRender);
					PrintEndpointNames(eRender);
					return;
				}
				// print cap/capture/rec/recording/mic 
				if(std::any_of(PARAMS_RECORDING.begin(), PARAMS_RECORDING.end(), WordMatch(param))) {
					GetDeviceNames(eCapture);
					PrintEndpointNames(eCapture);
					return;
				}
			}
			// set [n]
			// check if user provided [n] matches any device name in the console_utilities internal list;
			if (std::any_of(COMMANDS_CHANGE.begin(), COMMANDS_CHANGE.end(), WordMatch(action))) {
				// target device, search all render devices for id;
				Devices devices;
				//const wchar_t ch_dev_id = param.at(0);
				const std::wstring wstr_param = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(param);
				// first we check if such device is in rendering devices list
				if (std::any_of(vec_render_dev_name_.begin(), vec_render_dev_name_.end(), CharIdMatch(wstr_param))) {
					devices.SetDefaultDevice(std::stoi(wstr_param), eCommunications);
					return;
				}
				// if it's not in rendering devices, check capture devices
				if (std::any_of(vec_capture_dev_name_.begin(), vec_capture_dev_name_.end(), CharIdMatch(wstr_param))) {
					devices.SetDefaultDevice(std::stoi(wstr_param), eCommunications);
				} else {
					std::wcout << "No such device on the list. Make sure you input the correct number." << std::endl;
				}
				// target device, search all capture devices for id;
			}
		}
		
	}

	// Prints two columns of endpoints.
	void ConsoleController::PrintEndpointsColumn() const {
		size_t longest_name_len = 0;
		for (const auto& i : vec_render_dev_name_) {
			if (longest_name_len < i.length()) longest_name_len = i.length();
		}

		const auto num_render_dev = vec_render_dev_name_.size();
		const auto num_capture_dev = vec_capture_dev_name_.size();
		const auto more_render = vec_capture_dev_name_.size() < vec_render_dev_name_.size();
		std::wcout << STR_RENDER_DEV << std::setw(longest_name_len + SPACING + 1) << STR_CAPTURE_DEV << std::endl;
		for (unsigned int i = 0; i < (more_render ? num_render_dev : num_capture_dev); i++) {
			if (i < num_render_dev) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left  << vec_render_dev_name_.at(i);
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices on the right
			// are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << vec_capture_dev_name_.at(i) << std::endl;
			}
		}
	}

	// Prints a list of endpoints
	void ConsoleController::PrintEndpointNames(EDataFlow dir) const {

		dir == eRender ? (std::wcout << STR_RENDER_DEV << std::endl) : (std::wcout << STR_CAPTURE_DEV << std::endl);
		auto dev_list = dir == eRender ? vec_render_dev_name_ : vec_capture_dev_name_;
		for (const auto& i : dev_list) {
			std::wcout << i << std::endl;
		}
		fflush(stdout);
	}
}
