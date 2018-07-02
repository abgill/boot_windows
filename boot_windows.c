#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DevicePathToText.h>

EFI_STATUS EFIAPI UefiMain (IN EFI_HANDLE ImageHandle,IN EFI_SYSTEM_TABLE  *SystemTable){
	Print(L"CAEDM UEFI Boot\n");
	
	EFI_BOOT_SERVICES *bs = SystemTable->BootServices;
	
	EFI_GUID sfspGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
	EFI_HANDLE* handles = NULL;
	UINTN handleCount = 0;

	EFI_STATUS status = bs->LocateHandleBuffer(ByProtocol,&sfspGuid,NULL,&handleCount, &handles);

	for(UINTN idx =0; idx <  handleCount; ++idx){
		EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs = NULL;

		status = bs->HandleProtocol(handles[idx], &sfspGuid, (void**)&fs);

		EFI_DEVICE_PATH* dev_path = DevicePathFromHandle(handles[idx]);
		Print(L"Device path idx[%d]: %s\n",idx, ConvertDevicePathToText(dev_path, 0, 0));

		EFI_FILE_PROTOCOL* root = NULL;

		status = fs->OpenVolume(fs, &root);

		EFI_FILE_PROTOCOL* token = NULL;

		status = root->Open(root, &token, L"EFI\\BOOT\\bootx64.efi", EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);

		if(status == EFI_SUCCESS){
			Print(L"Found bootx64.efi\n");
			EFI_DEVICE_PATH_PROTOCOL* filePath = FileDevicePath(handles[idx], L"EFI\\BOOT\\bootx64.efi");
			Print(L"Path: %s\n",ConvertDevicePathToText(filePath, 0, 0));
			EFI_HANDLE imgHandle;
			status = bs->LoadImage(FALSE, ImageHandle, filePath, NULL, 0, &imgHandle);

			if(status == EFI_SUCCESS){
				Print(L"Loaded image!\n");
				bs->StartImage(imgHandle, 0, NULL);
			}

			else{
				Print(L"Image Load Failed (%r)\n", status);
			}

			break;
			
		}
		Print(L"status = %d \n", status);

	}

	Print(L"Unable to boot to hard drive");
	return EFI_SUCCESS;
}
