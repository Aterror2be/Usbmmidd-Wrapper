#include "UsbMmidd.hpp"
#include "UsbMmiddData.hpp"

#include <Windows.h>
#include <processthreadsapi.h>
#include <format>

UsbMmidd* UsbMmidd::GetInstance()
{
	std::lock_guard<std::mutex> lock(mtx);
	if (instance == nullptr)
	{
		instance = new UsbMmidd();
		std::atexit(Cleanup);
	}

	return instance;
}

bool UsbMmidd::Install()
{
	if (!UsbMmiddData::GetInstance()->InstallUsbMmidd())
		return false;

	const std::string working_path = UsbMmiddData::GetInstance()->GetPath() + "\\";
	const std::string full_exe_path = working_path + "deviceinstaller64.exe";
	const std::string command_line = "\"" + full_exe_path + "\" install usbmmidd.inf usbmmidd";

	STARTUPINFOA startup_info;
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;
	ZeroMemory(&process_info, sizeof(process_info));

	if (CreateProcessA(nullptr, const_cast<char*>(command_line.c_str()), nullptr, nullptr, FALSE, 0, nullptr, working_path.c_str(), &startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
		return true;
	}
	return false;
}

bool UsbMmidd::Uninstall()
{
	const std::string working_path = UsbMmiddData::GetInstance()->GetPath() + "\\";
	const std::string full_exe_path = working_path + "deviceinstaller64.exe";
	std::string command_line_1 = "\"" + full_exe_path + "\" stop usbmmidd";
	std::string command_line_2 = "\"" + full_exe_path + "\" remove usbmmidd";

	STARTUPINFOA startup_info;
	RtlZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;
	RtlZeroMemory(&process_info, sizeof(process_info));

	if (CreateProcessA(nullptr, &command_line_1[0], nullptr, nullptr, FALSE, 0, nullptr, working_path.c_str(), &startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);

		RtlZeroMemory(&startup_info, sizeof(startup_info));
		startup_info.cb = sizeof(startup_info);
		RtlZeroMemory(&process_info, sizeof(process_info));
		if (CreateProcessA(nullptr, &command_line_2[0], nullptr, nullptr, FALSE, 0, nullptr, working_path.c_str(), &startup_info, &process_info))
		{
			WaitForSingleObject(process_info.hProcess, INFINITE);
			CloseHandle(process_info.hProcess);
			CloseHandle(process_info.hThread);

			return UsbMmiddData::GetInstance()->UninstallUsbMmidd();
		}
	}

	UsbMmiddData::GetInstance()->UninstallUsbMmidd();
	return false;
}

bool UsbMmidd::SetupResolution(int width, int height)
{
	const char* sub_key = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WUDF\\Services\\usbmmIdd\\Parameters\\Monitors";
	const char* value = "9";
	std::string data = std::format("{},{}", width, height);

	HKEY key;
	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, sub_key, 0, KEY_READ | KEY_WRITE, &key);
	if (result != ERROR_SUCCESS)
		return false;

	DWORD type = 0;
	DWORD data_size = 0;
	result = RegQueryValueExA(key, value, NULL, &type, NULL, &data_size);
	if (result == ERROR_FILE_NOT_FOUND)
	{
		result = RegSetValueExA(key, value, 0, REG_SZ, (const BYTE*)data.c_str(), data.length() + 1);
		if (result != ERROR_SUCCESS)
		{
			RegCloseKey(key);
			return false;
		}
	}
	else if (result == ERROR_SUCCESS)
	{
		result = RegSetValueExA(key, value, 0, REG_SZ, (const BYTE*)data.c_str(), data.length() + 1);
		if (result != ERROR_SUCCESS)
		{
			RegCloseKey(key);
			return false;
		}
	}
	else
	{
		RegCloseKey(key);
		return false;
	}

	RegCloseKey(key);
	return true;
}

bool UsbMmidd::ChangeResolution(int width, int height, std::string* device_name)
{
	int index = 0;
	DISPLAY_DEVICE display_device;
	display_device.cb = sizeof(DISPLAY_DEVICE);
	while (EnumDisplayDevicesA(NULL, index, &display_device, 0))
	{
		if (display_device.StateFlags & DISPLAY_DEVICE_ACTIVE && !(display_device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
		{
			if (strcmp(display_device.DeviceString, "USB Mobile Monitor Virtual Display") == 0)
			{
				DEVMODE devMode = {};
				devMode.dmSize = sizeof(devMode);
				devMode.dmPelsWidth = width;
				devMode.dmPelsHeight = height;
				devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

				*device_name = display_device.DeviceName;
				LONG result = ChangeDisplaySettingsExA(display_device.DeviceName, &devMode, NULL, CDS_UPDATEREGISTRY | CDS_FULLSCREEN, NULL);
				if (result != DISP_CHANGE_SUCCESSFUL)
					return false;

				break;
			}
		}
		index++;
	}
	return true;
}

bool UsbMmidd::GetVirtualDisplayPosition(int* x, int* y)
{
	int index = 0;
	DISPLAY_DEVICE display_device;
	ZeroMemory(&display_device, sizeof(DISPLAY_DEVICE));
	display_device.cb = sizeof(DISPLAY_DEVICE);
	while (EnumDisplayDevicesA(NULL, index, &display_device, 0))
	{
		if (display_device.StateFlags & DISPLAY_DEVICE_ACTIVE && !(display_device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE))
		{
			if (strcmp(display_device.DeviceString, "USB Mobile Monitor Virtual Display") != 0)
				continue;

			DEVMODE dev_mode;
			ZeroMemory(&dev_mode, sizeof(DEVMODE));
			dev_mode.dmSize = sizeof(DEVMODE);

			if (EnumDisplaySettingsA(display_device.DeviceName, ENUM_CURRENT_SETTINGS, &dev_mode))
			{
				*x = dev_mode.dmPosition.x;
				*y = dev_mode.dmPosition.y;
			}
			break;
		}
		index++;
	}
	return true;
}

bool UsbMmidd::EnableVirtualDisplay()
{
	const std::string working_path = UsbMmiddData::GetInstance()->GetPath() + "\\";
	const std::string full_exe_path = working_path + "deviceinstaller64.exe";
	std::string command_line = "\"" + full_exe_path + "\" enableidd 1";

	STARTUPINFOA startup_info;
	RtlZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;
	RtlZeroMemory(&process_info, sizeof(process_info));

	if (CreateProcessA(nullptr, &command_line[0], nullptr, nullptr, FALSE, 0, nullptr, working_path.c_str(), &startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);

		return true;
	}
	return false;
}

bool UsbMmidd::DisableVirtualDisplay()
{
	const std::string working_path = UsbMmiddData::GetInstance()->GetPath() + "\\";
	const std::string full_exe_path = working_path + "deviceinstaller64.exe";
	std::string command_line = "\"" + full_exe_path + "\" enableidd 0";

	STARTUPINFOA startup_info;
	RtlZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);

	PROCESS_INFORMATION process_info;
	RtlZeroMemory(&process_info, sizeof(process_info));

	if (CreateProcessA(nullptr, &command_line[0], nullptr, nullptr, FALSE, 0, nullptr, working_path.c_str(), &startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);

		return true;
	}
	return false;
}

void UsbMmidd::Cleanup()
{
	delete instance;
	instance = nullptr;
}

UsbMmidd* UsbMmidd::instance = nullptr;
std::mutex UsbMmidd::mtx;