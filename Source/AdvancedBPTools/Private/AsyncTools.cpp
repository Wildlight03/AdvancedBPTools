// Copyright 2025, Wildlight. All Rights Reserved.

#include "AsyncTools.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AdvBPUtility.h"

//
// UAsyncMoveActorTask Implementation
//

UAsyncMoveActorTask* UAsyncMoveActorTask::MoveActor(
    UObject* worldContextObject,
    AActor* targetActor,
    FVector desiredLocation,
    float time,
    EMoveTimingMode timingMode,
    EEasingFunction easingType,
    bool bSweep)
{
    // Create task instance
    UAsyncMoveActorTask* TaskInstance = NewObject<UAsyncMoveActorTask>();

    // Early validation
    if (!IsValid(targetActor) || time <= KINDA_SMALL_NUMBER)
    {
        // Initialize with minimal duration for immediate completion
        TaskInstance->InitializeTask(
            worldContextObject,
            targetActor,
            desiredLocation,
            0.001f, // Minimal duration
            easingType,
            bSweep);

        return TaskInstance;
    }

    // Handle duration calculation based on timing mode
    float EffectiveDuration = 0.0f;

    switch (timingMode)
    {
    case EMoveTimingMode::Duration:
        EffectiveDuration = FMath::Max(0.001f, time);
        break;

    case EMoveTimingMode::Velocity:
    {
        // Calculate appropriate duration based on distance and velocity
        const FVector StartLocation = targetActor->GetActorLocation();
        EffectiveDuration = CalculateDurationFromVelocity(
            StartLocation,
            desiredLocation,
            time);
    }
    break;

    default:
        // Fallback to direct duration if enum is invalid
        EffectiveDuration = FMath::Max(0.001f, time);
        break;
    }

    // Initialize with calculated duration
    TaskInstance->InitializeTask(
        worldContextObject,
        targetActor,
        desiredLocation,
        EffectiveDuration,
        easingType,
        bSweep);

    return TaskInstance;
}

void UAsyncMoveActorTask::InitializeTask(
    UObject* worldContextObject,
    AActor* targetActor,
    FVector desiredLocation,
    float duration,
    EEasingFunction easingType,
    bool bSweeps)
{
    // Store parameters
    WorldContextObject = worldContextObject;
    TargetActor = targetActor;
    DesiredLocation = desiredLocation;
    Duration = duration;
    EasingType = easingType;
    bSweep = bSweeps;
    ElapsedTime = 0.0f;
}

float UAsyncMoveActorTask::CalculateDurationFromVelocity(
    const FVector& startLocation,
    const FVector& targetLocation,
    float velocity)
{
    // Calculate distance between points using optimized SIMD operation
    const float Distance = FVector::Distance(startLocation, targetLocation);

    // Handle edge cases
    if (Distance <= KINDA_SMALL_NUMBER || velocity <= KINDA_SMALL_NUMBER)
    {
        return 0.001f; // Return minimal duration for immediate completion
    }

    // Calculate duration from distance and velocity
    return Distance / velocity;
}

void UAsyncMoveActorTask::Activate()
{
    // Parent class implementation
    Super::Activate();

    // Early validation
    if (!IsValid(TargetActor))
    {
        HandleTaskComplete(false);
        return;
    }

    // Handle zero duration cases immediately
    if (FMath::IsNearlyZero(Duration))
    {
        bool bSuccess = TargetActor->SetActorLocation(DesiredLocation, bSweep);
        HandleTaskComplete(bSuccess);
        return;
    }

    // Initialize movement
    InitialLocation = TargetActor->GetActorLocation();
    ElapsedTime = 0.0f;

    // Setup timer for movement updates
    UWorld* World = TargetActor ? TargetActor->GetWorld() : nullptr;
    if (World)
    {
        // High frequency timer for smooth movement
        World->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &UAsyncMoveActorTask::TickTask,
            1.0f / 60.0f, // 60 FPS update rate
            true
        );
    }
    else
    {
        HandleTaskComplete(false);
    }
}

