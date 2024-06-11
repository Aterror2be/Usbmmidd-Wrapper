#ifndef USB_MMIDD_HPP
#define USB_MMIDD_HPP

#include <mutex>
#include <string>

class UsbMmidd
{
public:
	static UsbMmidd* GetInstance();

	bool Install();
	bool Uninstall();
	bool SetupResolution(int width, int height);
	bool ChangeResolution(int width, int heigth, std::string* device_name);
	bool GetVirtualDisplayPosition(int* x, int* y);
	bool EnableVirtualDisplay();
	bool DisableVirtualDisplay();

private:
	UsbMmidd() {};

	// Prevent copy construction and assignment
	UsbMmidd(const UsbMmidd&) = delete;
	UsbMmidd& operator=(const UsbMmidd&) = delete;

	// Prevent move operations
	UsbMmidd(UsbMmidd&&) = delete;
	UsbMmidd& operator=(UsbMmidd&&) = delete;

	static void Cleanup();

	std::string device_name;

	static UsbMmidd* instance;
	static std::mutex mtx;
};

#endif // !USB_MMIDD_HPP