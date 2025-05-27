// Copyright 2025, Wildlight. All Rights Reserved.

#pragma once



/**
 * Enum representing the result of a latent action
 */


/**
 * Enum defining various easing functions for interpolation
 */
//UENUM(BlueprintType)
//enum class EEasingFunction : uint8
//{
//    Linear      UMETA(DisplayName = "Linear"),
//    EaseIn      UMETA(DisplayName = "Ease In (Quadratic)"),
//    EaseOut     UMETA(DisplayName = "Ease Out (Quadratic)"),
//    EaseInOut   UMETA(DisplayName = "Ease In Out (Quadratic)"),
//    ExpoIn      UMETA(DisplayName = "Exponential In"),
//    ExpoOut     UMETA(DisplayName = "Exponential Out"),
//    ExpoInOut   UMETA(DisplayName = "Exponential In Out")
//};

///**
// * Generic base class for implementing latent actions with various easing functions
// * Optimized for minimal memory footprint and fast execution
// */
//template<typename T>
//class TGenericLatentAction : public FPendingLatentAction
//{
//public:
//    TGenericLatentAction(const FLatentActionInfo& latentInfo, float duration, EEasingFunction easingType, const FOnLatentActionStarted& onStarted)
//        : ExecutionFunction(latentInfo.ExecutionFunction)
//        , OutputLink(latentInfo.Linkage)
//        , CallbackTarget(latentInfo.CallbackTarget)
//        , Duration(FMath::Max(0.001f, duration))
//        , ElapsedTime(0.0f)
//        , EasingType(easingType)
//        , OperationResult(ELatentActionResult::Success)
//        , OnStarted(onStarted)
//    {
//        // Immediately execute the OnStarted delegate if provided
//        if (OnStarted.IsBound())
//        {
//            OnStarted.Execute();
//        }
//    }
//
//    virtual ~TGenericLatentAction() = default;
//
//    // Static function for easing calculations to avoid code duplication
//    [[nodiscard]] static constexpr float CalculateAlpha(float rawAlpha, EEasingFunction easingType)
//    {
//        switch (easingType)
//        {
//        case EEasingFunction::Linear:
//            return rawAlpha;
//
//        case EEasingFunction::EaseIn:
//            return rawAlpha * rawAlpha;
//
//        case EEasingFunction::EaseOut:
//            return 1.0f - FMath::Square(1.0f - rawAlpha);
//
//        case EEasingFunction::EaseInOut:
//            return rawAlpha < 0.5f
//                ? 2.0f * rawAlpha * rawAlpha
//                : 1.0f - FMath::Pow(-2.0f * rawAlpha + 2.0f, 2.0f) * 0.5f;
//
//        case EEasingFunction::ExpoIn:
//            return rawAlpha == 0.0f ? 0.0f : FMath::Pow(2.0f, 10.0f * rawAlpha - 10.0f);
//
//        case EEasingFunction::ExpoOut:
//            return rawAlpha == 1.0f ? 1.0f : 1.0f - FMath::Pow(2.0f, -10.0f * rawAlpha);
//
//        case EEasingFunction::ExpoInOut:
//            if (rawAlpha == 0.0f) return 0.0f;
//            if (rawAlpha == 1.0f) return 1.0f;
//            return rawAlpha < 0.5f
//                ? FMath::Pow(2.0f, 20.0f * rawAlpha - 10.0f) * 0.5f
//                : (2.0f - FMath::Pow(2.0f, -20.0f * rawAlpha + 10.0f)) * 0.5f;
//
//        default:
//            return rawAlpha;
//        }
//    }
//
//    // FPendingLatentAction interface
//    virtual void UpdateOperation(FLatentResponse& response) override
//    {
//        // Early exit if target is invalid - trigger Failed branch
//        if (!IsTargetValid())
//        {
//            OperationResult = ELatentActionResult::Failed;
//            response.TriggerLink(ExecutionFunction, OutputLink + static_cast<int32>(OperationResult), CallbackTarget);
//            response.DoneIf(true);
//            return;
//        }
//
//        // Update time and calculate interpolation alpha
//        ElapsedTime += response.ElapsedTime();
//        const float RawAlpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
//        const float Alpha = CalculateAlpha(RawAlpha, EasingType);
//
//        // Execute the operation with the calculated alpha
//        if (!PerformOperation(Alpha))
//        {
//            OperationResult = ELatentActionResult::Failed;
//        }
//
//        // Check if operation is complete
//        const bool bIsComplete = RawAlpha >= 1.0f;
//        if (bIsComplete)
//        {
//            // The +0/+1 offsets map to the enum values (Success=0, Failed=1)
//            const int32 LinkOffset = static_cast<int32>(OperationResult);
//            response.TriggerLink(ExecutionFunction, OutputLink + LinkOffset, CallbackTarget);
//        }
//
//        response.DoneIf(bIsComplete);
//    }
//
//    // Abstract methods
//    [[nodiscard]] virtual bool IsTargetValid() const = 0;
//    [[nodiscard]] virtual T* GetTarget() = 0;
//    [[nodiscard]] virtual bool PerformOperation(float alpha) = 0; // Returns false if operation fails
//
//protected:
//    // Action parameters
//    FName ExecutionFunction;
//    int32 OutputLink;
//    FWeakObjectPtr CallbackTarget;
//    float Duration;
//    float ElapsedTime;
//    EEasingFunction EasingType;
//    ELatentActionResult OperationResult;
//    FOnLatentActionStarted OnStarted;
//};
//
///**
// * Optimized implementation for actor movement with latent action
// * Uses pre-calculated vectors for efficient interpolation
// */
//class FMoveActorToLocationAction : public TGenericLatentAction<AActor>
//{
//public:
//    FMoveActorToLocationAction(AActor* targetActor, const FVector& endLocation,
//        float duration, bool sweep, EEasingFunction easingType,
//        const FLatentActionInfo& latentInfo, const FOnLatentActionStarted& onStarted)
//        : TGenericLatentAction<AActor>(latentInfo, duration, easingType, onStarted)
//        , TargetActor(targetActor)
//        , StartLocation(targetActor ? targetActor->GetActorLocation() : FVector::ZeroVector)
//        , EndLocation(endLocation)
//        , LocationDelta(EndLocation - StartLocation) // Pre-calculate for performance
//        , bSweep(sweep)
//    {
//    }
//
//    // TGenericLatentAction interface
//    [[nodiscard]] virtual bool IsTargetValid() const override
//    {
//        return TargetActor.IsValid();
//    }
//
//    [[nodiscard]] virtual AActor* GetTarget() override
//    {
//        return TargetActor.Get();
//    }
//
//    [[nodiscard]] virtual bool PerformOperation(float alpha) override
//    {
//        AActor* Actor = TargetActor.Get();
//        if (!Actor)
//        {
//            return false;
//        }
//
//        // Direct calculation is faster than FMath::Lerp for simple vector interpolation
//        // when we already have the delta pre-calculated
//        const FVector NewLocation = StartLocation + (LocationDelta * alpha);
//
//        if (bSweep)
//        {
//            FHitResult HitResult;
//            bool bMoveSucceeded = Actor->SetActorLocation(NewLocation, bSweep, &HitResult);
//
//            // Consider it failed if we hit something and couldn't complete the move
//            if (!bMoveSucceeded && HitResult.bBlockingHit)
//            {
//                return false;
//            }
//        }
//        else
//        {
//            Actor->SetActorLocation(NewLocation);
//        }
//
//        return true;
//    }
//
//private:
//    // Cache values to avoid repeated lookups
//    TWeakObjectPtr<AActor> TargetActor;
//    FVector StartLocation;
//    FVector EndLocation;
//    FVector LocationDelta; // Pre-calculated for optimized interpolation
//    bool bSweep;
//};
//
///**
// * Optimized implementation for actor rotation with quaternion interpolation
// * Uses quaternion Slerp for more accurate rotation along shortest path
// */
//class FRotateActorToRotationAction : public TGenericLatentAction<AActor>
//{
//public:
//    FRotateActorToRotationAction(AActor* targetActor, const FRotator& endRotation,
//        float duration, EEasingFunction easingType,
//        const FLatentActionInfo& latentInfo, const FOnLatentActionStarted& onStarted)
//        : TGenericLatentAction<AActor>(latentInfo, duration, easingType, onStarted)
//        , TargetActor(targetActor)
//        , StartRotation(targetActor ? targetActor->GetActorRotation() : FRotator::ZeroRotator)
//        , EndRotation(endRotation)
//        // Pre-calculate quaternions for more efficient interpolation
//        , StartQuat(StartRotation.Quaternion())
//        , EndQuat(EndRotation.Quaternion())
//    {
//    }
//
//    [[nodiscard]] virtual bool IsTargetValid() const override
//    {
//        return TargetActor.IsValid();
//    }
//
//    [[nodiscard]] virtual AActor* GetTarget() override
//    {
//        return TargetActor.Get();
//    }
//
//    [[nodiscard]] virtual bool PerformOperation(float alpha) override
//    {
//        AActor* Actor = TargetActor.Get();
//        if (!Actor)
//        {
//            return false;
//        }
//
//        // Use quaternion Slerp for more accurate rotation interpolation
//        // This avoids gimbal lock and provides shortest-path rotation
//        const FQuat NewQuat = FQuat::Slerp(StartQuat, EndQuat, alpha);
//        Actor->SetActorRotation(NewQuat);
//
//        return true;
//    }
//
//private:
//    // Cache values to avoid repeated lookups and calculations
//    TWeakObjectPtr<AActor> TargetActor;
//    FRotator StartRotation;
//    FRotator EndRotation;
//
//    // Pre-calculated quaternions for optimized Slerp
//    FQuat StartQuat;
//    FQuat EndQuat;
//};
//
///**
// * Optimized latent action for actor scaling
// * Performs component-wise interpolation with minimal overhead
// */
//class FScaleActorAction : public TGenericLatentAction<AActor>
//{
//public:
//    FScaleActorAction(AActor* targetActor, const FVector& endScale,
//        float duration, EEasingFunction easingType,
//        const FLatentActionInfo& latentInfo, const FOnLatentActionStarted& onStarted)
//        : TGenericLatentAction<AActor>(latentInfo, duration, easingType, onStarted)
//        , TargetActor(targetActor)
//        , StartScale(targetActor ? targetActor->GetActorScale3D() : FVector::OneVector)
//        , EndScale(endScale)
//        // Pre-calculate scale delta for more efficient interpolation
//        , ScaleDelta(EndScale - StartScale)
//    {
//    }
//
//    [[nodiscard]] virtual bool IsTargetValid() const override
//    {
//        return TargetActor.IsValid();
//    }
//
//    [[nodiscard]] virtual AActor* GetTarget() override
//    {
//        return TargetActor.Get();
//    }
//
//    [[nodiscard]] virtual bool PerformOperation(float alpha) override
//    {
//        AActor* Actor = TargetActor.Get();
//        if (!Actor)
//        {
//            return false;
//        }
//
//        // Direct calculation approach is faster than calling FMath::Lerp for vectors
//        // when we already have the delta pre-calculated
//        const FVector NewScale = StartScale + (ScaleDelta * alpha);
//        Actor->SetActorScale3D(NewScale);
//
//        return true;
//    }
//
//private:
//    // Cache values to avoid repeated lookups and calculations
//    TWeakObjectPtr<AActor> TargetActor;
//    FVector StartScale;
//    FVector EndScale;
//
//    // Pre-calculated scale delta for optimized interpolation
//    FVector ScaleDelta;
//};