void UAsyncMoveActorTask::TickTask()
{
    // Validate actor is still valid
    if (!IsValid(TargetActor))
    {
        HandleTaskComplete(false);
        return;
    }

    // Update elapsed time with optimized world time retrieval
    if (UWorld* World = TargetActor->GetWorld())
    {
        ElapsedTime += World->GetDeltaSeconds();
    }

    // Calculate interpolation alpha with bounds checking
    const float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

    // Apply easing function
    const float EasedAlpha = UAdvBPUtilities::ApplyEasing(Alpha, EasingType);

    // Calculate new position using SIMD-optimized lerp
    const FVector NewLocation = FMath::Lerp(InitialLocation, DesiredLocation, EasedAlpha);

    // Update actor position
    bool bSuccess = TargetActor->SetActorLocation(NewLocation, bSweep);

    // Check for completion
    if (ElapsedTime >= Duration)
    {
        HandleTaskComplete(bSuccess);
    }
}

void UAsyncMoveActorTask::HandleTaskComplete(bool bSuccess)
{
    // Clear the timer
    if (UWorld* World = TargetActor ? TargetActor->GetWorld() : nullptr)
    {
        World->GetTimerManager().ClearTimer(TimerHandle);
    }

    // Broadcast appropriate completion delegate
    if (bSuccess)
    {
        OnSuccess.Broadcast();
    }
    else
    {
        OnFailed.Broadcast();
    }

    // Mark the async action as complete
    SetReadyToDestroy();
}

//
// UAsyncRotateActorTask Implementation
//

UAsyncRotateActorTask* UAsyncRotateActorTask::RotateActor(
    UObject* worldContextObject,
    AActor* targetActor,
    FRotator desiredRotation,
    float time,
    EMoveTimingMode timingMode,
    EEasingFunction easingType,
    bool bShortestPath)
{
    // Create task instance
    UAsyncRotateActorTask* TaskInstance = NewObject<UAsyncRotateActorTask>();

    // Early validation
    if (!IsValid(targetActor) || time <= KINDA_SMALL_NUMBER)
    {
        // Initialize with minimal duration for immediate completion
        TaskInstance->InitializeTask(
            worldContextObject,
            targetActor,
            desiredRotation,
            0.001f, // Minimal duration
            easingType,
            bShortestPath);

        return TaskInstance;
    }

    // Handle duration calculation based on timing mode
    float EffectiveDuration = 0.0f;

    switch (timingMode)
    {
    case EMoveTimingMode::Duration:
        EffectiveDuration = FMath::Max(0.001f, time);
        break;

    case EMoveTimingMode::Velocity:
    {
        // Calculate appropriate duration based on angular distance and velocity
        const FRotator StartRotation = targetActor->GetActorRotation();
        EffectiveDuration = CalculateDurationFromAngularVelocity(
            StartRotation,
            desiredRotation,
            time);
    }
    break;

    default:
        // Fallback to direct duration if enum is invalid
        EffectiveDuration = FMath::Max(0.001f, time);
        break;
    }

    // Initialize with calculated duration
    TaskInstance->InitializeTask(
        worldContextObject,
        targetActor,
        desiredRotation,
        EffectiveDuration,
        easingType,
        bShortestPath);

    return TaskInstance;
}

float UAsyncRotateActorTask::CalculateDurationFromAngularVelocity(
    const FRotator& startRotation,
    const FRotator& targetRotation,
    float degreesPerSecond)
{
    // Early validation
    if (degreesPerSecond <= KINDA_SMALL_NUMBER)
    {
        return 0.001f; // Return minimal duration for immediate completion
    }

    // Convert to quaternions for most accurate angular distance calculation
    const FQuat StartQuat = startRotation.Quaternion();
    const FQuat TargetQuat = targetRotation.Quaternion();

    // Calculate angular distance in radians
    float AngularDistance = 0.0f;

    if (StartQuat.Equals(TargetQuat, KINDA_SMALL_NUMBER))
    {
        return 0.001f; // Return minimal duration for immediate completion
    }

    // Get the angular distance in radians
    AngularDistance = StartQuat.AngularDistance(TargetQuat);

    // Convert to degrees and calculate duration
    const float AngularDistanceDegrees = FMath::RadiansToDegrees(AngularDistance);

    // Calculate duration based on angular velocity
    return AngularDistanceDegrees / degreesPerSecond;
}

