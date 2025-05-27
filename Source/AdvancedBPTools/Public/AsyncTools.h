// Copyright 2025, Wildlight. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AdvBPTypes.h"
#include "AsyncTools.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncTransformTaskOutputPin);

UCLASS()
class UAsyncMoveActorTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    // Completion delegates
    UPROPERTY(BlueprintAssignable)
    FAsyncTransformTaskOutputPin OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FAsyncTransformTaskOutputPin OnFailed;

    /**
     * Moves an actor to specified location
     *
     * @param TargetActor Actor to move
     * @param DesiredLocation Target destination
     * @param Time Time in seconds or units per second (depending on timingMode)
     * @param TimingMode Whether to use duration or velocity for timing
     * @param EasingType Interpolation curve type
     * @param bSweep Whether to sweep for collisions during movement
     */
    UFUNCTION(BlueprintCallable,
        meta = (BlueprintInternalUseOnly = "true",
            WorldContext = "worldContextObject",
            DisplayName = "Move Actor To Location",
            Keywords = "move,location,async,interpolate,animation,duration,velocity,speed"),
        Category = "AdvBPTools|Movement")
    static UAsyncMoveActorTask* MoveActor(
        UObject* worldContextObject,
        AActor* targetActor,
        FVector desiredLocation,
        float time = 1.0f,
        EMoveTimingMode timingMode = EMoveTimingMode::Duration,
        EEasingFunction easingType = EEasingFunction::Linear,
        bool bSweep = false);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;

private:
    // Task parameters
    UPROPERTY()
    AActor* TargetActor;

    UPROPERTY()
    UObject* WorldContextObject;

    UPROPERTY()
    FVector DesiredLocation;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    float ElapsedTime;

    UPROPERTY()
    FVector InitialLocation;

    UPROPERTY()
    EEasingFunction EasingType;

    UPROPERTY()
    bool bSweep;

    // Timer handle for movement updates
    FTimerHandle TimerHandle;

    // Handle task completion
    void HandleTaskComplete(bool bSuccess);

    // Tick handler for movement updates
    UFUNCTION()
    void TickTask();

    // Initialize task with common parameters
    void InitializeTask(
        UObject* worldContextObject,
        AActor* targetActor,
        FVector desiredLocation,
        float duration,
        EEasingFunction easingType,
        bool bSweep);

    // Calculate duration from velocity and distance
    static float CalculateDurationFromVelocity(const FVector& startLocation, const FVector& targetLocation, float velocity);
};

/**
 * Asynchronous task for rotating actors with precise control
 */
UCLASS()
class UAsyncRotateActorTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    // Completion delegates
    UPROPERTY(BlueprintAssignable)
    FAsyncTransformTaskOutputPin OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FAsyncTransformTaskOutputPin OnFailed;

    /**
     * Rotates an actor to specified rotation
     *
     * @param TargetActor Actor to rotate
     * @param DesiredRotation Target rotation
     * @param Time Time in seconds or degrees per second (depending on timingMode)
     * @param TimingMode Whether to use duration or angular velocity for timing
     * @param EasingType Interpolation curve type
     * @param bShortestPath Whether to take the shortest path for rotation
     */
    UFUNCTION(BlueprintCallable,
        meta = (BlueprintInternalUseOnly = "true",
            WorldContext = "worldContextObject",
            DisplayName = "Rotate Actor",
            Keywords = "rotate,rotation,async,interpolate,animation,duration,velocity,speed"),
        Category = "AdvBPTools|Movement")
    static UAsyncRotateActorTask* RotateActor(
        UObject* worldContextObject,
        AActor* targetActor,
        FRotator desiredRotation,
        float time = 1.0f,
        EMoveTimingMode timingMode = EMoveTimingMode::Duration,
        EEasingFunction easingType = EEasingFunction::Linear,
        bool bShortestPath = true);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;

private:
    // Task parameters
    UPROPERTY()
    AActor* TargetActor;

    UPROPERTY()
    UObject* WorldContextObject;

    UPROPERTY()
    FRotator DesiredRotation;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    float ElapsedTime;

    UPROPERTY()
    FRotator InitialRotation;

    // Cached quaternions for efficient interpolation
    FQuat InitialQuat;
    FQuat TargetQuat;

    UPROPERTY()
    EEasingFunction EasingType;

    UPROPERTY()
    bool bShortestPath;

    // Timer handle for rotation updates
    FTimerHandle TimerHandle;

    // Handle task completion
    void HandleTaskComplete(bool bSuccess);

    // Tick handler for rotation updates
    UFUNCTION()
    void TickTask();

    // Initialize task with common parameters
    void InitializeTask(
        UObject* worldContextObject,
        AActor* targetActor,
        FRotator desiredRotation,
        float duration,
        EEasingFunction easingType,
        bool bShortestPath);

    // Calculate duration from angular velocity
    static float CalculateDurationFromAngularVelocity(const FRotator& startRotation, const FRotator& targetRotation, float degreesPerSecond);
};

/**
 * Asynchronous task for scaling actors with precise control
 */
UCLASS()
class UAsyncScaleActorTask : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()

public:
    // Completion delegates
    UPROPERTY(BlueprintAssignable)
    FAsyncTransformTaskOutputPin OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FAsyncTransformTaskOutputPin OnFailed;

    /**
     * Scales an actor to specified scale
     *
     * @param TargetActor Actor to scale
     * @param DesiredScale Target scale
     * @param Duration Time to complete the scaling
     * @param EasingType Interpolation curve type
     */
    UFUNCTION(BlueprintCallable,
        meta = (BlueprintInternalUseOnly = "true",
            WorldContext = "worldContextObject",
            DisplayName = "Scale Actor",
            Keywords = "scale,size,async,interpolate,animation"),
        Category = "AdvBPTools|Movement")
    static UAsyncScaleActorTask* ScaleActor(
        UObject* worldContextObject,
        AActor* targetActor,
        FVector desiredScale,
        float duration = 1.0f,
        EEasingFunction easingType = EEasingFunction::Linear);

    // UBlueprintAsyncActionBase interface
    virtual void Activate() override;

private:
    // Task parameters
    UPROPERTY()
    AActor* TargetActor;

    UPROPERTY()
    UObject* WorldContextObject;

    UPROPERTY()
    FVector DesiredScale;

    UPROPERTY()
    float Duration;

    UPROPERTY()
    float ElapsedTime;

    UPROPERTY()
    FVector InitialScale;

    UPROPERTY()
    EEasingFunction EasingType;

    // Timer handle for scaling updates
    FTimerHandle TimerHandle;

    // Handle task completion
    void HandleTaskComplete(bool bSuccess);

    // Tick handler for scaling updates
    UFUNCTION()
    void TickTask();

    // Initialize task with common parameters
    void InitializeTask(
        UObject* worldContextObject,
        AActor* targetActor,
        FVector desiredScale,
        float duration,
        EEasingFunction easingType);
};