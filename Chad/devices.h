#pragma once

#include <vector>
#include <string>

struct Endpoint {
	std::wstring device_id;
	std::wstring device_name;
};

class Devices {
	void update();
	std::vector<Endpoint> list;
};