void UAsyncRotateActorTask::InitializeTask(
    UObject* worldContextObject,
    AActor* targetActor,
    FRotator desiredRotation,
    float duration,
    EEasingFunction easingType,
    bool shortestPath)
{
    // Store parameters
    WorldContextObject = worldContextObject;
    TargetActor = targetActor;
    DesiredRotation = desiredRotation;
    Duration = duration;
    EasingType = easingType;
    bShortestPath = shortestPath;
    ElapsedTime = 0.0f;
}

void UAsyncRotateActorTask::Activate()
{
    // Parent class implementation
    Super::Activate();

    // Early validation
    if (!IsValid(TargetActor))
    {
        HandleTaskComplete(false);
        return;
    }

    // Handle zero duration cases immediately
    if (FMath::IsNearlyZero(Duration))
    {
        bool bSuccess = TargetActor->SetActorRotation(DesiredRotation);
        HandleTaskComplete(bSuccess);
        return;
    }

    // Initialize rotation and cache quaternions for interpolation
    InitialRotation = TargetActor->GetActorRotation();
    InitialQuat = InitialRotation.Quaternion();
    TargetQuat = DesiredRotation.Quaternion();

    // Ensure we're using the shortest path if requested
    if (bShortestPath && !FMath::IsNearlyEqual(InitialQuat | TargetQuat, 0.0f, KINDA_SMALL_NUMBER))
    {
        if ((InitialQuat | TargetQuat) < 0.0f)
        {
            TargetQuat = -TargetQuat;
        }
    }

    ElapsedTime = 0.0f;

    // Setup timer for rotation updates
    UWorld* World = TargetActor ? TargetActor->GetWorld() : nullptr;
    if (World)
    {
        // High frequency timer for smooth rotation
        World->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &UAsyncRotateActorTask::TickTask,
            1.0f / 60.0f, // 60 FPS update rate
            true
        );
    }
    else
    {
        HandleTaskComplete(false);
    }
}

void UAsyncRotateActorTask::TickTask()
{
    // Validate actor is still valid
    if (!IsValid(TargetActor))
    {
        HandleTaskComplete(false);
        return;
    }

    // Update elapsed time with optimized world time retrieval
    if (UWorld* World = TargetActor->GetWorld())
    {
        ElapsedTime += World->GetDeltaSeconds();
    }

    // Calculate interpolation alpha with bounds checking
    const float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

    // Apply easing function
    const float EasedAlpha = UAdvBPUtilities::ApplyEasing(Alpha, EasingType);

    // Calculate new rotation using quaternion Slerp for optimal path
    const FQuat NewQuat = FQuat::Slerp(InitialQuat, TargetQuat, EasedAlpha);

    // Update actor rotation
    bool bSuccess = TargetActor->SetActorRotation(NewQuat);

    // Check for completion
    if (ElapsedTime >= Duration)
    {
        HandleTaskComplete(bSuccess);
    }
}

void UAsyncRotateActorTask::HandleTaskComplete(bool bSuccess)
{
    // Clear the timer
    if (UWorld* World = TargetActor ? TargetActor->GetWorld() : nullptr)
    {
        World->GetTimerManager().ClearTimer(TimerHandle);
    }

    // Broadcast appropriate completion delegate
    if (bSuccess)
    {
        OnSuccess.Broadcast();
    }
    else
    {
        OnFailed.Broadcast();
    }

    // Mark the async action as complete
    SetReadyToDestroy();
}

