#pragma once

#include "pch.h"
#include <mmdeviceapi.h>
#include <atlbase.h> // CComPtr
#include <comdef.h> // _com_error
#include "endpoint.h"
#include "ipolicyconfig.h"


inline void ThrowOnFail(const HRESULT hr) {
	if (FAILED(hr)) {
		throw _com_error(hr);
	}
}

class Devices {
public:
	Devices();
	void Update(EDataFlow);
	void SetDefaultDevice(unsigned int num_id, ::ERole role);
private:
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	const CLSID CLSID_PolicyConfig = __uuidof(CPolicyConfigClient);
 	const IID IID_IPolicyConfig = __uuidof(IPolicyConfig);
	
	CComPtr<IPolicyConfig> ptr_policy_config = nullptr;
	CComPtr<IMMDeviceEnumerator> ptr_enumerator_ = nullptr;
	CComPtr<IMMDeviceCollection> ptr_collection_;
	CComPtr<IMMDevice> ptr_endpoint_;
	CComPtr<IPropertyStore> ptr_property_store_;
	CComHeapPtr<WCHAR> ptr_ep_id_;
public:
	std::vector<Endpoint> ren_endpoints_;
	std::vector<Endpoint> cap_endpoints_;
};