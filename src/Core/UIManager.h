

class UIManager
{
public:
	UIManager() = default;
	UIManager(const class Window& window);

	void OnInit(const class Window& window);
	void OnStartFrame();
	void OnEndFrame();
	void OnDestroy();

	~UIManager();
};