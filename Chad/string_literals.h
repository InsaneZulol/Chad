#pragma once
#include "pch.h"
namespace util {
	constexpr auto SPACING = 6;
	constexpr auto WELCOME_TEXT = L"Welcome in Chad. Start program with 'help' param to list available commands.";
	constexpr auto WELCOME_WHELP_TEXT = L"Welcome in Chad. Available commands:";
	constexpr auto REPEAT_SPEC_TEXT = L"Please repeat the command and specify the device";
	constexpr auto UNREC_COMMAND_TEXT = L"Unrecognized command: ";
	constexpr auto NO_SUCH_DEVICE_TEXT = L"No such device on the list. Make sure you input the correct number.";
	constexpr auto AVAILABLE_COMMANDS_PRINT_TEXT = L"list, li, l, print  - to list available audio devices";
	constexpr auto AVAILABLE_COMMANDS_CHANGE_TEXT = L"set, change, select sel - to set audio device [num] as default system one";
	constexpr auto AVAILABLE_PARAMS_PLAYBACK_TEXT = L"ren, render, play, playback";
	constexpr auto AVAILABLE_PARAMS_RECORDING_TEXT = L"cap, capture, rec, recording, mic";
	constexpr auto STR_RENDER_DEV = L"Playback devices: ";
	constexpr auto STR_CAPTURE_DEV = L"Recording devices: ";
}