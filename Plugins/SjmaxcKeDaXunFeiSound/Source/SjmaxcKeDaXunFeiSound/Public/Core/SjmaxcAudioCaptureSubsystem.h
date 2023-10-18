// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "AudioCapture.h"
#include "SjmaxcAudioCaptureSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SJMAXCKEDAXUNFEISOUND_API USjmaxcAudioCaptureSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

public:
	// Starts capturing audio
	UFUNCTION(BlueprintCallable, Category = "AudioCapture")
	void StartCapturingAudio();

	// Stops capturing audio
	UFUNCTION(BlueprintCallable, Category = "AudioCapture")
	void StopCapturingAudio();

	// Returns true if capturing audio
	UFUNCTION(BlueprintCallable, Category = "AudioCapture")
	bool IsCapturingAudio();

	// TODO 垃圾代码等待优化
	//一帧是1024个float
	static	float* SjmaxcAudio;
	static	int32 SjmaxcNumSamples;
	static TArray<float*> InAudios;

protected:

	static void OnAudioGenerate(const float* InAudio, int32 NumSamples);


protected:
	UPROPERTY()
	UAudioCapture* SjmaxcAudioCapture;
};
