# USBMMIDD Wrapper

## Disclaimer

I have no association with Amyuni Technologies or any of its products or services. This project is an independent, third-party the original project and binaries can be found at https://www.amyuni.com/forum/viewtopic.php?t=3030

## Classes and Functions

### UsbMmidd

This class is responsible for managing USB MMIDD operations such as installation, uninstallation, resolution setup, and virtual display management.

#### Methods

- **`static UsbMmidd* GetInstance()`**
  - Returns the singleton instance of the `UsbMmidd` class.
  
- **`bool Install()`**
  - Installs the USB MMIDD.
  - Returns `true` if the installation is successful, otherwise `false`.

- **`bool Uninstall()`**
  - Uninstalls the USB MMIDD.
  - Returns `true` if the uninstallation is successful, otherwise `false`.

- **`bool SetupResolution(int width, int height)`**
  - Sets up the resolution of the virtual display.
  - `width`: Width of the display.
  - `height`: Height of the display.
  - Returns `true` if the resolution setup is successful, otherwise `false`.

- **`bool ChangeResolution(int width, int height, std::string* device_name)`**
  - Changes the resolution of the virtual display.
  - `width`: New width of the display.
  - `height`: New height of the display.
  - `device_name`: Name of the device whose resolution is being changed.
  - Returns `true` if the resolution change is successful, otherwise `false`.

- **`bool GetVirtualDisplayPosition(int* x, int* y)`**
  - Gets the position of the virtual display.
  - `x`: Pointer to store the x-coordinate.
  - `y`: Pointer to store the y-coordinate.
  - Returns `true` if the position retrieval is successful, otherwise `false`.

- **`bool EnableVirtualDisplay()`**
  - Enables the virtual display.
  - Returns `true` if the virtual display is enabled successfully, otherwise `false`.

- **`bool DisableVirtualDisplay()`**
  - Disables the virtual display.
  - Returns `true` if the virtual display is disabled successfully, otherwise `false`.

### UsbMmiddData

#### Methods

- **`static UsbMmiddData* GetInstance()`**
  - Returns the singleton instance of the `UsbMmiddData` class.

- **`bool InstallUsbMmidd()`**
  - Installs the USB MMIDD data.
  - Returns `true` if the installation is successful, otherwise `false`.

- **`bool UninstallUsbMmidd()`**
  - Uninstalls the USB MMIDD data.
  - Returns `true` if the uninstallation is successful, otherwise `false`.

- **`std::string GetPath()`**
  - Gets the path to the USB MMIDD data.
  - Returns the path as a string.

## Usage

To use the `UsbMmidd` and `UsbMmiddData` classes, include the headers and call the functions as needed. Below is a simple example:

```cpp
#include <cstdlib>
#include <stdlib.h>

#include "UsbMmiddData.hpp"
#include "UsbMmidd.hpp"

int main()
{
	UsbMmidd* usbmmidd = UsbMmidd::GetInstance();
	if (usbmmidd == nullptr)
		return 1;

	if (usbmmidd->Install() == false)
		return 1;

	if (usbmmidd->SetupResolution(1920, 1080) == false)
		return 1;

	usbmmidd->EnableVirtualDisplay();

	//wait for user input before uninstalling the virtual display
	system("pause");

	usbmmidd->DisableVirtualDisplay();

	usbmmidd->Uninstall();
	return 0;
}
