#pragma once

#include <Windows.h>
#include <iostream>

#define PAGE_SHIFT 12L

#define PAGE_SIZE 0x1000

#define PAGE_ALIGN(Va) ((PVOID)((ULONG_PTR)(Va) & ~(PAGE_SIZE - 1)))

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#define NT_ERROR(Status) ((((ULONG)(Status)) >> 30) == 3)

#define ImRgbaToImVec4(x,y,z,w) ImVec4(x / 255.0f, y / 255.0f, z / 255.0f, w)

#define rgba(x,y,z,w) ImVec4(x / 255.0f, y / 255.0f, z / 255.0f, w)

constexpr auto STATUS_SUCCESS = 0;

enum class SYSTEM_INFORMATION_CLASS :unsigned __int32 {

	SystemModuleInformation = 0xb
};

struct RTL_PROCESS_MODULE_INFORMATION {

	PVOID  	Section;
	PVOID 	MappedBase;
	PVOID 	ImageBase;
	ULONG 	ImageSize;
	ULONG 	Flags;
	USHORT 	LoadOrderIndex;
	USHORT 	InitOrderIndex;
	USHORT 	LoadCount;
	USHORT 	OffsetToFileName;
	CHAR 	FullPathName[256];
};

struct RTL_PROCESS_MODULES {

	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
};

struct PEB {

	UCHAR InheritedAddressSpace;                                              //0x0
	UCHAR ReadImageFileExecOptions;                                           //0x1
	UCHAR BeingDebugged;                                                      //0x2

	union {

		UCHAR BitField;                                                       //0x3

		struct {

			UCHAR ImageUsesLargePages : 1;                                    //0x3
			UCHAR IsProtectedProcess : 1;                                     //0x3
			UCHAR IsImageDynamicallyRelocated : 1;                            //0x3
			UCHAR SkipPatchingUser32Forwarders : 1;                           //0x3
			UCHAR IsPackagedProcess : 1;                                      //0x3
			UCHAR IsAppContainer : 1;                                         //0x3
			UCHAR IsProtectedProcessLight : 1;                                //0x3
			UCHAR IsLongPathAwareProcess : 1;                                 //0x3
		};
	};

	UCHAR Padding0[4];                                                      //0x4
	VOID* Mutant;                                                           //0x8
	VOID* ImageBaseAddress;                                                 //0x10
};

using fnZwQuerySystemInformation = NTSTATUS(*)(SYSTEM_INFORMATION_CLASS SystemInfoClass, PVOID SystemInfoBuffer, ULONG SystemInfoBufferSize, PULONG BytesReturned);

using fnZwAllocateVirtualMemory = NTSTATUS(*)(HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);

using fnZwReadVirtualMemory = NTSTATUS(*)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, ULONG NumberOfBytesToRead, PULONG NumberOfBytesReaded);

using fnZwWriteVirtualMemory = NTSTATUS(*)(HANDLE ProcessHandle, PVOID BaseAddress, PVOID Buffer, SIZE_T NumberOfBytesToWrite, PSIZE_T NumberOfBytesWritten);

using fnZwProtectVirtualMemory = NTSTATUS(*)(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);

inline fnZwQuerySystemInformation ZwQuerySystemInformation;

inline fnZwAllocateVirtualMemory ZwAllocateVirtualMemory;

inline fnZwReadVirtualMemory ZwReadVirtualMemory;

inline fnZwWriteVirtualMemory ZwWriteVirtualMemory;

inline fnZwProtectVirtualMemory ZwProtectVirtualMemory;

inline auto Print = []<typename ...E>(const char* Format, E ...Arguments) -> void {

	char buf[256] = "";
	sprintf_s(buf, 256, Format, Arguments...);

	OutputDebugStringA(buf);
};

inline auto KiDelayExecutionThread = [](DWORD Second) -> void {

	Sleep(Second * 1000);
};

inline bool MmIsValidMemory(const void* p) {

	MEMORY_BASIC_INFORMATION MemoryInformation = {};

	if (VirtualQuery(p, &MemoryInformation, sizeof(MEMORY_BASIC_INFORMATION))) {

		constexpr DWORD f = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);

		bool R = MemoryInformation.Protect & f;

		if (MemoryInformation.Protect & (PAGE_GUARD | PAGE_NOACCESS)) {
			R = false;
		}

		return R;
	}

	return false;
};

inline bool MmIsValidMemory(const unsigned __int64 p) {

	return MmIsValidMemory(reinterpret_cast<const void*>(p));
};

bool InitSystemRoutineAddress();

unsigned __int64 SearchSignature(unsigned __int64 ModuleAddress, unsigned char* SignatureBytes, const char* Segment, const char* Mask);

bool HookX64RoutineAddress(unsigned __int64* OriginToTrampoline, void* Handler, unsigned __int32 PatchSize);