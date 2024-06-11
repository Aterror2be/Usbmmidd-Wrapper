#ifndef USB_MMIDD_DATA_HPP
#define USB_MMIDD_DATA_HPP

#include <mutex>
#include <string>

class UsbMmiddData
{
public:
	static UsbMmiddData* GetInstance();

	bool InstallUsbMmidd();
	bool UninstallUsbMmidd();
	std::string GetPath();

private:
	UsbMmiddData() {};

	unsigned char* GetCabinetBytes();
	unsigned int GetCabinetSize();

	// Prevent copy construction and assignment
	UsbMmiddData(const UsbMmiddData&) = delete;
	UsbMmiddData& operator=(const UsbMmiddData&) = delete;

	// Prevent move operations
	UsbMmiddData(UsbMmiddData&&) = delete;
	UsbMmiddData& operator=(UsbMmiddData&&) = delete;

	static unsigned int __stdcall ExtractCallback(void* context, unsigned int notification, unsigned long long param1, unsigned long long param2);
	static void Cleanup();

	static UsbMmiddData* instance;
	static std::mutex mtx;

	static unsigned char usbmmidd_v2_data[150754];
};

#endif // !USB_MMIDD_DATA_HPP