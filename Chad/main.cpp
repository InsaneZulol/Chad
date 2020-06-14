// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <mmdeviceapi.h>
#include <comdef.h>  // _com_error
#include <functiondiscoverykeys.h> // prop_key
#include <system_error>
#include <fcntl.h>  // setmode
#include <io.h> // setmode
#include <atlbase.h> //CComPtr
#include "ipolicyconfig.h"
#include "devices.h"


void SetDefaultDevice(__in PCWSTR wszDeviceId, __in::ERole Role) {

	const IID CLSID_PolicyConfig = __uuidof(CPolicyConfigClient);
	const IID IID_IPolicyConfig = __uuidof(IPolicyConfig);
	CComPtr<IPolicyConfig> ptr_PolicyConfig;
	HRESULT Result = ptr_PolicyConfig.CoCreateInstance(CLSID_PolicyConfig);
	if (S_OK != Result)
		std::wcout<<"Error occured while creating CoCreateInstance CPolicyConfig" << std::endl;

	Result = ptr_PolicyConfig->SetDefaultEndpoint(wszDeviceId, Role);
	if (S_OK != Result)
		std::wcout << "Error ptr_PolicyConfig->SetDefaultEndpoint" << std::endl;
}

void PrintEndpointNames() {
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	CComPtr<IMMDeviceEnumerator> ptr_enumerator = nullptr;
	CComPtr<IMMDeviceCollection> ptr_collection;
	CComPtr<IMMDevice> ptr_endpoint;
	CComPtr<IPropertyStore> ptr_property_store;
	LPWSTR ptr_ep_id = nullptr;
	try
	{

		ThrowOnFail(ptr_enumerator.CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL));

		ThrowOnFail(ptr_enumerator->EnumAudioEndpoints(
			eRender, DEVICE_STATE_ACTIVE,
			&ptr_collection));

		UINT num_devices = 0;
		ThrowOnFail(ptr_collection->GetCount(&num_devices));
		if (num_devices == 0) {
			std::wcout<<"No endpoints found." << std::endl;
		}

		for (UINT i = 0; i < num_devices; i++) {
			// Get pointer to endpoint number i.
			ThrowOnFail(ptr_collection->Item(i, &ptr_endpoint));
			// Get pointer to endpoint ID string.
			ThrowOnFail(ptr_endpoint->GetId(&ptr_ep_id));
			ThrowOnFail(ptr_endpoint->OpenPropertyStore(
					STGM_READ, &ptr_property_store));

			// Initialize container for property value.
			PROPVARIANT var_name;
			PropVariantInit(&var_name);

			// Get the endpoint's friendly-name property.
			ThrowOnFail(ptr_property_store->GetValue(
				PKEY_Device_FriendlyName, &var_name));

			std::wcout << "Endpoint " << i << ": " << ptr_ep_id << " name: " << var_name.pwszVal << std::endl;
			std::wstring xdevid(ptr_ep_id);
			if (xdevid == L"{0.0.0.00000000}.{20ac9196-e822-4053-88a7-8379c2836e1b}") {
				std::wcout << "checkut!";
				SetDefaultDevice(ptr_ep_id, eMultimedia);
			};
			ptr_endpoint.Release(); // non-safe relase?
			ptr_property_store.Release(); // non-safe relase?
			ptr_ep_id = nullptr;
			CoTaskMemFree(ptr_ep_id);
			PropVariantClear(&var_name);
		}
	}
	catch (_com_error err)
	{
		std::wcout << "Exception occured: " << err.ErrorMessage() << std::endl; // a little more todo here
		// Handle error.
	}
}

void nPrintEndpointNames(const std::vector<std::string>& input) {
	// for_each(auto i& : Collection.rendering_devices()) {
	//		std::wcout << i;
	// }
	// fflush(stdout);
}

int main()
{
	fflush(stdout);
	SetConsoleOutputCP(CP_UTF8);
	_setmode(_fileno(stdout), _O_U8TEXT); // from now on remember to either: a) only use wcout b) setmode to O_TEXT before couting
	// initialize COM
	try {
		ThrowOnFail(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE));

		// PrintEndpointNames();
		Devices devices;
		
	}
	catch(_com_error err) {
		std::wcout << "Exception occured while initializing com: " << err.ErrorMessage() << std::endl; // a little more todo here
	}
	CoUninitialize();
	return 0;
}
