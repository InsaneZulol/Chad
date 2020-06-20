#include "pch.h"
#include "console_controller.h"
#include <codecvt>
#include "devices.h"
#include <iomanip>
#include <utility>
#include <fcntl.h> // setmode
#include <io.h> // setmode

#include "config.h"
#include "string_literals.h"

namespace util {

	std::wstring Utf8Decode(const std::string& str) {
		if (str.empty()) return std::wstring();
		const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), nullptr, 0);
		std::wstring wstr_to(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &wstr_to[0], size_needed);
		return wstr_to;
	}

	// predicate; compares the whole wstring, returns bool
	struct WordMatch {
		const std::string s_;
		WordMatch(std::string s) : s_(std::move(s)) {}
		bool operator()(std::string const& s) const { return s == s_; }
	};

	// predicate; compares the first character of a wstring, returns bool
	struct CharIdMatch {
		const std::wstring s_;
		CharIdMatch(std::wstring s) : s_(std::move(s)) {}
		bool operator()(const std::wstring& s) const { return s[0] == s_[0]; }
	};
	// predicate; compares Endpoint num_id value, returns bool
	struct EndpointIntIdMatch {
		const unsigned int num_id_;

		EndpointIntIdMatch(const unsigned int num_id) : num_id_(num_id){}
		bool operator()(const Endpoint& ep) const { return num_id_ == ep.num_id; }
	};

	void ShowHelp() {
		std::wcout << WELCOME_WHELP_TEXT << std::endl
			<< AVAILABLE_COMMANDS_PRINT_TEXT << std::endl
			<< AVAILABLE_COMMANDS_CHANGE_TEXT << std::endl
			<< AVAILABLE_COMMANDS_SAVE_TEXT << std::endl
			<< AVAILABLE_COMMANDS_RESET_TEXT << std::endl;
	}
	
	ConsoleController::ConsoleController(Devices& devices) : devices_(devices) {
		SetConsoleOutputCP(CP_UTF8);
		_setmode(_fileno(stdout), _O_U8TEXT);
		// from now on remember to either:
		// a) only use wcout 
		// b) setmode to O_TEXT before couting

		rb_ = devices.ren_endpoints_.begin(); // Iterator of ren_endpoints_ vector
		re_ = devices.ren_endpoints_.end(); // Iterator of ren_endpoints_ vector
		cb_ = devices.cap_endpoints_.begin(); // Iterator of cap_endpoints_ vector
		ce_ = devices.cap_endpoints_.end(); // Iterator of cap_endpoints_ vector
		apb_ = actions_print_.begin(); // iterator of actions_print_ vector
		ape__ = actions_print_.end(); // iterator of actions_print_ vector
		acb_ = actions_change_.begin(); // iterator of actions_change_ vector
		ace_ = actions_change_.end(); // iterator of actions_change_ vector
		ppb_ = params_playback_.begin(); // iterator of params_playback_ vector
		ppe_ = params_playback_.end(); // iterator of params_playback_ vector
		prb_ = params_recording_.begin(); // iterator of params_recording_ vector
		pre_ = params_recording_.end(); // iterator of params_recording_ vector
	}


	// Sets console output translation mode
	// use _O_U8TEXT for Unicode, O_TEXT for narrow charset
	// Returns _setmode() success code
	int ConsoleController::SetOutputMode(const int code_page, const int translation_mode) {
		fflush(stdout);
		SetConsoleOutputCP(code_page);
		return  _setmode(_fileno(stdout), translation_mode);
	}

	void ConsoleController::LoadDefaults() const {
		Config cfg;

		if (cfg.Load()) {
			unsigned int ren_num_id = 999;
			if (std::any_of(rb_, re_, [&ren_num_id, cfg](const Endpoint& ep) {
				ren_num_id = ep.num_id;
				return cfg.default_ren_name_ == ep.device_name;
				})) {
				devices_.SetDefaultDevice(ren_num_id, ::eMultimedia);
				PrintOnChangedEndpoint(cfg.default_ren_name_);
			}
			unsigned int cap_num_id = 998;
			if (std::any_of(cb_, ce_, [&cap_num_id, cfg](const Endpoint& ep) {
				cap_num_id = ep.num_id;
				return cfg.default_cap_name_ == ep.device_name;
				})) {
				devices_.SetDefaultDevice(cap_num_id, ::eMultimedia);
				PrintOnChangedEndpoint(cfg.default_cap_name_);
			}
		}
	}

	
	void ConsoleController::HandleAction(const std::string& action) const {
		if (action == "help") {
			ShowHelp();
			return;
		}
		if (action == "reset") {
			LoadDefaults();
			return;
		}
		if ( action == "savedef") {
			std::wcout << REPEAT_SPEC_TEXT << std::endl;
		}
		// action is list, li, l, print
		if (std::any_of(apb_, ape__, WordMatch(action))) {
			PrintEndpointsColumn(devices_.ren_endpoints_, devices_.cap_endpoints_);
			return;
		}
		// action is set, change, select, sel
		if (std::any_of(acb_, ace_, WordMatch(action))) {
			std::wcout << REPEAT_SPEC_TEXT << std::endl;
		}
		else {
			std::wcout << UNREC_COMMAND_TEXT << std::endl;
		}
	}

	void ConsoleController::HandleActionWParam(const std::string& action, const std::string& param) const {
		// action is list, li, l, print
		if (std::any_of(apb_, ape__, WordMatch(action))) {
			// param is ren/render/play/playback
			if (std::any_of(ppb_, ppe_, WordMatch(param))) {
				PrintEndpointNames(eRender, devices_.ren_endpoints_);
				return;
			}
			// param is cap/capture/rec/recording/mic 
			if (std::any_of(prb_, pre_, WordMatch(param))) {
				PrintEndpointNames(eCapture, devices_.cap_endpoints_);
				return;
			}
			// param is unknown
			std::wcout << UNREC_COMMAND_TEXT
			<< Utf8Decode(action) << L" "
			<< Utf8Decode(param) << std::endl;
			return;
		}
		// action is set param is [n]
		if (std::any_of(acb_, ace_, WordMatch(action))) {
			// search all render devices for id;
			const auto int_param = std::stoi(param);
			std::wstring device_name;
			if (std::any_of(rb_, re_, [int_param, &device_name](const Endpoint& ep) {
				device_name = ep.device_name;
				return ep.num_id == int_param;
			})) {
				devices_.SetDefaultDevice(int_param, eCommunications);
				PrintOnChangedEndpoint(device_name);
				return;
			}
			if (std::any_of(cb_, ce_, [int_param, &device_name](const Endpoint& ep) {
				device_name = ep.device_name;
				return ep.num_id == int_param;
			})) {
				devices_.SetDefaultDevice(int_param, eCommunications);
				PrintOnChangedEndpoint(device_name);
			} else {
				std::wcout << NO_SUCH_DEVICE_TEXT << std::endl;
			}
		}
		// action savedefault, param is [n]
		if (action == "savedef") {
			const Config cfg;
			const auto int_param = std::stoi(param);
			std::wstring device_name;
			if (std::any_of(rb_, re_, [int_param, &device_name](const Endpoint& ep) {
				device_name = ep.device_name;
				return ep.num_id == int_param;
				})) {
				cfg.SaveDefaultRender(device_name);
				std::wcout << device_name << STR_SAVED_REN_DEV << std::endl;
				
			}
			if (std::any_of(cb_, ce_, [int_param, &device_name](const Endpoint& ep) {
				device_name = ep.device_name;
				return ep.num_id == int_param;
				})) {
				cfg.SaveDefaultCapture(device_name);
				std::wcout << device_name << STR_SAVED_CAP_DEV << std::endl;
			} else {
				std::wcout << NO_SUCH_DEVICE_TEXT << std::endl;
			}
		}
		else {
			std::wcout << UNREC_COMMAND_TEXT
				<< Utf8Decode(action) << L" "
				<< Utf8Decode(param) << std::endl;
		}
	}

	void ConsoleController::HandleInput(const int argc, char* argv[]) const {
		// if program was started without params:
		if (argc <= 1) {
			std::wcout << WELCOME_TEXT << std::endl;
		}
		// one param:
		if (argc == 2) {
			const std::string action(argv[1]);
			HandleAction(action);
		}
		// two params
		if (argc == 3) {
			const std::string action(argv[1]); // main param: set, print
			const std::string param(argv[2]); // additional param: cap, capture, rec, recording, mic, ren, render, play, playback / [device]
			HandleActionWParam(action, param);
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
				std::wcout << std::left << std::to_wstring(ren_col.at(i).num_id) << L": " << std::setw(spacing-3) << ren_col.at(i).device_name;
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices on the right
			// are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(spacing) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << std::to_wstring(cap_col.at(i).num_id) << L": " << cap_col.at(i).device_name << std::endl;
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
			auto line = std::to_wstring(i.num_id) + L": " + i.device_name;
			std::wcout << line << std::endl;
		}
		fflush(stdout);
	}
	
	void ConsoleController::PrintOnChangedEndpoint(const std::wstring& device_name) const {
		std::wcout << L"[] Default device has been changed to: " << device_name << std::endl;
	}
}
