#include "Component/SjmaxcTTSPlayVoiceComponent.h"
#include "Core/SjmaxcTTSSubsystem.h"
#include "Kismet/GameplayStatics.h"

bool USjmaxcTTSPlayVoiceComponent::Init(int32& SampleRate)
{
	NumChannels = 1;
	SampleRate = 16000;

	return true;
}

int32 USjmaxcTTSPlayVoiceComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	//避免越界
	if (PlayIndex > USjmaxcTTSSubsystem::FinalUEData.Num() - 1)
	{
		return 1024;
	}
	
	//需要优化
	for (int32 SoundIndex=0; SoundIndex<1024; SoundIndex++)
	{

		if (PlayIndex > USjmaxcTTSSubsystem::FinalUEData.Num() - 1)
		{
			return 1024;
		}
		OutAudio[SoundIndex] = USjmaxcTTSSubsystem::FinalUEData[PlayIndex];
		PlayIndex++;
	}
	return 1024;
}