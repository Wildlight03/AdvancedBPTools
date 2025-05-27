// Copyright 2025, Wildlight. All Rights Reserved.

#include "AdvBPUtility.h"

float UAdvBPUtilities::ApplyEasing(float alpha, EEasingFunction easingType)
{
    // Clamp alpha to valid range
    const float clampedAlpha = FMath::Clamp(alpha, 0.0f, 1.0f);

    // Apply the appropriate easing function
    switch (easingType)
    {
    case EEasingFunction::Linear:
        return Linear(clampedAlpha);

    case EEasingFunction::EaseIn:
        return EaseInQuad(clampedAlpha);

    case EEasingFunction::EaseOut:
        return EaseOutQuad(clampedAlpha);

    case EEasingFunction::EaseInOut:
        return EaseInOutQuad(clampedAlpha);

    case EEasingFunction::ExpoIn:
        return ExponentialIn(clampedAlpha);

    case EEasingFunction::ExpoOut:
        return ExponentialOut(clampedAlpha);

    case EEasingFunction::ExpoInOut:
        return ExponentialInOut(clampedAlpha);

    default:
        // Fallback to linear if unknown type
        return clampedAlpha;
    }
}

float UAdvBPUtilities::EaseFloat(float startValue, float endValue, float alpha, EEasingFunction easingType)
{
    return EaseValue<float>(startValue, endValue, alpha, easingType);
}

FVector UAdvBPUtilities::EaseVector(FVector startValue, FVector endValue, float alpha, EEasingFunction easingType)
{
    return EaseValue<FVector>(startValue, endValue, alpha, easingType);
}

FRotator UAdvBPUtilities::EaseRotator(FRotator startValue, FRotator endValue, float alpha, EEasingFunction easingType)
{
    // Convert rotators to quaternions for optimal interpolation
    const FQuat startQuat = startValue.Quaternion();
    const FQuat endQuat = endValue.Quaternion();

    // Apply easing to alpha
    const float easedAlpha = ApplyEasing(alpha, easingType);

    // Use spherical interpolation for quaternions
    const FQuat resultQuat = FQuat::Slerp(startQuat, endQuat, easedAlpha);

    // Convert back to rotator
    return resultQuat.Rotator();
}