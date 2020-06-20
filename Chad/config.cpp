#include "pch.h"
#include "config.h"
#include <filesystem>


std::filesystem::path Config::GetExecutablePath() const {
    wchar_t this_process_path[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, this_process_path, MAX_PATH);
    return this_process_path;
}

Config::Config() : path_(GetExecutablePath().remove_filename().wstring()) {
    filepath_ = path_ + filepath_;
}

int Config::Load() {
    constexpr auto buf_size = 320;
    WCHAR ren_name_buffer[buf_size];
    WCHAR cap_name_buffer[buf_size];

    const UINT num_char_ren = GetPrivateProfileString(L"Default devices", L"playback device", nullptr, ren_name_buffer, buf_size, filepath_.c_str());
    const UINT num_char_cap = GetPrivateProfileString(L"Default devices", L"capture device", nullptr, cap_name_buffer, buf_size, filepath_.c_str());
    const std::wstring def_ren_from_file(ren_name_buffer, num_char_ren);
    const std::wstring def_cap_from_file(cap_name_buffer, num_char_cap);
	default_ren_name_ = def_ren_from_file;
    default_cap_name_ = def_cap_from_file;

    if(num_char_ren > 0 || num_char_cap > 0) {
        return 1;
    }
    return 0;
}


int Config::SaveDefaultRender(std::wstring &device_name) const {
    const bool res = WritePrivateProfileString(L"Default devices", L"playback device", device_name.c_str(), filepath_.c_str());
    return res;
}


int Config::SaveDefaultCapture(std::wstring& device_name) const {
    const bool res = WritePrivateProfileString(L"Default devices", L"capture device", device_name.c_str(), filepath_.c_str());
    return res;
}
