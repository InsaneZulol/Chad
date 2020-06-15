#include "pch.h"
#include "console_utilities.h"
#include <iomanip>
#include <array>

namespace utilities {
	constexpr auto SPACING = 6;
	constexpr auto WELCOME_TEXT = "Welcome in Chad. Type help to list available commands.";
	constexpr auto WELCOME_WHELP_TEXT = "Welcome in Chad. Available commands:";
	constexpr auto AVAILABLE_COMMANDS_PRINT_TEXT = "list, li, l, print  - to list available audio devices";
	constexpr auto AVAILABLE_COMMANDS_CHANGE_TEXT = "set, change, select sel - to set selected audio device as default system one";
	constexpr auto AVAILABLE_PARAMS_PLAYBACK_TEXT = "ren, render, play, playback";
	constexpr auto AVAILABLE_PARAMS_RECORDING_TEXT = "cap, capture, rec, recording, mic";
	std::vector<std::string> VEC_COMMANDS_PRINT{"list", "li", "print", "l"};
	std::vector<std::string> VEC_COMMANDS_CHANGE{"set", "change", "select", "sel"};
	std::vector<std::string> VEC_PARAMS_PLAYBACK{"ren", "render", "play", "playback"};
	std::vector<std::string> VEC_PARAMS_RECORDING{"cap", "capture", "rec", "recording", "mic"};

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

		// clear vector to update
		if (!vec_render_dev_name_.empty()) vec_render_dev_name_.clear();
		if (!vec_capture_dev_name_.empty()) vec_capture_dev_name_.clear();
		
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
				if (devices.render_list_.size() > i) {
					std::wstring ren_dev_text = std::to_wstring(devices.render_list_.at(i).id_num) + L": " + devices.render_list_.at(i).device_name;
					vec_render_dev_name_.push_back(ren_dev_text);
				};
				if (devices.capture_list_.size() > i) {
					std::wstring cap_dev_text = std::to_wstring(devices.capture_list_.at(i).id_num) + L": " + devices.capture_list_.at(i).device_name;
					vec_capture_dev_name_.push_back(cap_dev_text);
				};
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
			if(std::any_of(VEC_COMMANDS_PRINT.begin(), VEC_COMMANDS_PRINT.end(), WordMatch(action))) {
				GetDeviceNames(eAll);
				PrintEndpointsColumn();
				return;
			}
			// set, change, select, sel
			if (std::any_of(VEC_COMMANDS_CHANGE.begin(),
				VEC_COMMANDS_CHANGE.end(), WordMatch(action))) {
				std::wcout << "Please repeat the command and specify the device" << std::endl;
			} else {
					std::wcout << "Command unrecognized" << std::endl;
			}
		}
		// two params
		if (argc == 3) {
			const std::string action(argv[1]);
			const std::string param(argv[2]); // additional param cap, capture, rec, recording, mic, ren, render, play, playback / [device]
			// print
			if (std::any_of(VEC_COMMANDS_PRINT.begin(), VEC_COMMANDS_PRINT.end(), WordMatch(action))) {
				// print ren/render/play/playback
				if (std::any_of(VEC_PARAMS_PLAYBACK.begin(), VEC_PARAMS_PLAYBACK.end(), WordMatch(param))) {
					PrintEndpointNames(eRender);
					return;
				}
				// print cap/capture/rec/recording/mic 
				if(std::any_of(VEC_PARAMS_RECORDING.begin(), VEC_PARAMS_RECORDING.end(), WordMatch(param))) {
					PrintEndpointNames(eCapture);
					return;
				}
			}
			// set
			if (std::any_of(VEC_COMMANDS_CHANGE.begin(), VEC_COMMANDS_CHANGE.end(), WordMatch(action))) {
				// target device
				if (std::any_of(VEC_PARAMS_PLAYBACK.begin(), VEC_PARAMS_PLAYBACK.end(), WordMatch(param))) {
					
				}
			}
		}
		
	}

	// Prints two columns of endpoints.
	// Remember to pass render devices as first parameter so it's displayed under correct label in console.
	void ConsoleUtilities::PrintEndpointsColumn() const {
		// Get Devices names;
		// save Devices names to own Vector of strings,
		// Print Own vector  of strings
		//
		//
		size_t longest_name_len = 0;
		for (const auto& i : vec_render_dev_name_) {
			if (longest_name_len < i.length()) longest_name_len = i.length();
		}
		const auto num_render_dev = vec_render_dev_name_.size();
		const auto num_capture_dev = vec_capture_dev_name_.size();
		const auto more_render = vec_capture_dev_name_.size() < vec_render_dev_name_.size();
		std::wcout << str_render_dev_ << std::setw(longest_name_len + SPACING + 1) << str_capture_dev_ << std::endl;
		for (auto i = 0; i < (more_render ? num_render_dev : num_capture_dev); i++) {
			if (i < num_render_dev) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left  << vec_render_dev_name_.at(i);
			}
			// if there are still capture devices to list, but no more render devices then
			// output an empty string, so recording devices are offset in correct position
			if ((i >= num_render_dev) && (i < num_capture_dev)) {
				std::wcout << std::setw(longest_name_len + SPACING) << std::left << L" ";
			}
			if (i < num_capture_dev) {
				std::wcout << std::left << vec_capture_dev_name_.at(i) << std::endl;
			}
		}
	}

	// Prints a list of endpoints
	void ConsoleUtilities::PrintEndpointNames(EDataFlow dir) const {

		dir == eRender ? (std::wcout << str_render_dev_ << std::endl) : (std::wcout << str_capture_dev_ << std::endl);
		auto dev_list = dir == eRender ? vec_render_dev_name_ : vec_capture_dev_name_;
		for (const auto& i : dev_list) {
			std::wcout << i << std::endl;
		}
		fflush(stdout);
	}
}
