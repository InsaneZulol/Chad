#include "devices.h"
#include "pch.h"


Devices::Devices() {
	std::wcout << "Devices created()" << std::endl;

	ThrowOnFail(ptr_enumerator_.CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL));

	ThrowOnFail(ptr_enumerator_->EnumAudioEndpoints(
		eRender, DEVICE_STATE_ACTIVE,
		&ptr_collection_));

	UINT num_devices = 0;
	ThrowOnFail(ptr_collection_->GetCount(&num_devices));
	if (num_devices == 0) {
		std::wcout << "No endpoints found." << std::endl;
	}

	for (UINT i = 0; i < num_devices; i++) {
		// Get pointer to endpoint number i.
		ThrowOnFail(ptr_collection_->Item(i, &ptr_endpoint_));
		// Get pointer to endpoint ID string.
		ThrowOnFail(ptr_endpoint_->GetId(&ptr_ep_id_.m_pData));
		ThrowOnFail(ptr_endpoint_->OpenPropertyStore(
			STGM_READ, &ptr_property_store_));

		// Initialize container for property value.
		PROPVARIANT var_name;
		PropVariantInit(&var_name);

		// Get the endpoint's friendly-name property.
		ThrowOnFail(ptr_property_store_->GetValue(
			PKEY_Device_FriendlyName, &var_name));

		ptr_endpoint_.Release(); // non-safe relase?
		ptr_property_store_.Release(); // non-safe relase?
		PropVariantClear(&var_name);
	}
}

void Devices::Update() {

}

void Devices::SetDefaultRender() {}
