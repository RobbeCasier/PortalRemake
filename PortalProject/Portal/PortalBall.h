#pragma once
#include <GameObject.h>
#include <Components.h>

class PortalBall :
    public GameObject
{
public:
    PortalBall(DirectX::XMFLOAT3 startLocation, DirectX::XMFLOAT3 direction, bool isBluePortal = true);
    virtual ~PortalBall() = default;

    PortalBall(const PortalBall& other) = delete;
    PortalBall(PortalBall&& other) noexcept = delete;
    PortalBall& operator=(const PortalBall& other) = delete;
    PortalBall& operator=(PortalBall&& other) noexcept = delete;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

    DirectX::XMFLOAT3 GetPortalLocation() const { return m_PortalLocation; }
    DirectX::XMFLOAT3 GetPortalDirection() const { return m_PortalDirection; }

    bool CanPlacePortal() const { return m_CanPlacePortal; }
    bool IsBlue() const { return m_IsBluePortal; }
    bool HasHit() const { return m_Hit; }

private:
    void CheckForPlaceableObject(const GameContext& gameContext, const float& addedDistance);

    ParticleEmitterComponent* m_pParticleEmitter = nullptr;
    DirectX::XMFLOAT3 m_StartLocation;
    DirectX::XMFLOAT3 m_Direction;

    DirectX::XMFLOAT3 m_PortalLocation;
    DirectX::XMFLOAT3 m_PortalDirection;

    float m_Speed = 500.f;
    bool m_IsBluePortal;
    bool m_Hit = false;
    bool m_CanPlacePortal = false;
};

