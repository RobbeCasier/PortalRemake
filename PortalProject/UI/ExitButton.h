#pragma once
#include <UIButton.h>

class ExitButton :
    public UIButton
{
public:
    ExitButton() = default;
    virtual ~ExitButton() = default;

    ExitButton(const ExitButton& other) = delete;
    ExitButton(ExitButton&& other) noexcept = delete;
    ExitButton& operator=(const ExitButton& other) = delete;
    ExitButton& operator=(ExitButton&& other) noexcept = delete;

    void Initialize(const GameContext & gameContext) override;
    void PostInitialize(const GameContext & gameContext) override { UNREFERENCED_PARAMETER(gameContext); };
    void Update(const GameContext & gameContext) override { UNREFERENCED_PARAMETER(gameContext); };

    void Click(const GameContext& gameContext) override;
    bool MouseInShape(const POINT& mousePos) override;
};

