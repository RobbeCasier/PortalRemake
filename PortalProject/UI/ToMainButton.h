#pragma once
#include <UIButton.h>
class ToMainButton :
    public UIButton
{
public:
    ToMainButton() = default;
    virtual ~ToMainButton() = default;

    ToMainButton(const ToMainButton& other) = delete;
    ToMainButton(ToMainButton&& other) noexcept = delete;
    ToMainButton& operator=(const ToMainButton& other) = delete;
    ToMainButton& operator=(ToMainButton&& other) noexcept = delete;

    void Initialize(const GameContext & gameContext) override;
    void PostInitialize(const GameContext & gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
    void Update(const GameContext & gameContext) override { UNREFERENCED_PARAMETER(gameContext); };

    void Click(const GameContext & gameContext) override;
    bool MouseInShape(const POINT & mousePos) override;
};

