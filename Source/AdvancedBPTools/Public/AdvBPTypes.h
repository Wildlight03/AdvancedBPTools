#pragma once


UENUM(BlueprintType)
enum class EEasingFunction : uint8
{
    Linear      UMETA(DisplayName = "Linear"),
    EaseIn      UMETA(DisplayName = "Ease In (Quadratic)"),
    EaseOut     UMETA(DisplayName = "Ease Out (Quadratic)"),
    EaseInOut   UMETA(DisplayName = "Ease In Out (Quadratic)"),
    ExpoIn      UMETA(DisplayName = "Exponential In"),
    ExpoOut     UMETA(DisplayName = "Exponential Out"),
    ExpoInOut   UMETA(DisplayName = "Exponential In Out")
};

UENUM(BlueprintType)
enum class EMoveTimingMode : uint8
{
    Duration UMETA(DisplayName = "Duration-Based", ToolTip = "Specify how long the movement should take"),
    Velocity UMETA(DisplayName = "Velocity-Based", ToolTip = "Specify how fast the actor should move")
};

UENUM(BlueprintType)
enum class EThreadingType : uint8
{
    GameThread UMETA(DisplayName = "Game Thread", ToolTip = "Runs asyncronsly in the game thread"),
    HighPrio UMETA(DisplayName = "HiPrioThread", ToolTip = "Runs asyncronsly on any high priority background thread"),
    NormalPrio UMETA(DisplayName = "NormalThread", ToolTip = "Runs asyncronsly on any normal priority background thread")
};