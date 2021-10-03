#pragma once
#include <GameObject.h>
#include <Components.h>

class Button;

class Door :
    public GameObject
{
public:
    Door(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation);
    virtual ~Door() = default;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

    void AddButton(Button* pButton);

private:
    void OpenDoor(bool open);

    GameObject* m_pLeftDoor = nullptr;
    GameObject* m_pRightDoor = nullptr;

    DirectX::XMFLOAT3 m_Position;
    DirectX::XMFLOAT3 m_Rotation;

    physx::PxVec3 m_EndPositionRight;
    physx::PxVec3 m_EndPositionLeft;

    FMOD::System* m_pAudioSystem = nullptr;
    FMOD::Sound* m_pOpen = nullptr;
    FMOD::Sound* m_pClose = nullptr;

    std::vector<Button*> m_Buttons;

    float m_Speed = 20.f;

    float m_DisplacementDoor = 6.0f;
    bool m_IsOpen = false;
};

