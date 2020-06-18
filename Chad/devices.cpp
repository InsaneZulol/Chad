#include "pch.h"
#include "devices.h"
#include <functiondiscoverykeys.h>



Devices::Devices() {
	Update(eRender);
	Update(eCapture);
}

void Devices::Update(const EDataFlow flow_direction) {
	ThrowOnFail(ptr_enumerator_.CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL));

	ThrowOnFail(ptr_enumerator_->EnumAudioEndpoints(
		flow_direction, DEVICE_STATE_ACTIVE,
		&ptr_collection_));

	UINT num_endpoints = 0;
	ThrowOnFail(ptr_collection_->GetCount(&num_endpoints));
	if (num_endpoints == 0) {
		throw std::runtime_error("No endpoints found");
	}

	
	for (UINT i = 0; i < num_endpoints; i++) {
		// Get pointer to endpoint[i].
		ThrowOnFail(ptr_collection_->Item(i, &ptr_endpoint_));
		// Get pointer to endpoint[i] ID string.
		ThrowOnFail(ptr_endpoint_->GetId(&ptr_ep_id_.m_pData));
		ThrowOnFail(ptr_endpoint_->OpenPropertyStore(
			STGM_READ, &ptr_property_store_));

		// Initialize container for property value.
		PROPVARIANT var_name;
		PropVariantInit(&var_name);

		// Get the endpoint's friendly-name property.
		ThrowOnFail(ptr_property_store_->GetValue(
			PKEY_Device_FriendlyName, &var_name));

		const UINT num_id = ren_endpoints_.size() + cap_endpoints_.size();
		// now put required data into endpoint list
		if (flow_direction == eRender) {
			ren_endpoints_.push_back(Endpoint{ ptr_ep_id_.m_pData, var_name.pwszVal, num_id });
		} else {
			cap_endpoints_.push_back(Endpoint{ ptr_ep_id_.m_pData, var_name.pwszVal, num_id });
		}
		PropVariantClear(&var_name);
		ptr_endpoint_.Release();
		ptr_property_store_.Release();
	}
	ptr_collection_.Release();
	ptr_enumerator_.Release();
}

void Devices::SetDefaultDevice(const unsigned int num_id, ::ERole role) {
	std::wstring wsz_device_id;
	for(auto const &i: ren_endpoints_) {
		if (i.num_id == num_id) wsz_device_id = i.device_id;
	}
	if (wsz_device_id.empty()) {
		for (auto const& i : cap_endpoints_) {
			if (i.num_id == num_id) wsz_device_id = i.device_id;
		}
	};
	try {
		ThrowOnFail(ptr_policy_config.CoCreateInstance(CLSID_PolicyConfig));
		
		ThrowOnFail(ptr_policy_config->SetDefaultEndpoint(wsz_device_id.c_str(), eMultimedia));
		ThrowOnFail(ptr_policy_config->SetDefaultEndpoint(wsz_device_id.c_str(), eCommunications));
		
	}catch(_com_error& err) {
		std::wcout << "COM Error occured while setting default device" << std::endl;
		throw;
	}
}