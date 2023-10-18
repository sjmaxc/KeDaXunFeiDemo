
#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "SjmaxcTTSPlayVoiceComponent.generated.h"


UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class SJMAXCKEDAXUNFEISOUND_API USjmaxcTTSPlayVoiceComponent : public USynthComponent
{
	GENERATED_BODY()

	// Called when synth is created
	virtual bool Init(int32& SampleRate) override;
	
	// Called to generate more audio
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

protected:
	int32 PlayIndex = 0;
};