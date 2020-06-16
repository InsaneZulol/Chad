#pragma once
#include "pch.h"
namespace util {
	constexpr auto SPACING = 6;
	constexpr auto WELCOME_TEXT = "Welcome in Chad. Type help to list available commands.";
	constexpr auto WELCOME_WHELP_TEXT = "Welcome in Chad. Available commands:";
	constexpr auto AVAILABLE_COMMANDS_PRINT_TEXT = "list, li, l, print  - to list available audio devices";
	constexpr auto AVAILABLE_COMMANDS_CHANGE_TEXT = "set, change, select sel - to set audio device [num] as default system one";
	constexpr auto AVAILABLE_PARAMS_PLAYBACK_TEXT = "ren, render, play, playback";
	constexpr auto AVAILABLE_PARAMS_RECORDING_TEXT = "cap, capture, rec, recording, mic";
	constexpr auto STR_RENDER_DEV = L"Playback devices: ";
	constexpr auto STR_CAPTURE_DEV = L"Recording devices: ";
	std::vector<std::string> COMMANDS_PRINT{ "list", "li", "print", "l" };
	std::vector<std::string> COMMANDS_CHANGE{ "set", "change", "select", "sel" };
	std::vector<std::string> PARAMS_PLAYBACK{ "ren", "render", "play", "playback" };
	std::vector<std::string> PARAMS_RECORDING{ "cap", "capture", "rec", "recording", "mic" };
}