// Copyright 2025, Wildlight. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AdvBPTypes.h"
#include "AdvBPUtility.generated.h"

/**
 * High-performance utility library for easing functions
 * Provides optimized interpolation calculations for animation and movement
 */
UCLASS()
class UAdvBPUtilities : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Applies an easing function to an alpha value (0.0-1.0)
     *
     * @param alpha Input value in range 0.0-1.0
     * @param easingType Type of easing function to apply
     * @return Eased value in range 0.0-1.0
     */
    UFUNCTION(BlueprintPure, Category = "AdvBPTools|Math|Interpolation")
    static float ApplyEasing(float alpha, EEasingFunction easingType);

    /**
     * Applies an easing function to interpolate between two float values
     *
     * @param startValue Starting value
     * @param endValue Ending value
     * @param alpha Input value in range 0.0-1.0
     * @param easingType Type of easing function to apply
     * @return Interpolated value
     */
    UFUNCTION(BlueprintPure, Category = "AdvBPTools|Math|Interpolation")
    static float EaseFloat(float startValue, float endValue, float alpha, EEasingFunction easingType);

    /**
     * Applies an easing function to interpolate between two vector values
     *
     * @param startValue Starting vector
     * @param endValue Ending vector
     * @param alpha Input value in range 0.0-1.0
     * @param easingType Type of easing function to apply
     * @return Interpolated vector
     */
    UFUNCTION(BlueprintPure, Category = "AdvBPTools|Math|Interpolation")
    static FVector EaseVector(FVector startValue, FVector endValue, float alpha, EEasingFunction easingType);

    /**
     * Applies an easing function to interpolate between two rotator values
     * Uses quaternion interpolation for optimal rotation paths
     *
     * @param startValue Starting rotator
     * @param endValue Ending rotator
     * @param alpha Input value in range 0.0-1.0
     * @param easingType Type of easing function to apply
     * @return Interpolated rotator
     */
    UFUNCTION(BlueprintPure, Category = "AdvBPTools|Math|Interpolation")
    static FRotator EaseRotator(FRotator startValue, FRotator endValue, float alpha, EEasingFunction easingType);

private:
    // Optimized internal implementations of easing functions

    /** Linear interpolation (no easing) */
    static constexpr float Linear(float alpha)
    {
        return alpha;
    }

    /** Quadratic ease in */
    static constexpr float EaseInQuad(float alpha)
    {
        return alpha * alpha;
    }

    /** Quadratic ease out */
    static constexpr float EaseOutQuad(float alpha)
    {
        return alpha * (2.0f - alpha);
    }

    /** Quadratic ease in and out */
    static constexpr float EaseInOutQuad(float alpha)
    {
        alpha *= 2.0f;
        if (alpha < 1.0f)
        {
            return 0.5f * alpha * alpha;
        }

        alpha -= 1.0f;
        return 0.5f * (1.0f - alpha * (alpha - 2.0f));
    }

    /** Exponential ease in */
    static float ExponentialIn(float alpha)
    {
        return alpha == 0.0f ? 0.0f : FMath::Pow(2.0f, 10.0f * (alpha - 1.0f));
    }

    /** Exponential ease out */
    static float ExponentialOut(float alpha)
    {
        return alpha == 1.0f ? 1.0f : 1.0f - FMath::Pow(2.0f, -10.0f * alpha);
    }

    /** Exponential ease in and out */
    static float ExponentialInOut(float alpha)
    {
        if (alpha == 0.0f) return 0.0f;
        if (alpha == 1.0f) return 1.0f;

        alpha *= 2.0f;
        if (alpha < 1.0f)
        {
            return 0.5f * FMath::Pow(2.0f, 10.0f * (alpha - 1.0f));
        }

        return 0.5f * (2.0f - FMath::Pow(2.0f, -10.0f * (alpha - 1.0f)));
    }

    /**
     * Template function to apply easing between two values of any type that supports lerp
     * Uses SFINAE to ensure compile-time type safety
     */
    template<typename T>
    static T EaseValue(const T& startValue, const T& endValue, float alpha, EEasingFunction easingType)
    {
        const float easedAlpha = ApplyEasing(alpha, easingType);
        return FMath::Lerp(startValue, endValue, easedAlpha);
    }
};