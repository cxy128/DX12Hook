#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "util.h"
#include <vector>
#include <atlbase.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")

inline ID3D12CommandQueue* D3D12CommandQueue = nullptr;

inline ID3D12Device* D3D12Device = nullptr;

inline unsigned __int32 BufferCount = 0;

inline ID3D12DescriptorHeap* D3D12SrvDescHeap = nullptr;
inline ID3D12DescriptorHeap* D3D12RtvDescHeap = nullptr;

inline std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> MainRenderTargetDescriptor = {};
inline std::vector<CComPtr<ID3D12Resource>> MainRenderTargetResource = {};
inline std::vector<CComPtr<ID3D12CommandAllocator>> CommandAllocator = {};

inline ID3D12GraphicsCommandList* D3D12CommandList = nullptr;

using fnExecuteCommandLists = void(*)(ID3D12CommandQueue* CommandQueue, unsigned __int32 NumCommandLists, ID3D12CommandList* const* ppCommandLists);

using fnPresent1 = HRESULT(*)(IDXGISwapChain3* SwapChain, unsigned __int32 SyncInterval, unsigned __int32 PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);

inline HINSTANCE Instance;

inline unsigned __int64 ImageBaseAddress;

inline WNDPROC WndProc = nullptr;

inline bool IsInitPresent1 = false;

inline fnExecuteCommandLists fExecuteCommandListsTrampoline = nullptr;

inline fnPresent1 fPresent1Trampoline = nullptr;

inline HWND WuXiaHwnd = nullptr;

DWORD DrawImGui(void*);

bool InitDX12Hook();

void fExecuteCommandLists(ID3D12CommandQueue* CommandQueue, unsigned __int32 NumCommandLists, ID3D12CommandList* const* ppCommandLists);

HRESULT fPresent1(IDXGISwapChain3* SwapChain, unsigned __int32 SyncInterval, unsigned __int32 PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters);

LRESULT fWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool CreateDeviceD3D(IDXGISwapChain3* SwapChain);
