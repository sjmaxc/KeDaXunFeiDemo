// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SjmaxcAudioCaptureSubsystem.h"

float* USjmaxcAudioCaptureSubsystem::SjmaxcAudio = nullptr;
int32 USjmaxcAudioCaptureSubsystem::SjmaxcNumSamples = -1;
TArray<float*>  USjmaxcAudioCaptureSubsystem::InAudios = {};

bool USjmaxcAudioCaptureSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USjmaxcAudioCaptureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	// 没有音频设备的话在这里会崩溃
	SjmaxcAudioCapture = UAudioCaptureFunctionLibrary::CreateAudioCapture();
	SjmaxcAudioCapture->AddGeneratorDelegate(&USjmaxcAudioCaptureSubsystem::OnAudioGenerate);

	if (InAudios.Num() > 0)
	{
		for (auto& SoundPtr : InAudios)
		{
			delete SoundPtr;
			SoundPtr = nullptr;
		}
		InAudios.Empty();
	}
}

void USjmaxcAudioCaptureSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USjmaxcAudioCaptureSubsystem::Tick(float DeltaTime)
{
}

bool USjmaxcAudioCaptureSubsystem::IsTickable() const
{
	return !IsTemplate(); //不是CDO才Tick
}

TStatId USjmaxcAudioCaptureSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USjmaxcAudioCaptureSubsystem, STATGROUP_Tickables); 
}

void USjmaxcAudioCaptureSubsystem::StartCapturingAudio()
{
	if (InAudios.Num() > 0)
	{
		for (auto& SoundPtr : InAudios)
		{
			delete SoundPtr;
			SoundPtr = nullptr;
		}
		InAudios.Empty();
	}
	int32 test = SjmaxcAudioCapture->GetSampleRate();
	UE_LOG(LogTemp, Display, TEXT("OnAudioCapture %d"), test);
	SjmaxcAudioCapture->StartCapturingAudio();
}

void USjmaxcAudioCaptureSubsystem::StopCapturingAudio()
{
	SjmaxcAudioCapture->StopCapturingAudio();
}

bool USjmaxcAudioCaptureSubsystem::IsCapturingAudio()
{
	return SjmaxcAudioCapture->IsCapturingAudio();
}

void USjmaxcAudioCaptureSubsystem::OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
	UE_LOG(LogTemp, Display, TEXT("OnAudioCapture %f-----%d"), *InAudio, NumSamples);
	
	SjmaxcAudio = const_cast<float*>(InAudio);
	SjmaxcNumSamples = NumSamples;

	SjmaxcAudio = new float[NumSamples];

	for (int i = 0; i < NumSamples; i++)
	{
		SjmaxcAudio[i] = InAudio[i];
	}
	
	InAudios.Add(SjmaxcAudio);
}
