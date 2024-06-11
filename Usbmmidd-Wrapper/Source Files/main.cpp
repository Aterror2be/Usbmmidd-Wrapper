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