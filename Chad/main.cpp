// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mmdeviceapi.h>
#include <comdef.h>  // Declares _com_error
#include <functiondiscoverykeys.h> // prop_key
#include <system_error>
#include <fcntl.h>  
#include <io.h>  




inline void throw_if_fail(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw _com_error(hr);
	}
}

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

//https://docs.microsoft.com/en-us/windows/win32/learnwin32/error-handling-in-com

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

void PrintEndpointNames()
{
	HRESULT hr = S_OK;
	IMMDeviceEnumerator* ptr_enumerator = NULL;
	IMMDeviceCollection* ptr_collection = NULL;
	IMMDevice* ptr_endpoint = NULL;
	IPropertyStore* ptr_propertoryStore = NULL;
	LPWSTR pwszID = NULL;
	_com_error err(hr);
	LPCTSTR errMsg = err.ErrorMessage();

	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&ptr_enumerator);
	EXIT_ON_ERROR(hr)
		hr = ptr_enumerator->EnumAudioEndpoints(
			eRender, DEVICE_STATE_ACTIVE,
			&ptr_collection);
	EXIT_ON_ERROR(hr)

	UINT  count;
	hr = ptr_collection->GetCount(&count);
	EXIT_ON_ERROR(hr)

		if (count == 0)
		{
			printf("No endpoints found.\n");
		}

	// Each loop prints the name of an endpoint device.
	for (ULONG i = 0; i < count; i++)
	{
		// Get pointer to endpoint number i.
		hr = ptr_collection->Item(i, &ptr_endpoint);
		EXIT_ON_ERROR(hr)

		// Get the endpoint ID string.
		hr = ptr_endpoint->GetId(&pwszID);
		EXIT_ON_ERROR(hr)

		hr = ptr_endpoint->OpenPropertyStore(
				STGM_READ, &ptr_propertoryStore);
		EXIT_ON_ERROR(hr)

		PROPVARIANT varName;
		// Initialize container for property value.
		PropVariantInit(&varName);

		// Get the endpoint's friendly-name property.
		hr = ptr_propertoryStore->GetValue(
			PKEY_Device_FriendlyName, &varName);
		EXIT_ON_ERROR(hr)

		std::wcout << "Endpoint " << i << ": " << pwszID << "name: " << varName.pwszVal << std::endl;

		CoTaskMemFree(pwszID);
		pwszID = NULL;
		PropVariantClear(&varName);
		SAFE_RELEASE(ptr_propertoryStore)
		SAFE_RELEASE(ptr_endpoint)
	}
	SAFE_RELEASE(ptr_enumerator)
		SAFE_RELEASE(ptr_collection)
		return;

Exit:
	std::cout << "Error: " << std::endl;
	CoTaskMemFree(pwszID);
	SAFE_RELEASE(ptr_enumerator)
		SAFE_RELEASE(ptr_collection)
		SAFE_RELEASE(ptr_endpoint)
		SAFE_RELEASE(ptr_propertoryStore)
}

int main()
{
	fflush(stdout);
	SetConsoleOutputCP(CP_UTF8);
	_setmode(_fileno(stdout), _O_U8TEXT); // from now on remember to either: a) only use wcout b) setmode to O_TEXT before couting
	std::wcout << L"\n TEST: abc-¹bæ \n";
	// initialize COM - remember to cleanup
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);

	if (SUCCEEDED(hr)) {
		PrintEndpointNames();
	}
	else {
		std::wcout << "Error initializing com library\n";
	}
	return 0;

}
