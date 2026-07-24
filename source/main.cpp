#include "camera.hpp"
#include "level.hpp"
#include <OgreApplicationContext.h>
#include <OgreCameraMan.h>
#include <OgreEntity.h>
#include <OgreFrameListener.h>
#include <OgreImGuiInputListener.h>
#include <OgreImGuiOverlay.h>
#include <OgreInput.h>
#include <OgreLight.h>
#include <OgreMath.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreMeshSerializer.h>
#include <OgreMovableObject.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgreOverlaySystem.h>
#include <OgrePrerequisites.h>
#include <OgreRenderTargetListener.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreSubMesh.h>
#include <SDL2/SDL_mouse.h>
#include <SDL_video.h>
#include <imgui.h>
#include <memory>

class Main : public OgreBites::ApplicationContext,
             public OgreBites::InputListener,
             public Ogre::RenderTargetListener {
public:
  Main(void) : OgreBites::ApplicationContext("Main") {}

  bool keyPressed(const OgreBites::KeyboardEvent &evt) override {
    if (evt.keysym.sym == OgreBites::SDLK_ESCAPE)
      getRoot()->queueEndRendering();
    return true;
  }

  void preViewportUpdate(const Ogre::RenderTargetViewportEvent &evt) override {
    if (!evt.source->getOverlaysEnabled())
      return;

    Ogre::ImGuiOverlay::NewFrame();

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground;
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::Begin("FPS", nullptr, flags);
    ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
    ImGui::End();
  }

  void locateResources(void) override {
    OgreBites::ApplicationContext::locateResources();

    auto &rgm = Ogre::ResourceGroupManager::getSingleton();
    rgm.addResourceLocation("assets/materials", "FileSystem", "Game");
    rgm.addResourceLocation("assets/models", "FileSystem", "Game");
    rgm.addResourceLocation("assets/shaders", "FileSystem", "Game");
    std::string mediaPath =
        "/Users/ethan/vcpkg/packages/ogre_arm64-osx/share/ogre/Media";
    rgm.addResourceLocation(mediaPath + "/Main", "FileSystem", "OgreInternal");
    rgm.addResourceLocation(mediaPath + "/RTShaderLib", "FileSystem",
                            "OgreInternal");
  }

  void cookAssets(void) {
    namespace fs = std::filesystem;
    const std::set<std::string> sourceExts = {".gltf", ".glb", ".obj", ".fbx",
                                              ".dae"};
    for (const auto &entry : fs::directory_iterator("assets/models")) {
      if (!entry.is_regular_file())
        continue;

      fs::path src = entry.path();
      std::string ext = src.extension().string();
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      if (!sourceExts.count(ext))
        continue;

      std::string name = src.stem().string();
      fs::path meshOut = "assets/models" / fs::path(name + ".mesh");
      fs::path materialOut = "assets/materials" / fs::path(name + ".material");

      if (fs::exists(meshOut) &&
          fs::last_write_time(meshOut) >= fs::last_write_time(src))
        continue;

      Ogre::LogManager::getSingleton().logMessage("COOK: " + src.string());

      auto mesh = Ogre::MeshManager::getSingleton().load(
          src.filename().string(), "Game");
      try {
        mesh->buildTangentVectors();
      } catch (...) {
      }
      Ogre::MeshSerializer().exportMesh(mesh.get(), meshOut.string());

      if (!fs::exists(materialOut)) {
        Ogre::MaterialSerializer ms;
        std::set<Ogre::String> queued;
        for (auto *sub : mesh->getSubMeshes()) {
          if (!queued.insert(sub->getMaterialName()).second)
            continue;
          auto mat = Ogre::MaterialManager::getSingleton().getByName(
              sub->getMaterialName());
          if (mat)
            ms.queueForExport(mat);
        }
        ms.exportQueued(materialOut.string());
      }
    }
  }

  void setup(void) override {
    OgreBites::ApplicationContext::setup();
    cookAssets();
    addInputListener(this);
    setWindowGrab(true);
    SDL_SetRelativeMouseMode(SDL_TRUE);
    // SDL_SetWindowFullscreen(SDL_GetWindowFromID(1), SDL_WINDOW_FULLSCREEN);

    getRenderWindow()->setVSyncEnabled(false);

    Ogre::Root *root = getRoot();
    Ogre::SceneManager *scnMgr = root->createSceneManager();
    Ogre::RTShader::ShaderGenerator *shaderGen =
        Ogre::RTShader::ShaderGenerator::getSingletonPtr();

    shaderGen->addSceneManager(scnMgr);
    scnMgr->addRenderQueueListener(getOverlaySystem());

    mCamera = std::make_unique<Camera>("MainCamera", scnMgr);

    auto camMan = mCamera->getCameraMan();
    addInputListener(camMan);
    addInputListener(mCamera.get());

    auto *vp = getRenderWindow()->addViewport(mCamera->getCamera());

    shaderGen->getRenderState(Ogre::MSN_SHADERGEN)
        ->addTemplateSubRenderState(shaderGen->createSubRenderState(
            Ogre::RTShader::SRS_SHADOW_MAPPING));
    vp->setMaterialScheme(Ogre::MSN_SHADERGEN);
    Ogre::MaterialManager::getSingleton().setActiveScheme(
        vp->getMaterialScheme());

    level = std::make_unique<Level>(scnMgr, "Level01");

    mImguiOverlay = initialiseImGui();
    mImguiOverlay->setZOrder(300);
    mImguiOverlay->show();

    float vpScale = Ogre::OverlayManager::getSingleton().getPixelRatio();
    ImGui::GetIO().FontGlobalScale = std::round(vpScale * 2);
    addInputListener(getImGuiInputListener());
    getRenderWindow()->addListener(this);
  }

  bool frameRenderingQueued(const Ogre::FrameEvent &evt) override {
    level->update(evt.timeSinceLastFrame);
    mCamera->update(evt.timeSinceLastFrame);
    return OgreBites::ApplicationContext::frameRenderingQueued(evt);
  }

  void shutdown(void) override {
    getRenderWindow()->removeListener(this);
    removeInputListener(getImGuiInputListener());
    Ogre::OverlayManager::getSingleton().destroy("ImGuiOverlay");
    mImguiOverlay = nullptr;

    level.reset();
    removeInputListener(mCamera->getCameraMan());
    removeInputListener(mCamera.get());
    getRenderWindow()->removeAllViewports();
    mCamera.reset();
    OgreBites::ApplicationContext::shutdown();
  }

private:
  std::unique_ptr<Level> level;
  std::unique_ptr<Camera> mCamera;
  Ogre::ImGuiOverlay *mImguiOverlay = nullptr;
};

int main(int argc, char **argv) {
  bool cookOnly = argc > 1 && std::string(argv[1]) == "--cook";
  Main app;
  app.initApp();
  if (!cookOnly)
    app.getRoot()->startRendering();
  app.closeApp();
  return 0;
}
