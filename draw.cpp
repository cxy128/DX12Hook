#include "draw.h"
#include "frame.h"

DWORD DrawImGui(void* Value) {

	Instance = reinterpret_cast<HINSTANCE>(Value);

	ImageBaseAddress = reinterpret_cast<unsigned __int64>(GetModuleHandleA("wuxia_client_dx12.exe"));

	if (!InitSystemRoutineAddress()) {
		return false;
	}

	if (!InitDX12Hook()) {
		return false;
	}

	return true;
}

bool InitDX12Hook() {

	auto __f = [](void*) -> DWORD {

		KiDelayExecutionThread(60);

		WNDCLASSEXW WindowClassEx = {
			sizeof(WindowClassEx),
			CS_HREDRAW | CS_VREDRAW,
			DefWindowProc,
			0L,
			0L,
			GetModuleHandle(nullptr),
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			L"_InterlockedExchangePointer",
			nullptr
		};

		::RegisterClassExW(&WindowClassEx);

		HWND __Hwnd = ::CreateWindowW(WindowClassEx.lpszClassName, L"_InterlockedExchangePointer", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, WindowClassEx.hInstance, nullptr);

		IDXGIFactory4* __DxgiFactory = nullptr;
		IDXGIAdapter* __DxgiAdapter = nullptr;
		ID3D12Device* __D3D12Device = nullptr;
		ID3D12CommandQueue* __D3D12CommandQueue = nullptr;
		IDXGISwapChain* __DxgiSwapChain = nullptr;
		IDXGISwapChain3* __DxgiSwapChain3 = nullptr;

		if (CreateDXGIFactory1(IID_PPV_ARGS(&__DxgiFactory)) != S_OK) {
			return false;
		}

		if (__DxgiFactory->EnumAdapters(0, &__DxgiAdapter) == DXGI_ERROR_NOT_FOUND) {
			return false;
		}

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1;
		if (D3D12CreateDevice(__DxgiAdapter, featureLevel, IID_PPV_ARGS(&__D3D12Device)) != S_OK) {
			return false;
		}

		D3D12_COMMAND_QUEUE_DESC D3d12CommandQueueDesc = {};
		D3d12CommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		D3d12CommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		if (__D3D12Device->CreateCommandQueue(&D3d12CommandQueueDesc, IID_PPV_ARGS(&__D3D12CommandQueue)) != S_OK) {
			return false;
		}

		DXGI_SWAP_CHAIN_DESC DxgiSwapChainDesc = {};
		DxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		DxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		DxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		DxgiSwapChainDesc.BufferCount = 2;
		DxgiSwapChainDesc.OutputWindow = __Hwnd;
		DxgiSwapChainDesc.Windowed = 1;
		DxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		DxgiSwapChainDesc.BufferDesc.Width = 0;
		DxgiSwapChainDesc.BufferDesc.Height = 0;
		DxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		DxgiSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		DxgiSwapChainDesc.SampleDesc.Count = 1;
		DxgiSwapChainDesc.SampleDesc.Quality = 0;
		if (__DxgiFactory->CreateSwapChain(__D3D12CommandQueue, &DxgiSwapChainDesc, &__DxgiSwapChain) != S_OK) {
			return false;
		}

		if (__DxgiSwapChain->QueryInterface(IID_PPV_ARGS(&__DxgiSwapChain3)) != S_OK) {
			return false;
		}

		class __DX12 {

		public:
			void** vft1;
			void** vft2;
			void** vft3;
		};

		auto DX12SwapChain = reinterpret_cast<__DX12*>(__DxgiSwapChain);
		auto DX12SwapChain3 = reinterpret_cast<__DX12*>(__DxgiSwapChain3);
		auto DX12CommandQueue = reinterpret_cast<__DX12*>(__D3D12CommandQueue);

		{
			ULONG AccessProtection = 0;
			SIZE_T NumberOfBytesToProtect = 1;

			if (VirtualProtect(PAGE_ALIGN(DX12SwapChain3->vft1), 1, PAGE_READWRITE, &AccessProtection)) {

				fPresent1Trampoline = reinterpret_cast<fnPresent1>(_InterlockedExchangePointer(&DX12SwapChain3->vft1[22], fPresent1));

				VirtualProtect(DX12SwapChain3->vft1, 1, AccessProtection, &AccessProtection);
			}
		}

		{
			ULONG AccessProtection = 0;
			SIZE_T NumberOfBytesToProtect = 1;

			if (VirtualProtect(PAGE_ALIGN(DX12CommandQueue->vft1), 1, PAGE_READWRITE, &AccessProtection)) {

				fExecuteCommandListsTrampoline = reinterpret_cast<fnExecuteCommandLists>(_InterlockedExchangePointer(&DX12CommandQueue->vft1[10], fExecuteCommandLists));

				VirtualProtect(DX12CommandQueue->vft1, 1, AccessProtection, &AccessProtection);
			}
		}

		//__DxgiSwapChain3->Release();
		//__DxgiSwapChain->Release();
		//__D3D12CommandQueue->Release();
		//__D3D12Device->Release();
		//__DxgiAdapter->Release();
		//__DxgiFactory->Release();

		::DestroyWindow(__Hwnd);
		::UnregisterClass(WindowClassEx.lpszClassName, WindowClassEx.hInstance);

		return 0;
	};

	CreateThread(nullptr, 0, __f, nullptr, 0, nullptr);

	return true;
}

