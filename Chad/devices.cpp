#include "devices.h"
#include "pch.h"


Devices::Devices() {
	ThrowOnFail(ptr_enumerator_.CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL));

	ThrowOnFail(ptr_enumerator_->EnumAudioEndpoints(
		eRender, DEVICE_STATE_ACTIVE,
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

		// now put all data into endpoint list
		list_.push_back(Endpoint{ ptr_ep_id_.m_pData, var_name.pwszVal,  });

		PropVariantClear(&var_name);
		ptr_endpoint_.Release(); // non-safe relase?
		ptr_property_store_.Release(); // non-safe relase?
	}
}

void Devices::Update() {

}

void Devices::SetDefaultRender() {}
