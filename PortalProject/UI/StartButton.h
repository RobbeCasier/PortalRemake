#pragma once
#include <UIButton.h>
#include <Components.h>

class StartButton :
    public UIButton
{
public:
    StartButton();
    virtual ~StartButton() = default;

    StartButton(const StartButton& other) = delete;
    StartButton(StartButton&& other) noexcept = delete;
    StartButton& operator=(const StartButton& other) = delete;
    StartButton& operator=(StartButton&& other) noexcept = delete;

    void Initialize(const GameContext& gameContext) override;
    void PostInitialize(const GameContext& gameContext) override;
    void Update(const GameContext& gameContext) override { UNREFERENCED_PARAMETER(gameContext); };

    void Click(const GameContext& gameContext) override;
    bool MouseInShape(const POINT& mousePos) override;
};

