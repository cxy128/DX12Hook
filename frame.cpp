#include "frame.h"
#include "util.h"

void Frame() {

	UpdateWindowAlpha();

	if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q)) {
		IsShowImGuiWindow = !IsShowImGuiWindow;
	}

	if (!WindowAlpha) {
		return;
	}

	ImGui::SetNextWindowBgAlpha(WindowAlpha);

	ImGui::Begin("ImGui", nullptr, ImGuiWindowFlags_NoTitleBar);

	if (!IsInitImGuiWindow) {
		ImGui::SetWindowSize(ImVec2(480.0f, 230.0f));
		ImGui::SetWindowPos(ImVec2(60.0f, 70.0f));
		IsInitImGuiWindow = true;
	}

	ImGui::End();
}

void UpdateWindowAlpha() {

	if (IsShowImGuiWindow && WindowAlpha < 1.0f) {

		WindowAlpha += FadeSpeed;

		if (WindowAlpha > 1.0f) {
			WindowAlpha = 1.0f;
		}

	} else if (!IsShowImGuiWindow && WindowAlpha > 0.0f) {

		WindowAlpha -= FadeSpeed;

		if (WindowAlpha < 0.0f) {
			WindowAlpha = 0.0f;
		}
	}
}
