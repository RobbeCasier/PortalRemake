#pragma once
#include <GameObject.h>
#include <Components.h>

class Floor :
    public GameObject
{
public:
    Floor(const DirectX::XMFLOAT3& position);
    virtual ~Floor() = default;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

private:
    DirectX::XMFLOAT3 m_StartLocation;
};

