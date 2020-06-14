#pragma once
#include "pch.h"


#include <fcntl.h> // setmode
#include <io.h> // setmode
#include "endpoint.h"
class ConsoleUtilities {

 public:
	ConsoleUtilities();
	void PrintEndpointNames(const std::vector<Endpoint>& render_devices, const std::vector<Endpoint>& recording_devices);
	void PrintEndpointNames(const std::vector<Endpoint>& input);
};

