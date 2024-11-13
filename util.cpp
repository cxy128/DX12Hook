#include <Windows.h>
#include <iostream>
#include "util.h"

bool InitSystemRoutineAddress() {

	auto Handle = GetModuleHandleA("ntdll.dll");
	if (!Handle) {
		return false;
	}

	ZwQuerySystemInformation = (fnZwQuerySystemInformation)GetProcAddress(Handle, "ZwQuerySystemInformation");
	if (!ZwQuerySystemInformation) {
		return false;
	}

	ZwAllocateVirtualMemory = (fnZwAllocateVirtualMemory)GetProcAddress(Handle, "ZwAllocateVirtualMemory");
	if (!ZwAllocateVirtualMemory) {
		return false;
	}

	ZwReadVirtualMemory = (fnZwReadVirtualMemory)GetProcAddress(Handle, "ZwReadVirtualMemory");
	if (!ZwReadVirtualMemory) {
		return false;
	}

	ZwWriteVirtualMemory = (fnZwWriteVirtualMemory)GetProcAddress(Handle, "ZwWriteVirtualMemory");
	if (!ZwWriteVirtualMemory) {
		return false;
	}

	ZwProtectVirtualMemory = (fnZwProtectVirtualMemory)GetProcAddress(Handle, "ZwProtectVirtualMemory");
	if (!ZwProtectVirtualMemory) {
		return false;
	}

	return true;
}

unsigned __int64 SearchSignature(unsigned __int64 ModuleAddress, unsigned char* SignatureBytes, const char* Segment, const char* Mask) {

	IMAGE_DOS_HEADER* DosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(ModuleAddress);
	if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		return 0;
	}

	IMAGE_NT_HEADERS* NtHeader = reinterpret_cast<IMAGE_NT_HEADERS*>(ModuleAddress + DosHeader->e_lfanew);

	unsigned __int64 MaskLength = strlen(Mask);
	PIMAGE_SECTION_HEADER Sections = IMAGE_FIRST_SECTION(NtHeader);

	for (int i = 0; i < NtHeader->FileHeader.NumberOfSections; i++) {

		IMAGE_SECTION_HEADER* Section = &Sections[i];

		if (_stricmp((const char*)(Section->Name), Segment) == 0) {

			unsigned __int64 SectionAddress = ModuleAddress + Section->VirtualAddress;

			for (unsigned __int32 n = 0; n < Section->Misc.VirtualSize - MaskLength; n++) {

				int f = 1;

				for (unsigned __int64 x = 0; x < MaskLength; x++) {

					if (((((unsigned char*)(SectionAddress + n))[x]) == SignatureBytes[x]) || Mask[x] == '?') {
						continue;
					}

					f = 0;
					break;
				}

				if (f) {
					return SectionAddress + n;
				}
			}
		}
	}

	return 0;
}

bool HookX64RoutineAddress(unsigned __int64* OriginToTrampoline, void* Handler, unsigned __int32 PatchSize) {

	unsigned char TrampolineCode[] = {
		0x6A, 0x00,													// push 0
		0x36, 0xC7, 0x04, 0x24 ,0x00, 0x00, 0x00, 0x00,	 			// mov dword ptr ss : [rsp] , 0x00
		0x36, 0xC7, 0x44, 0x24 ,0x04 ,0x00, 0x00, 0x00,  0x00,		// mov dword ptr ss : [rsp + 4] , 0x00
		0xC3														// ret
	};

	*(unsigned __int32*)&TrampolineCode[6] = (unsigned __int32)((*OriginToTrampoline + PatchSize) & 0xFFFFFFFF);
	*(unsigned __int32*)&TrampolineCode[15] = (unsigned __int32)(((*OriginToTrampoline + PatchSize) >> 32) & 0xFFFFFFFF);

	unsigned __int8* TrampolineBuffer = nullptr;
	SIZE_T RegionSize = 0x1000;
	auto Status = ZwAllocateVirtualMemory(GetCurrentProcess(), (void**)&TrampolineBuffer, 0, &RegionSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (NT_ERROR(Status)) {
		return false;
	}

	RtlZeroMemory(TrampolineBuffer, PAGE_SIZE);

	RtlCopyMemory(TrampolineBuffer, reinterpret_cast<unsigned __int64*>(*OriginToTrampoline), PatchSize);
	RtlCopyMemory(TrampolineBuffer + PatchSize, TrampolineCode, sizeof(TrampolineCode));

	unsigned char JmpBytes[] = {
		0xff,0x25,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	};

	*(unsigned __int64*)(&JmpBytes[6]) = (unsigned __int64)Handler;

	auto Protect = 0ul;
	if (!VirtualProtect((void*)*OriginToTrampoline, 0x1000, PAGE_EXECUTE_READWRITE, &Protect)) {
		return false;
	}

	RtlCopyMemory((void*)*OriginToTrampoline, JmpBytes, sizeof(JmpBytes));

	if (!VirtualProtect((void*)*OriginToTrampoline, 0x1000, Protect, &Protect)) {
		return false;
	}

	*OriginToTrampoline = reinterpret_cast<unsigned __int64>(TrampolineBuffer);

	return true;
}