void fExecuteCommandLists(ID3D12CommandQueue* CommandQueue, unsigned __int32 NumCommandLists, ID3D12CommandList* const* ppCommandLists) {

	if (!D3D12CommandQueue && CommandQueue->GetDesc().Type == D3D12_COMMAND_LIST_TYPE_DIRECT) {
		D3D12CommandQueue = CommandQueue;
	}

	return fExecuteCommandListsTrampoline(CommandQueue, NumCommandLists, ppCommandLists);
}

HRESULT fPresent1(IDXGISwapChain3* SwapChain, unsigned __int32 SyncInterval, unsigned __int32 PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters) {

	if (!IsInitPresent1 && CreateDeviceD3D(SwapChain)) {

		WndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(WuXiaHwnd, GWLP_WNDPROC, reinterpret_cast<__int64>(fWndProc)));

		ImGui::CreateContext();

		auto& Io = ImGui::GetIO();
		Io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		Io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 22.0f, nullptr, Io.Fonts->GetGlyphRangesChineseFull());

		auto& Style = ImGui::GetStyle();
		Style.WindowRounding = 8.0f;
		Style.FrameRounding = 5.0f;

		ImGui::StyleColorsDark();

		ImVec4* colors = Style.Colors;
		colors[ImGuiCol_WindowBg] = ImRgbaToImVec4(44, 62, 80, 1.0);

		ImGui_ImplWin32_Init(WuXiaHwnd);
		ImGui_ImplDX12_Init(
			D3D12Device,
			BufferCount,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D12SrvDescHeap,
			D3D12SrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			D3D12SrvDescHeap->GetGPUDescriptorHandleForHeapStart()
		);

		IsInitPresent1 = true;
	}

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();

	ImGui::NewFrame();

	Frame();

	auto CurrentBackBufferIndex = SwapChain->GetCurrentBackBufferIndex();
	CommandAllocator[CurrentBackBufferIndex]->Reset();

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = MainRenderTargetResource[CurrentBackBufferIndex];
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	D3D12CommandList->Reset(CommandAllocator[CurrentBackBufferIndex], nullptr);
	D3D12CommandList->ResourceBarrier(1, &barrier);

	D3D12CommandList->OMSetRenderTargets(1, &MainRenderTargetDescriptor[CurrentBackBufferIndex], FALSE, nullptr);
	D3D12CommandList->SetDescriptorHeaps(1, &D3D12SrvDescHeap);

	ImGui::Render();

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), D3D12CommandList);

	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

	D3D12CommandList->ResourceBarrier(1, &barrier);
	D3D12CommandList->Close();

	D3D12CommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&D3D12CommandList);

	return fPresent1Trampoline(SwapChain, SyncInterval, PresentFlags, pPresentParameters);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT fWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
		return true;
	}

	return CallWindowProc(WndProc, hWnd, uMsg, wParam, lParam);
}

bool CreateDeviceD3D(IDXGISwapChain3* SwapChain) {

	DXGI_SWAP_CHAIN_DESC DxgiSwapChainDesc = {};
	if (SwapChain->GetDesc(&DxgiSwapChainDesc) != S_OK) {
		return false;
	}

	WuXiaHwnd = DxgiSwapChainDesc.OutputWindow;

	BufferCount = DxgiSwapChainDesc.BufferCount;

	MainRenderTargetDescriptor.clear();
	MainRenderTargetDescriptor.resize(BufferCount);

	MainRenderTargetResource.clear();
	MainRenderTargetResource.resize(BufferCount);

	CommandAllocator.clear();
	CommandAllocator.resize(BufferCount);

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_1;
	if (SwapChain->GetDevice(IID_PPV_ARGS(&D3D12Device)) != S_OK) {
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC D3d12DescriptorHeapDescSrv = {};
	D3d12DescriptorHeapDescSrv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	D3d12DescriptorHeapDescSrv.NumDescriptors = BufferCount;
	D3d12DescriptorHeapDescSrv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (D3D12Device->CreateDescriptorHeap(&D3d12DescriptorHeapDescSrv, IID_PPV_ARGS(&D3D12SrvDescHeap)) != S_OK) {
		return false;
	}


	D3D12_DESCRIPTOR_HEAP_DESC D3D12DescriptorHeapDescRtv = {};
	D3D12DescriptorHeapDescRtv.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	D3D12DescriptorHeapDescRtv.NumDescriptors = BufferCount;
	D3D12DescriptorHeapDescRtv.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	D3D12DescriptorHeapDescRtv.NodeMask = 1;
	if (D3D12Device->CreateDescriptorHeap(&D3D12DescriptorHeapDescRtv, IID_PPV_ARGS(&D3D12RtvDescHeap)) != S_OK) {
		return false;
	}

	SIZE_T RtvDescriptorSize = D3D12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = D3D12RtvDescHeap->GetCPUDescriptorHandleForHeapStart();

	for (unsigned __int32 i = 0; i < BufferCount; i++) {

		MainRenderTargetDescriptor[i] = rtvHandle;

		SwapChain->GetBuffer(i, IID_PPV_ARGS(&MainRenderTargetResource[i]));

		D3D12Device->CreateRenderTargetView(MainRenderTargetResource[i], nullptr, rtvHandle);

		rtvHandle.ptr += RtvDescriptorSize;
	}

	for (unsigned __int32 i = 0; i < BufferCount; i++) {

		if (D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocator[i])) != S_OK) {

			return false;
		}
	}

	if (D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocator[0], nullptr, IID_PPV_ARGS(&D3D12CommandList)) != S_OK || D3D12CommandList->Close() != S_OK) {
		return false;
	}

	return true;
}
