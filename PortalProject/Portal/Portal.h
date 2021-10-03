#pragma once
#include <GameObject.h>
#include <Components.h>

#include "../Character.h"
#include "../Env/FullWall.h"

enum class PortalColor
{
    blue = 0,
    orange = 1
};
class Portal :
    public GameObject
{
public:
    Portal(const DirectX::XMVECTOR& location, const DirectX::XMVECTOR& normal, PortalColor color);
    virtual ~Portal() = default;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

    void SetPortalLocationRotation(const DirectX::XMVECTOR& location, const DirectX::XMVECTOR& rotation, const DirectX::XMVECTOR& playerForward);
    void SetOtherPortal(Portal* pPortal);
    void SetCharacter(Character* pCharacter);

    void SetIsOverlapping(bool isOverlapping = true) { m_IsCharacterOverlapping = isOverlapping; }
    GameObject* GetModel() const { return m_pModel; }
    FixedCamera* GetCamera() const { return m_pCamera; }
    void Reset();

private:
    void BeginOverlap(GameObject* triggerObject, GameObject* otherObject, TriggerAction action);
    void Offset();
    DirectX::XMVECTOR m_Location;
    DirectX::XMVECTOR m_Normal;
    PortalColor m_Color;
    FixedCamera* m_pCamera = nullptr;
    GameObject* m_pModel = nullptr;
    Portal* m_pOtherPortal = nullptr;
    ColliderComponent* m_pCollider = nullptr;
    Character* m_pCharacter = nullptr;
    PortalMapRenderer* m_pPortalMapper = nullptr;

    bool m_IsCharacterOverlapping = false;
    bool m_IsActive = false;
};

