#include <vector>
#include <memory>
#include <unordered_map>
#include "External Libs/imgui/imgui.h"

class UIManager
{
public:
	UIManager() = default;
	UIManager(const class Window& window);

	void OnInit(const class Window& window);
	void OnStartFrame();
	void OnEndFrame();
	void OnDestroy();

	inline bool* ImGuiWantCaptureMouse() { return m_ImGuiWantCaptureMouse; }
	inline std::vector<const char*>& AvaliableFontString() { return m_AvaliableFonts; }
	ImFont* GetFont(std::string font_name);
	inline std::unordered_map<std::string, ImFont*>& GetFontsMap() { return m_Fonts; }

	~UIManager();

private:
	bool* m_ImGuiWantCaptureMouse;

	std::vector<const char*> m_AvaliableFonts;
	std::unordered_map<std::string, ImFont*> m_Fonts;
};