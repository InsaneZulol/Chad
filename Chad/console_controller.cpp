#include "pch.h"
#include "console_controller.h"
#include <codecvt>
#include "devices.h"
#include <iomanip>
#include <utility>
#include <fcntl.h> // setmode
#include <io.h> // setmode
#include "string_literals.h"

namespace util {
	
	struct WordMatch {
		const std::string s_;
		WordMatch(std::string s) : s_(std::move(s)) {}
		bool operator()(std::string const& s) const { return s == s_; }
	};

	// compares the first character of a wstring, returns bool
	struct CharIdMatch {
		const std::wstring s_;
		CharIdMatch(std::wstring s) : s_(std::move(s)) {}
		bool operator()(const std::wstring s) const { return s[0] == s_[0]; }
	};

	void  ShowHelp() {
		std::wcout << WELCOME_WHELP_TEXT << std::endl
			<< AVAILABLE_COMMANDS_PRINT_TEXT << std::endl
			<< AVAILABLE_COMMANDS_CHANGE_TEXT << std::endl;
	}
	
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
		if (!ren_devs_.empty()) ren_devs_.clear();
		if (!cap_devs_.empty()) cap_devs_.clear();
		
		if(dir == eRender) {
			const auto dev_list = devices.ren_endpoints_;
			for (const auto& i : dev_list) {
				std::wstring ren_dev_text = std::to_wstring(i.num_id) + L": " + i.device_name;
				ren_devs_.push_back(ren_dev_text);
			}
			return ren_devs_.size();
		}
		if (dir == eCapture) {
			const auto dev_list = devices.cap_endpoints_;
			for (const auto& i : dev_list) {
				std::wstring cap_dev_text = std::to_wstring(i.num_id) + L": " + i.device_name;
				cap_devs_.push_back(cap_dev_text);
			}
			return cap_devs_.size();
		}
		if (dir == eAll) {
			const auto is_render_bigger = devices.ren_endpoints_.size() > devices.cap_endpoints_.size();
			const auto dev_list = is_render_bigger ? devices.ren_endpoints_ : devices.cap_endpoints_;
			for (auto i = 0; i < dev_list.size(); i++) {
				if (devices.ren_endpoints_.size() > i) {
					unsigned num_id = devices.ren_endpoints_.at(i).num_id;
					std::wstring device_name = devices.ren_endpoints_.at(i).device_name;;
					std::wstring ren_dev_text = std::to_wstring(num_id) + L": " + device_name;
					ren_devs_.push_back(ren_dev_text);
				};
				if (devices.cap_endpoints_.size() > i) {
					unsigned num_id = devices.cap_endpoints_.at(i).num_id;
					std::wstring device_name = devices.cap_endpoints_.at(i).device_name;
					std::wstring cap_dev_text = std::to_wstring(num_id) + L": " + devices.cap_endpoints_.at(i).device_name;
					cap_devs_.push_back(cap_dev_text);
				};
			}
			return ren_devs_.size() + cap_devs_.size();
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
		auto rb = ren_devs_.begin(); auto re = ren_devs_.end(); // Iterator of ren_devices vector
		auto cb = cap_devs_.begin(); auto ce = cap_devs_.end(); // Iterator of cap_devices vector
		auto cmdpb = COMMANDS_PRINT.begin(); auto cmdpe = COMMANDS_PRINT.end(); // iterator of COMMANDS_PRINT vector
		auto cmdcb = COMMANDS_CHANGE.begin(); auto cmdce = COMMANDS_CHANGE.end(); // iterator of COMMANDS_CHANGE vector
		auto ppb = PARAMS_PLAYBACK.begin(); auto ppe = PARAMS_PLAYBACK.end(); // iterator of PARAMS_PLAYBACK vector
		auto prb = PARAMS_RECORDING.begin(); auto pre = PARAMS_RECORDING.end(); // iterator of PARAMS_RECORDING vector
		// if program was started without params:
		if (argc <= 1) {
			std::wcout << WELCOME_TEXT << std::endl;
		}
		// one param:
		if (argc == 2) {
			const std::string action(argv[1]);
			if (action == "help") {
				ShowHelp();
				return;
			}
			// action is list, li, l, print
			if(std::any_of(cmdpb, cmdpe, WordMatch(action))) {
				GetDeviceNames(eAll);
				PrintEndpointsColumn();
				return;
			}
			// action is set, change, select, sel
			if (std::any_of(cmdcb, cmdce, WordMatch(action))) {
				std::wcout << REPEAT_SPEC_TEXT << std::endl;
			} else {
					std::wcout << UNREC_COMMAND_TEXT << std::endl;
			}
		}
		// two params
		//
		if (argc == 3) {
			const std::string action(argv[1]); // main param: set, print
			const std::string param(argv[2]); // additional param: cap, capture, rec, recording, mic, ren, render, play, playback / [device]
			// action is print
			if (std::any_of(cmdpb, cmdpe, WordMatch(action))) {
				// action is print param is ren/render/play/playback
				if (std::any_of(ppb, ppe, WordMatch(param))) {
					GetDeviceNames(eRender);
					PrintEndpointNames(eRender);
					return;
				}
				// action is print param is cap/capture/rec/recording/mic 
				if(std::any_of(prb, pre, WordMatch(param))) {
					GetDeviceNames(eCapture);
					PrintEndpointNames(eCapture);
				}
				// action is print param is unknown
				else {
					std::wcout << UNREC_COMMAND_TEXT
					<< std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(action) << L" "
					<< std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(param) << std::endl;
				}
			}
			// action is set param is [n]
			if (std::any_of(cmdcb, cmdce, WordMatch(action))) {
				// target device, search all render devices for id;
				Devices devices;
				const auto wstr_param = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(param);
				if (std::any_of(rb, re, CharIdMatch(wstr_param))) {
					devices.SetDefaultDevice(std::stoi(wstr_param), eCommunications);
					return;
				}
				if (std::any_of(cb, ce, CharIdMatch(wstr_param))) {
					devices.SetDefaultDevice(std::stoi(wstr_param), eCommunications);
				} else {
					std::wcout << NO_SUCH_DEVICE_TEXT << std::endl;
				}
			}
		}
		
	}

	// Prints two columns of endpoints.
	void ConsoleController::PrintEndpointsColumn() const {
		size_t longest_name_len = 0;
		for (const auto& i : ren_devs_) {
			if (longest_name_len < i.length()) longest_name_len = i.length();
		}

		const auto num_render_dev = ren_devs_.size();
		const auto num_capture_dev = cap_devs_.size();
		const auto more_render = cap_devs_.size() < ren_devs_.size();
		std::wcout << STR_RENDER_DEV << std::setw(longest_name_len + SPACING + 1) << STR_CAPTURE_DEV << std::endl;
		for (unsigned int i = 0; i < (more_render ? num_render_dev : num_capture_dev); i++) {
			if (i < num_render_dev) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left  << ren_devs_.at(i);
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices on the right
			// are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << cap_devs_.at(i) << std::endl;
			}
		}
	}

	// Prints a list of endpoints
	void ConsoleController::PrintEndpointNames(EDataFlow dir) const {

		dir == eRender ? (std::wcout << STR_RENDER_DEV << std::endl) : (std::wcout << STR_CAPTURE_DEV << std::endl);
		auto dev_list = dir == eRender ? ren_devs_ : cap_devs_;
		for (const auto& i : dev_list) {
			std::wcout << i << std::endl;
		}
		fflush(stdout);
	}
}
