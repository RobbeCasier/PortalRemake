#pragma once
#include <GameObject.h>
#include <Components.h>
#include <Prefabs.h>


class MovableBox :
    public GameObject
{
public:
    MovableBox(DirectX::XMFLOAT3 startLocation);
    virtual ~MovableBox() = default;

    MovableBox(const MovableBox& other) = delete;
    MovableBox(MovableBox&& other) noexcept = delete;
    MovableBox& operator=(const MovableBox& other) = delete;
    MovableBox& operator=(MovableBox&& other) noexcept = delete;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override;

    void SetStartLocation(const float& x, const float& y, const float& z);
    void Reset() { GetTransform()->Translate(m_StartLocation); }
private:
    DirectX::XMFLOAT3 m_StartLocation;
};

