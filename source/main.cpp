#include "camera.hpp"
#include "level.hpp"

#include <OgreAbiUtils.h>
#include <OgreArchiveManager.h>
#include <OgreCamera.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorNodeDef.h>
#include <Compositor/OgreCompositorShadowNode.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <Compositor/Pass/PassScene/OgreCompositorPassSceneDef.h>
#include <OgreException.h>
#include <OgreHlmsManager.h>
#include <OgreHlmsPbs.h>
#include <OgreHlmsUnlit.h>
#include <OgrePlugin.h>
#include <OgreRenderSystem.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreStringConverter.h>
#include <OgreWindow.h>
#include <OgreWindowEventUtilities.h>

#include <SDL.h>
#include <SDL_syswm.h>

#if defined(OGRE_STATIC_LIB)
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include <OgreMetalPlugin.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <OgreD3D11Plugin.h>
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <OgreGL3PlusPlugin.h>
#endif
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "platform/osx_window.hpp"
#endif

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace {
std::filesystem::path executableDirectory() {
  char *basePath = SDL_GetBasePath();
  if (!basePath)
    throw std::runtime_error("SDL_GetBasePath failed: " +
                             std::string(SDL_GetError()));
  const std::filesystem::path result(basePath);
  SDL_free(basePath);
  return result;
}

const char *rendererName() {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  return "Metal Rendering Subsystem";
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  return "Direct3D11 Rendering Subsystem";
#else
  return "OpenGL 3+ Rendering Subsystem";
#endif
}

void registerHlms(const std::filesystem::path &hlmsRoot) {
  using namespace Ogre;

  String root = hlmsRoot.generic_string();
  if (!root.ends_with('/'))
    root += '/';

  ArchiveManager &archives = ArchiveManager::getSingleton();
  HlmsManager *manager = Root::getSingleton().getHlmsManager();

  String mainPath;
  StringVector libraryPaths;

  HlmsUnlit::getDefaultPaths(mainPath, libraryPaths);
  Archive *unlitArchive = archives.load(root + mainPath, "FileSystem", true);
  ArchiveVec unlitLibraries;
  for (const String &path : libraryPaths)
    unlitLibraries.push_back(archives.load(root + path, "FileSystem", true));
  manager->registerHlms(OGRE_NEW HlmsUnlit(unlitArchive, &unlitLibraries));

  HlmsPbs::getDefaultPaths(mainPath, libraryPaths);
  Archive *pbsArchive = archives.load(root + mainPath, "FileSystem", true);
  ArchiveVec pbsLibraries;
  for (const String &path : libraryPaths)
    pbsLibraries.push_back(archives.load(root + path, "FileSystem", true));
  manager->registerHlms(OGRE_NEW HlmsPbs(pbsArchive, &pbsLibraries));
}

class Application final {
public:
  ~Application() { shutdown(); }

  int run() {
    initialize();

    auto previousFrame = std::chrono::steady_clock::now();
    while (!mQuit) {
      pumpEvents();
      if (mQuit)
        break;

      const auto now = std::chrono::steady_clock::now();
      const float dt = std::min(
          std::chrono::duration<float>(now - previousFrame).count(), 0.1f);
      previousFrame = now;

      mCamera->update(dt);
      mLevel->update(dt);

      if (mRenderWindow->isVisible()) {
        mQuit |= !mRoot->renderOneFrame();
      } else {
        SDL_Delay(16u);
      }
    }

    return 0;
  }

private:
  void initialize() {
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS |
                 SDL_INIT_GAMECONTROLLER) != 0) {
      throw std::runtime_error("SDL_Init failed: " +
                               std::string(SDL_GetError()));
    }
    mSdlInitialized = true;

    constexpr int width = 1280;
    constexpr int height = 720;
    mSdlWindow = SDL_CreateWindow(
        "TSE", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!mSdlWindow)
      throw std::runtime_error("SDL_CreateWindow failed: " +
                               std::string(SDL_GetError()));

    const std::filesystem::path basePath = executableDirectory();
#if defined(OGRE_STATIC_LIB)
    const Ogre::String pluginsFile;
#else
    const Ogre::String pluginsFile = (basePath / "plugins.cfg").string();
#endif

    const Ogre::AbiCookie abiCookie = Ogre::generateAbiCookie();
    mRoot = OGRE_NEW Ogre::Root(&abiCookie, pluginsFile, "",
                                 (basePath / "Ogre.log").string(), "TSE");

#if defined(OGRE_STATIC_LIB)
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    mRenderPlugin = OGRE_NEW Ogre::MetalPlugin();
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    mRenderPlugin = OGRE_NEW Ogre::D3D11Plugin();
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    mRenderPlugin = OGRE_NEW Ogre::GL3PlusPlugin();
#endif
    mRoot->installPlugin(mRenderPlugin, nullptr);
#endif

    Ogre::RenderSystem *renderer =
        mRoot->getRenderSystemByName(rendererName());
    if (!renderer)
      throw std::runtime_error(std::string("OGRE-Next renderer unavailable: ") +
                               rendererName());

    renderer->setConfigOption("sRGB Gamma Conversion", "Yes");
    mRoot->setRenderSystem(renderer);
    mRoot->initialise(false, "TSE");

    Ogre::NameValuePairList windowParameters;
    addNativeWindowHandle(windowParameters);
    windowParameters["title"] = "TSE";
    windowParameters["gamma"] = "Yes";
    windowParameters["vsync"] = "Yes";
    windowParameters["reverse_depth"] = "Yes";
    mRenderWindow = mRoot->createRenderWindow("TSE", width, height, false,
                                               &windowParameters);

    const std::filesystem::path assets = basePath / "assets";
    registerHlms(assets / "Hlms");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
        (assets / "models").string(), "FileSystem", "Game");
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups(
        true);

    mSceneManager =
        mRoot->createSceneManager(Ogre::ST_GENERIC, 1u, "TSE Scene");
    mCamera = std::make_unique<Camera>(mSceneManager);
    mLevel = std::make_unique<Level>(mSceneManager);

    Ogre::CompositorManager2 *compositor = mRoot->getCompositorManager2();
    const Ogre::String workspaceName = "TSE Workspace";
    compositor->createBasicWorkspaceDef(
        workspaceName, Ogre::ColourValue(0.03f, 0.045f, 0.07f),
        Ogre::IdString());

    Ogre::ShadowNodeHelper::ShadowParam sunShadows{};
    sunShadows.technique = Ogre::SHADOWMAP_PSSM;
    sunShadows.numPssmSplits = 3u;
    sunShadows.resolution[0] = {2048u, 2048u};
    sunShadows.resolution[1] = {1024u, 1024u};
    sunShadows.resolution[2] = {1024u, 1024u};
    sunShadows.atlasStart[0] = {0u, 0u};
    sunShadows.atlasStart[1] = {0u, 2048u};
    sunShadows.atlasStart[2] = {1024u, 2048u};
    sunShadows.addLightType(Ogre::Light::LT_DIRECTIONAL);

    Ogre::ShadowNodeHelper::createShadowNodeWithSettings(
        compositor, renderer->getCapabilities(), "TSE Sun Shadows",
        {sunShadows}, false);

    const Ogre::String nodeName =
        "AutoGen " + Ogre::IdString(workspaceName + "/Node").getReleaseText();
    Ogre::CompositorNodeDef *node =
        compositor->getNodeDefinitionNonConst(nodeName);
    Ogre::CompositorTargetDef *target = node->getTargetPass(0u);
    const Ogre::CompositorPassDefVec &passes = target->getCompositorPasses();
    if (passes.empty())
      throw std::runtime_error("TSE compositor has no scene pass");
    auto *scenePass = dynamic_cast<Ogre::CompositorPassSceneDef *>(passes[0]);
    if (!scenePass)
      throw std::runtime_error("TSE compositor's first pass is not a scene pass");
    scenePass->mShadowNode = "TSE Sun Shadows";

    mWorkspace = compositor->addWorkspace(
        mSceneManager, mRenderWindow->getTexture(), mCamera->getCamera(),
        workspaceName, true);

    SDL_SetRelativeMouseMode(SDL_TRUE);
  }

  void addNativeWindowHandle(Ogre::NameValuePairList &parameters) {
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(mSdlWindow, &info) != SDL_TRUE)
      throw std::runtime_error("SDL_GetWindowWMInfo failed: " +
                               std::string(SDL_GetError()));

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    if (info.subsystem != SDL_SYSWM_COCOA)
      throw std::runtime_error("OGRE-Next Metal requires an SDL Cocoa window");
    parameters["parentWindowHandle"] =
        Ogre::StringConverter::toString(tse::cocoaContentView(info));
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    if (info.subsystem != SDL_SYSWM_WINDOWS)
      throw std::runtime_error("OGRE-Next D3D11 requires an SDL Win32 window");
    parameters["externalWindowHandle"] = Ogre::StringConverter::toString(
        reinterpret_cast<std::uintptr_t>(info.info.win.window));
