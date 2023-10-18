// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SjmaxcPlayVoiceComponent.h"
#include "Core/SjmaxcAudioCaptureSubsystem.h"


int32 USjmaxcPlayVoiceComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	int i = 0;
	int j = 0;
	for (; i < 1024;)
	{
		// 1024 <= 2048
		// LLLLLLLL <=LRLRLRLRLRLR
		OutAudio[i] = USjmaxcAudioCaptureSubsystem::InAudios[SoundIndex][j];
		i++;
		j = j + 2;
	}
	SoundIndex++;
	return 1024;
}
