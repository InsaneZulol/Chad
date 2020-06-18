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

	// compares the whole wstring, returns bool
	struct WordMatch {
		const std::string s_;
		WordMatch(std::string s) : s_(std::move(s)) {}
		bool operator()(std::string const& s) const { return s == s_; }
	};

	// compares the first character of a wstring, returns bool
	struct CharIdMatch {
		const std::wstring s_;
		CharIdMatch(std::wstring s) : s_(std::move(s)) {}
		bool operator()(const std::wstring& s) const { return s[0] == s_[0]; }
	};
	// compares Endpoint num_id value, returns bool
	struct EndpointIntIdMatch {
		const unsigned int num_id_;

		EndpointIntIdMatch(const unsigned int num_id) : num_id_(num_id){}
		bool operator()(const Endpoint& ep) const { return num_id_ == ep.num_id; }
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
	}


	// Sets console output translation mode
	// use _O_U8TEXT for Unicode, O_TEXT for narrow charset
	// Returns _setmode() success code
	int ConsoleController::SetOutputMode(const int code_page, const int translation_mode) {
		fflush(stdout);
		SetConsoleOutputCP(code_page);
		return  _setmode(_fileno(stdout), translation_mode);
	}

	void ConsoleController::HandleInput(const int argc, char* argv[], Devices& devices) const {
		const auto rb = devices.ren_endpoints_.begin(); // Iterator of ren_endpoints_ vector
		const auto re = devices.ren_endpoints_.end(); // Iterator of ren_endpoints_ vector
		const auto cb = devices.cap_endpoints_.begin(); // Iterator of cap_endpoints_ vector
		const auto ce = devices.cap_endpoints_.end(); // Iterator of cap_endpoints_ vector
		const auto cmdpb = COMMANDS_PRINT.begin(); // iterator of COMMANDS_PRINT vector
		const auto cmdpe = COMMANDS_PRINT.end(); // iterator of COMMANDS_PRINT vector
		const auto cmdcb = COMMANDS_CHANGE.begin(); // iterator of COMMANDS_CHANGE vector
		const auto cmdce = COMMANDS_CHANGE.end(); // iterator of COMMANDS_CHANGE vector
		const auto ppb = PARAMS_PLAYBACK.begin(); // iterator of PARAMS_PLAYBACK vector
		const auto ppe = PARAMS_PLAYBACK.end(); // iterator of PARAMS_PLAYBACK vector
		const auto prb = PARAMS_RECORDING.begin(); // iterator of PARAMS_RECORDING vector
		const auto pre = PARAMS_RECORDING.end(); // iterator of PARAMS_RECORDING vector
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
			if (std::any_of(cmdpb, cmdpe, WordMatch(action))) {
				PrintEndpointsColumn(devices.ren_endpoints_, devices.cap_endpoints_);
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
					PrintEndpointNames(eRender, devices.ren_endpoints_);
					return;
				}
				// action is print param is cap/capture/rec/recording/mic 
				if (std::any_of(prb, pre, WordMatch(param))) {
					PrintEndpointNames(eCapture, devices.cap_endpoints_);
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
				// search all render devices for id;
				auto int_param = std::stoi(param);
				std::wstring device_name;
				if (std::any_of(rb, re, [int_param, &device_name](const Endpoint& ep) {
					device_name = ep.device_name;
					return ep.num_id == int_param;
				})) {
					devices.SetDefaultDevice(int_param, eCommunications);
					PrintOnChangedEndpoint(device_name);
					return;
				}
				if (std::any_of(cb, ce, [int_param, &device_name](const Endpoint& ep) {
					device_name = ep.device_name;
					return ep.num_id == int_param;
				})) {
					devices.SetDefaultDevice(int_param, eCommunications);
					PrintOnChangedEndpoint(device_name);
				}
			} else {
				std::wcout << NO_SUCH_DEVICE_TEXT << std::endl;
			}
		}
	}
		

	// Prints two columns of endpoints.
	void ConsoleController::PrintEndpointsColumn(const std::vector<Endpoint>& ren_col, const std::vector<Endpoint>& cap_col) const {
		size_t longest_name_len = 0;
		for (const auto& i : ren_col) {
			if (longest_name_len < i.device_name.length())
				longest_name_len = i.device_name.length();
		}

		const auto num_render_dev = ren_col.size();
		const auto num_capture_dev = cap_col.size();
		const auto is_more_render = cap_col.size() < ren_col.size();
		const auto dev_count = is_more_render ? num_render_dev : num_capture_dev;
		const auto spacing = longest_name_len + SPACING;
		
		std::wcout << STR_RENDER_DEV << std::setw(spacing + 1U) << STR_CAPTURE_DEV << std::endl;
		for (unsigned int i = 0; i < dev_count; i++) {
			if (i < num_render_dev) {
				std::wcout << std::setw(spacing) << std::left  << ren_col.at(i).device_name;
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices on the right
			// are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(spacing) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << cap_col.at(i).device_name << std::endl;
			}
		}
	}

	// Prints a list of endpoints
	void ConsoleController::PrintEndpointNames(const EDataFlow dir, const std::vector<Endpoint>& dev_col) const {

		if (dir == eRender)
			std::wcout << STR_RENDER_DEV << std::endl;
		else
			std::wcout << STR_CAPTURE_DEV << std::endl;
		for (const auto& i : dev_col) {
			auto line = i.num_id + L" " + i.device_name;
			std::wcout << line << std::endl;
		}
		fflush(stdout);
	}
	void ConsoleController::PrintOnChangedEndpoint(const std::wstring& device_name) const {
		std::wcout << L"[] Default device has been changed to: " << device_name << std::endl;
	}
}
