#include "pch.h"
#include "DemoDemon.h"
#include "bakkesmod/wrappers/GuiManagerWrapper.h"

#define FONT_SIZE 32
#define FONT_NAME "Bourgeois"
#define FONT_FILE "Bourgeois-BoldItalic.ttf"

std::string DemoDemon::GetPluginName() {
	return menuTitle;
}

void DemoDemon::RenderSettings() {
	// Settings
	CreateToggleableCheckbox("demodemon_enabled", "Enable overlay");
	CreateToggleableCheckbox("demodemon_display_game", "Display game stats");
	CreateToggleableCheckbox("demodemon_display_session", "Display session stats");
	CreateToggleableCheckbox("demodemon_display_alltime", "Display all time stats");

	// Note
	ImGui::Separator();
	ImGui::TextUnformatted("Note:\nReposition the overlay by dragging whilst the Bakkesmod menu is open.");

	// Credit
	ImGui::Separator();
	ImGui::TextUnformatted("Demo Demon by ");
	ImGui::SameLine(0, 0);
	ImGui::TextColored({ 0, 1, 1, 1 }, "AngaBlue");
	ImGui::TextUnformatted("https://anga.blue");
}

// Do ImGui rendering here
void DemoDemon::Render()
{
	// Check enabled
	if (!GetBoolCvar("demodemon_enabled")) return;

	// Check if in game
	if (!(gameWrapper->IsInGame() || gameWrapper->IsInOnlineGame()) || gameWrapper->IsInFreeplay()) return;

	// Load font
	if (!largeFont) {
		auto gui = gameWrapper->GetGUIManager();
		largeFont = gui.GetFont(FONT_NAME);
	}

	const ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground;
	if (!ImGui::Begin(menuTitle.c_str(), &isWindowOpen_, flags))
	{
		ImGui::End();
		return;
	}

	// Style
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowMinSize = { 200, 140 };

	// Content
	if (largeFont) {
		ImGui::PushFont(largeFont);
	}

	ImGui::Columns(2, 0, false);

	ImGui::SetColumnWidth(0, 120);
	ImGui::Text("KD");
	ImGui::Text("KILLS");
	ImGui::Text("DEATHS");
	ImGui::Text("TOTAL");

	ImGui::NextColumn();
	ImGui::SetColumnWidth(1, 80);
	std::string text = fmt::format("{:.2f}", game.getKD());
	RightAlignTextInColumn(text);
	ImGui::TextColored(game.getKDColor(), text.c_str());
	text = fmt::format("{:d}", game.getKills());
	RightAlignTextInColumn(text);
	ImGui::Text(text.c_str());
	text = fmt::format("{:d}", game.getDeaths());
	RightAlignTextInColumn(text);
	ImGui::Text(text.c_str());
	text = fmt::format("{:d}", total);
	RightAlignTextInColumn(text);
	ImGui::Text(text.c_str());

	ImGui::Columns();

	if (largeFont) {
		ImGui::PopFont();
	}
	// ImGui renreding ends here
	ImGui::End();
}

std::string DemoDemon::GetMenuTitle()
{
	return menuTitle;
}

std::string DemoDemon::GetMenuName()
{
	return menuName;
}

void DemoDemon::SetImGuiContext(uintptr_t ctx)
{
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));

	auto gui = gameWrapper->GetGUIManager();
	auto [res, font] = gui.LoadFont(FONT_FILE, FONT_FILE, FONT_SIZE);

	if (res == 0) {
		cvarManager->log("Failed to load the font!");
	}
	else if (res == 1) {
		cvarManager->log("The font will be loaded");
	}
	else if (res == 2 && font) {
		largeFont = font;
	}
}

bool DemoDemon::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

bool DemoDemon::IsActiveOverlay()
{
	return false;
}

void DemoDemon::OnOpen()
{
	isWindowOpen_ = true;
}

void DemoDemon::RightAlignTextInColumn(const std::string text)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(text.c_str()).x
		- ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);
}

void DemoDemon::OnClose()
{
	isWindowOpen_ = false;
}

void DemoDemon::StartRender()
{
	cvarManager->executeCommand("openmenu " + GetMenuName());
}

void DemoDemon::StopRender()
{
	cvarManager->executeCommand("closemenu " + GetMenuName());
}

void DemoDemon::CreateToggleableCheckbox(const std::string name, const char* const display)
{
	CVarWrapper cvar = cvarManager->getCvar(name);
	if (!cvar) return;

	bool state = cvar.getBoolValue();

	if (ImGui::Checkbox(display, &state)) {
		cvar.setValue(state);
	}
}