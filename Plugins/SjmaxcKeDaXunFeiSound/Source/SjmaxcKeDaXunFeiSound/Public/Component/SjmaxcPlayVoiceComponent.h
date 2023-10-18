// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "SjmaxcPlayVoiceComponent.generated.h"

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class SJMAXCKEDAXUNFEISOUND_API USjmaxcPlayVoiceComponent : public USynthComponent
{
	GENERATED_BODY()

	// Called to generate more audio
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	int32 SoundIndex = 0;
};
