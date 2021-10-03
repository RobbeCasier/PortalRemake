#pragma once
#include <GameScene.h>

class Character;
class Portal;
class Button;
class Door;
class UIButton;
class MovableBox;
class PortalBall;
class SpriteComponent;
class PostVignette;

enum class PortalInput
{
    Pause = 10,
    PickUpObject = 11,
    Portal1 = 12,
    Portal2 = 13,
    Continue
};
class PortalScene final :
    public GameScene
{
public:
    PortalScene();
    virtual ~PortalScene();

    PortalScene(const PortalScene& other) = delete;
    PortalScene(PortalScene&& other) noexcept = delete;
    PortalScene& operator=(const PortalScene& other) = delete;
    PortalScene& operator=(PortalScene&& other)noexcept = delete;

protected:
    void Initialize() override;
    void Update() override;
    void Draw() override;

    void SceneActivated();

private:
    void UpdateGame();
    void UpdateGameState();

    void PlacePortal(Portal** portalToPlace, Portal** connectionPortal, PortalBall* portalBall);

    void BeginOverlap(GameObject* triggerObject, GameObject* otherObject, GameObject::TriggerAction action);

    void ResetScene();
    Character* m_pCharacter = nullptr;

    std::vector<MovableBox*> m_Boxs;

    Portal* m_pBluePortal = nullptr;
    Portal* m_pOrangePortal = nullptr;

    PortalBall* m_pBlueBall = nullptr;
    PortalBall* m_pOrangeBall = nullptr;

    FMOD::System* m_pAudioSystem = nullptr;
    FMOD::Sound* m_pPortalGunSound = nullptr;
    FMOD::Sound* m_pAmbient = nullptr;
    FMOD::Channel* m_pAmbientChannel = nullptr;

    void* m_pBluePortalBuffer = nullptr;
    void* m_pOrangePortalBuffer = nullptr;

    GameObject* m_pEndPoint = nullptr;

    SpriteComponent* m_pCrossHair = nullptr;

    PostVignette* m_pVignette = nullptr;

    std::vector<UIButton*> m_Buttons;

    bool m_GoToMain = false;
    bool m_IsResetting = false;
    unsigned int m_ResetCount = 0;
};

