#pragma once
#include "pch.h"

#include <mmdeviceapi.h>
#include "endpoint.h"
#include "devices.h"

namespace util {
	
	class ConsoleController {
	public:
		ConsoleController(Devices& devices);
		static int SetOutputMode(int code_page, int translation_mode);
		void HandleInput(int argc, char* argv[]) const;
		void PrintEndpointsColumn(const std::vector<Endpoint>& ren_col, const std::vector<Endpoint>& cap_col) const;
		void PrintEndpointNames(EDataFlow dir, const std::vector<Endpoint>& dev_col) const;
	private:
		// Checks the config file for default device names
		// and attempts to set default devices accordingly.
		void LoadDefaults() const;
		void HandleAction(const std::string& action) const;
		void HandleActionWParam(const std::string& action, const std::string& param) const;
		void PrintOnChangedEndpoint(const std::wstring& device_name) const;
		typedef std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<Endpoint>>> VepIterator;
		typedef std::_Vector_const_iterator<std::_Vector_val<std::_Simple_types<std::string>>> VsIterator;
		VepIterator rb_; // Iterator of ren_endpoints_ vector
		VepIterator re_; // Iterator of ren_endpoints_ vector
		VepIterator cb_; // Iterator of cap_endpoints_ vector
		VepIterator ce_; // Iterator of cap_endpoints_ vector
		VsIterator apb_; // iterator of actions_print_ vector
		VsIterator ape__; // iterator of actions_print_ vector
		VsIterator acb_; // iterator of actions_change_ vector
		VsIterator ace_; // iterator of actions_change_ vector
		VsIterator ppb_; // iterator of params_playback_ vector
		VsIterator ppe_; // iterator of params_playback_ vector
		VsIterator prb_; // iterator of params_recording_ vector
		VsIterator pre_; // iterator of params_recording_ vector

		std::vector<std::string> actions_print_{ "list", "li", "print", "l" };
		std::vector<std::string> actions_change_{ "set", "change", "select", "sel" };
		std::vector<std::string> params_playback_{ "ren", "render", "play", "playback" };
		std::vector<std::string> params_recording_{ "cap", "capture", "rec", "recording", "mic" };
		
		Devices& devices_;
	};

}
