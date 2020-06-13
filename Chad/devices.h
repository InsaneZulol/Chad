#pragma once

#include <vector>
#include <string>
#include <mmdeviceapi.h>
#include <atlbase.h> //CComPtr
#include <comdef.h>
#include <functiondiscoverykeys.h>


inline void ThrowOnFail(const HRESULT hr) {
	if (FAILED(hr)) {
		throw _com_error(hr);
	}
}

struct Endpoint {
	std::wstring device_id;
	std::wstring device_name;
	// role needed?
};

class Devices {
public:
	Devices();
	void Update();
	void SetDefaultRender();
private:
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	CComPtr<IMMDeviceEnumerator> ptr_enumerator_ = nullptr;
	CComPtr<IMMDeviceCollection> ptr_collection_;
	CComPtr<IMMDevice> ptr_endpoint_;
	CComPtr<IPropertyStore> ptr_property_store_;
	CComHeapPtr<WCHAR> ptr_ep_id_;
	std::vector<Endpoint> list_;
};