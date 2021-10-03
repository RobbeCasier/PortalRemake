#pragma once
#include <UIButton.h>
class RestartButton :
    public UIButton
{
public:
    RestartButton() = default;
    virtual ~RestartButton() = default;

    RestartButton(const RestartButton& other) = delete;
    RestartButton(RestartButton&& other) noexcept = delete;
    RestartButton& operator=(const RestartButton& other) = delete;
    RestartButton& operator=(RestartButton&& other) noexcept = delete;

    void Initialize(const GameContext & gameContext) override;
    void PostInitialize(const GameContext & gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
    void Update(const GameContext & gameContext) override { UNREFERENCED_PARAMETER(gameContext); };

    void Click(const GameContext & gameContext) override;
    bool MouseInShape(const POINT & mousePos) override;
};