#elif defined(SDL_VIDEO_DRIVER_X11)
    if (info.subsystem != SDL_SYSWM_X11)
      throw std::runtime_error("OGRE-Next GL3+ currently requires an SDL X11 window");
    parameters["parentWindowHandle"] =
        Ogre::StringConverter::toString(info.info.x11.window);
#else
#error "No SDL native-window bridge is implemented for this platform"
#endif
  }

  void pumpEvents() {
    Ogre::WindowEventUtilities::messagePump();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        mQuit = true;
        continue;
      }

      if (event.type == SDL_KEYDOWN &&
          event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
        mQuit = true;
      }

      if (event.type == SDL_WINDOWEVENT)
        handleWindowEvent(event.window);

      mCamera->handleEvent(event);
    }
  }

  void handleWindowEvent(const SDL_WindowEvent &event) {
    switch (event.event) {
    case SDL_WINDOWEVENT_SIZE_CHANGED:
    case SDL_WINDOWEVENT_RESIZED:
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      mRenderWindow->requestResolution(static_cast<Ogre::uint32>(event.data1),
                                       static_cast<Ogre::uint32>(event.data2));
#endif
      mRenderWindow->windowMovedOrResized();
      break;
    case SDL_WINDOWEVENT_SHOWN:
      mRenderWindow->_setVisible(true);
      break;
    case SDL_WINDOWEVENT_HIDDEN:
      mRenderWindow->_setVisible(false);
      break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      mRenderWindow->setFocused(true);
      SDL_SetRelativeMouseMode(SDL_TRUE);
      break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      mRenderWindow->setFocused(false);
      break;
    default:
      break;
    }
  }

  void shutdown() {
    if (mRoot && mWorkspace) {
      mRoot->getCompositorManager2()->removeWorkspace(mWorkspace);
      mWorkspace = nullptr;
    }

    mLevel.reset();
    mCamera.reset();

    if (mRoot && mSceneManager) {
      mRoot->destroySceneManager(mSceneManager);
      mSceneManager = nullptr;
    }

    if (mRoot) {
      OGRE_DELETE mRoot;
      mRoot = nullptr;
      mRenderWindow = nullptr;
    }

#if defined(OGRE_STATIC_LIB)
    if (mRenderPlugin) {
      OGRE_DELETE mRenderPlugin;
      mRenderPlugin = nullptr;
    }
#endif

    if (mSdlWindow) {
      SDL_SetWindowFullscreen(mSdlWindow, 0);
      SDL_DestroyWindow(mSdlWindow);
      mSdlWindow = nullptr;
    }

    if (mSdlInitialized) {
      SDL_Quit();
      mSdlInitialized = false;
    }
  }

  SDL_Window *mSdlWindow = nullptr;
  Ogre::Root *mRoot = nullptr;
  Ogre::Window *mRenderWindow = nullptr;
  Ogre::SceneManager *mSceneManager = nullptr;
  Ogre::CompositorWorkspace *mWorkspace = nullptr;
#if defined(OGRE_STATIC_LIB)
  Ogre::Plugin *mRenderPlugin = nullptr;
#endif
  std::unique_ptr<Camera> mCamera;
  std::unique_ptr<Level> mLevel;
  bool mSdlInitialized = false;
  bool mQuit = false;
};
} // namespace

int main() {
  try {
    Application app;
    return app.run();
  } catch (const Ogre::Exception &error) {
    std::cerr << error.getFullDescription() << '\n';
  } catch (const std::exception &error) {
    std::cerr << error.what() << '\n';
  }
  return 1;
}
