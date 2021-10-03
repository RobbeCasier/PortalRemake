#pragma once
#include <GameObject.h>
#include <Components.h>

class Button :
    public GameObject
{
public:
    Button(const DirectX::XMFLOAT3& position);
    virtual ~Button() = default;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

    bool IsActive() const { return m_IsActive; }

private:
    void Overlap(GameObject* triggerObject, GameObject* otherObject, TriggerAction action);
    ColliderComponent* m_pTrigger = nullptr;
    ModelComponent* m_pModel = nullptr;
    DirectX::XMFLOAT3 m_Position;

    FMOD::System* m_pAudioSystem = nullptr;
    FMOD::Sound* m_pButtonPressed = nullptr;

    unsigned int m_ActiveCount = 0;
    bool m_IsActive = false;
};

