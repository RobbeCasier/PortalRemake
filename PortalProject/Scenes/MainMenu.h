#pragma once
#include <GameScene.h>

class UIButton;
enum class MenuInput
{
    LeftClick
};
class MainMenu :
    public GameScene
{
public:
    MainMenu();
    virtual ~MainMenu();

    MainMenu(const MainMenu& other) = delete;
    MainMenu(MainMenu&& other) noexcept = delete;
    MainMenu& operator=(const MainMenu& other) = delete;
    MainMenu& operator=(MainMenu&& other)noexcept = delete;

protected:
    void Initialize() override;
    void Update() override;
    void Draw() override;

    void SceneActivated();

private:
    std::vector<UIButton*> m_Buttons;

    FMOD::System* m_pAudioSystem = nullptr;
    FMOD::Sound* m_pAmbientMenu = nullptr;
    FMOD::Channel* m_pAmbientChannel = nullptr;

    GameScene* m_pPortalScene = nullptr;
};

