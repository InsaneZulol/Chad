#pragma once

#include "pch.h"
#include <mmdeviceapi.h>
#include <atlbase.h> //CComPtr
#include <comdef.h>
#include <functiondiscoverykeys.h>
#include "endpoint.h"


inline void ThrowOnFail(const HRESULT hr) {
	if (FAILED(hr)) {
		throw _com_error(hr);
	}
}

class Devices {
public:
	Devices();
	void Update(EDataFlow);
	void SetDefaultDevice(__in PCWSTR wszDeviceId, __in::ERole Role);
private:
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	
	CComPtr<IMMDeviceEnumerator> ptr_enumerator_ = nullptr;
	CComPtr<IMMDeviceCollection> ptr_collection_;
	CComPtr<IMMDevice> ptr_endpoint_;
	CComPtr<IPropertyStore> ptr_property_store_;
	CComHeapPtr<WCHAR> ptr_ep_id_;
public:
	std::vector<Endpoint> render_list_;
	std::vector<Endpoint> capture_list_;
};