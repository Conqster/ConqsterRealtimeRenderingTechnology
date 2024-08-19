

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

	~UIManager();

private:
	bool* m_ImGuiWantCaptureMouse;
};