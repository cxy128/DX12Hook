#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"

inline bool IsShowImGuiWindow = true;

inline bool IsInitImGuiWindow = false;

inline float WindowAlpha = 1.0f;

inline float FadeSpeed = 0.25f;

void Frame();

void UpdateWindowAlpha();