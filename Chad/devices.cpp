#include "pch.h"
#include "devices.h"


Devices::Devices() {
	Update(eRender);
	Update(eCapture);
	//Update(eAll); todo
}

void Devices::Update(const EDataFlow flow_direction) {
	ThrowOnFail(ptr_enumerator_.CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL));

	ThrowOnFail(ptr_enumerator_->EnumAudioEndpoints(
		flow_direction, DEVICE_STATE_ACTIVE,
		&ptr_collection_));

	UINT num_endpoints = 0;
	ThrowOnFail(ptr_collection_->GetCount(&num_endpoints));
	if (num_endpoints == 0) {
		std::wcout << "No endpoints found." << std::endl;
		// throw ch_error("No endpoints found.");
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

		const UINT num_id = render_list_.size() + capture_list_.size();
		// now put required data into endpoint list
		if (flow_direction == eRender) {
			render_list_.push_back(Endpoint{ ptr_ep_id_.m_pData, var_name.pwszVal, num_id });
		} else {
			capture_list_.push_back(Endpoint{ ptr_ep_id_.m_pData, var_name.pwszVal, num_id });
		}
		PropVariantClear(&var_name);
		ptr_endpoint_.Release();
		ptr_property_store_.Release();
	}
	ptr_collection_.Release();
	ptr_enumerator_.Release();
}

void Devices::SetDefaultRender() {}
