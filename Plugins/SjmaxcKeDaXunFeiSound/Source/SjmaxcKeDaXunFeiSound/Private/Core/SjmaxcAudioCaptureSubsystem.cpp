// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SjmaxcAudioCaptureSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Core/SjmaxcKeDaXunFeiSocketSubsystem.h"

float* USjmaxcAudioCaptureSubsystem::SjmaxcAudio = nullptr;
int32 USjmaxcAudioCaptureSubsystem::SjmaxcNumSamples = -1;
TArray<float*>  USjmaxcAudioCaptureSubsystem::InAudios = {};
float* USjmaxcAudioCaptureSubsystem::ToSendData = nullptr;

int32 USjmaxcAudioCaptureSubsystem::IndexSend = 0;

FCriticalSection USjmaxcAudioCaptureSubsystem::SjmaxcAudioCriticalSection;
TArray<float> USjmaxcAudioCaptureSubsystem::AudioData = {};

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
	FScopeLock Lock(&SjmaxcAudioCriticalSection);
	AudioData.Empty();
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
	FScopeLock Lock(&SjmaxcAudioCriticalSection);
	if (AudioData.Num()>0)
	{
		AudioData.Empty();
	}
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

bool USjmaxcAudioCaptureSubsystem::GetAudioCaptureDeviceInfo(FAudioCaptureDeviceInfo& OutInfo)
{
	bool bGet =	SjmaxcAudioCapture->GetAudioCaptureDeviceInfo(OutInfo);
	UE_LOG(LogTemp, Warning, TEXT("DeviceName:[%s]---NumInputChannels:[%d]----SampleRate:[%d]"), *(OutInfo.DeviceName.ToString()), OutInfo.NumInputChannels, OutInfo.SampleRate);
	return bGet;
}

void USjmaxcAudioCaptureSubsystem::OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
	/*
	 *UE_LOG(LogTemp, Display, TEXT("OnAudioCapture %f-----%d"), *InAudio, NumSamples);
	
	SjmaxcAudio = const_cast<float*>(InAudio);
	SjmaxcNumSamples = NumSamples;

	SjmaxcAudio = new float[NumSamples];

	for (int i = 0; i < NumSamples; i++)
	{
		SjmaxcAudio[i] = InAudio[i];
	}
	
	InAudios.Add(SjmaxcAudio);
	*/

	FScopeLock Lock(&SjmaxcAudioCriticalSection);
	int32 LeftIndex = 0;
	if (IndexSend == 0)
	{
		IndexSend++;
		for (int32 SendIndex = 0; SendIndex<341; SendIndex++)
		{
			AudioData.Add(InAudio[LeftIndex]);
			LeftIndex += 6;
			
			
		}

	}
	else if (IndexSend ==1)
	{

		IndexSend++;
		for (int32 SendIndex = 341; SendIndex < 682; SendIndex++)
		{
			AudioData.Add(InAudio[LeftIndex]);

			LeftIndex += 6;
		}


	}
	else if (IndexSend ==2)
	{

		IndexSend = 0;
		for (int32 SendIndex = 682; SendIndex < 1024; SendIndex++)
		{
			AudioData.Add(InAudio[LeftIndex]);

			LeftIndex += 6;
		}
		
		//	USjmaxcKeDaXunFeiSocketSubsystem::SendVoiceData(ToSendData, NumSamples);
	}
}