//
// UAsyncScaleActorTask Implementation
//

UAsyncScaleActorTask* UAsyncScaleActorTask::ScaleActor(
    UObject* worldContextObject,
    AActor* targetActor,
    FVector desiredScale,
    float duration,
    EEasingFunction easingType)
{
    // Create task instance
    UAsyncScaleActorTask* TaskInstance = NewObject<UAsyncScaleActorTask>();

    // Early validation
    if (!IsValid(targetActor) || duration <= KINDA_SMALL_NUMBER)
    {
        // Initialize with minimal duration for immediate completion
        TaskInstance->InitializeTask(
            worldContextObject,
            targetActor,
            desiredScale,
            0.001f, // Minimal duration
            easingType);

        return TaskInstance;
    }

    // Initialize with effective duration
    float EffectiveDuration = FMath::Max(0.001f, duration);

    TaskInstance->InitializeTask(
        worldContextObject,
        targetActor,
        desiredScale,
        EffectiveDuration,
        easingType);

    return TaskInstance;
}

void UAsyncScaleActorTask::InitializeTask(
    UObject* worldContextObject,
    AActor* targetActor,
    FVector desiredScale,
    float duration,
    EEasingFunction easingType)
{
    // Store parameters
    WorldContextObject = worldContextObject;
    TargetActor = targetActor;
    DesiredScale = desiredScale;
    Duration = duration;
    EasingType = easingType;
    ElapsedTime = 0.0f;
}

void UAsyncScaleActorTask::Activate()
{
    // Parent class implementation
    Super::Activate();

    // Early validation
    if (!IsValid(TargetActor))
    {
        HandleTaskComplete(false);
        return;
    }

    // Handle zero duration cases immediately
    if (FMath::IsNearlyZero(Duration))
    {
        TargetActor->SetActorScale3D(DesiredScale);
        HandleTaskComplete(true);
        return;
    }

    // Initialize scale
    InitialScale = TargetActor->GetActorScale3D();
    ElapsedTime = 0.0f;

    // Setup timer for scale updates
    UWorld* World = TargetActor ? TargetActor->GetWorld() : nullptr;
    if (World)
    {
        // High frequency timer for smooth scaling
        World->GetTimerManager().SetTimer(
            TimerHandle,
            this,
            &UAsyncScaleActorTask::TickTask,
            1.0f / 60.0f, // 60 FPS update rate
            true
        );
    }
    else
    {
        HandleTaskComplete(false);
    }
}

void UAsyncScaleActorTask::TickTask()
{
    // Validate actor is still valid
    if (!IsValid(TargetActor))
    {
        HandleTaskComplete(false);
        return;
    }

    // Update elapsed time with optimized world time retrieval
    if (UWorld* World = TargetActor->GetWorld())
    {
        ElapsedTime += World->GetDeltaSeconds();
    }

    // Calculate interpolation alpha with bounds checking
    const float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

    // Apply easing function
    const float EasedAlpha = UAdvBPUtilities::ApplyEasing(Alpha, EasingType);

    // Calculate new scale using SIMD-optimized lerp
    const FVector NewScale = FMath::Lerp(InitialScale, DesiredScale, EasedAlpha);

    // Update actor scale
    TargetActor->SetActorScale3D(NewScale);

    // Check for completion
    if (ElapsedTime >= Duration)
    {
        HandleTaskComplete(true);
    }
}

void UAsyncScaleActorTask::HandleTaskComplete(bool bSuccess)
{
    // Clear the timer
    if (UWorld* World = TargetActor ? TargetActor->GetWorld() : nullptr)
    {
        World->GetTimerManager().ClearTimer(TimerHandle);
    }

    // Broadcast appropriate completion delegate
    if (bSuccess)
    {
        OnSuccess.Broadcast();
    }
    else
    {
        OnFailed.Broadcast();
    }

    // Mark the async action as complete
    SetReadyToDestroy();
}