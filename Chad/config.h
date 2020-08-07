#pragma once
#include "pch.h"
#include <fstream>

class Config {
public:
	Config();
	int Load();
	// If the function successfully copies the string to the initialization file, the return value is nonzero.
	// If the function fails, or if it flushes the cached version of the most recently accessed initialization
	// file, the return value is zero. To get extended error information, call GetLastError.
	int SaveDefaultRender(std::wstring& device_name) const;
	// If the function successfully copies the string to the initialization file, the return value is nonzero.
	// If the function fails, or if it flushes the cached version of the most recently accessed initialization
	// file, the return value is zero. To get extended error information, call GetLastError.
	int SaveDefaultCapture(std::wstring& device_name) const;
private:
	std::filesystem::path GetExecutablePath() const;
	std::wstring path_ = L"";
	std::wstring filepath_ = L"config.ini";
public:
	std::wstring default_cap_name_; // Full name of default recording endpoint
	std::wstring default_ren_name_; // Full name of default playback endpoint
};

