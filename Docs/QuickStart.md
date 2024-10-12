# Quick Start

Visual Studio 2022 was used for the development, with an x86/Debug and Windows build.

### 1. Downloading the repository

Start by downloading a Zip from the "<> Code" option.

### 2. Building the Project

A build script is not included. You can build the project directly in Visual Studio Community Edition (Press F5): 

- Build Configuration: Choose either Release or Debug.
- Supported Platform: x86.
### 3. Using Scene Manager
The project uses a `SceneManager` to handle multiple scene, each abstracted into separate classes. 

#### Registering Scenes
Scenes are registed with the `SceneManager` using a template class and a uniques string key for later retrieval.
In `Game.cpp` (within the Init function):
```
//Initialise scene manager
m_SceneManger = new SceneManager();

//Register scenes with a unique key
m_SceneManger->RegisterNewScene<MainScene>("Main Scene");
m_SceneManager->RegisterNewScene<Texture_FrameBufferScene>("Haha Frame buffer");
```
#### Loading and Storing Scenes:
To load a registered scene, use `LoadScene(const std::string& name_key, Window* window)`. This method returns `nullptr` if the scene associated with `name_key` doesn't exist:
 ```
m_CurrentScene = m_SceneManager->LoadScene("Haha Frame buffer", m_Window);
```
#### Dynamically Unloading and Loading Scenes:
To unload the current scene and load another:
 ```
m_CurrentScene->OnDestroy();
m_CurrentScene = nullptr;
m_CurrentScene = m_SceneManager->LoadScene(m_SceneManager->ScenesByNamePtr()[1], m_Window);
 ```
You can also access scenes by index: `ScenesByNamePrt()` returns an array of scene names, which can be used to load scenes dynamically by their index.
### 4. Scene
The Base `Scene` class is a wrapper around OpenGL functions and window creation, designed to facilitate learning by allowing you to experiment with new rendering techniques without overwriting previous leaned methods.
