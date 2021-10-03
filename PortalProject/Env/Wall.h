#pragma once
#include <GameObject.h>
#include <Components.h>

class Wall :
    public GameObject
{
public:
    Wall(const DirectX::XMFLOAT3& position);
    virtual ~Wall() = default;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

private:
    DirectX::XMFLOAT3 m_StartLocation;
};

