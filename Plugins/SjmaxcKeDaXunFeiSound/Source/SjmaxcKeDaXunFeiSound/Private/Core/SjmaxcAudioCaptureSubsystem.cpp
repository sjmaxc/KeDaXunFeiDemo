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

	//故意的 如果出问题就崩在这里
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

	USjmaxcKeDaXunFeiSocketSubsystem::SjmaxcAudioCaptureSubsystem = this;

}

void USjmaxcAudioCaptureSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USjmaxcAudioCaptureSubsystem::Tick(float DeltaTime)
{

}

TStatId USjmaxcAudioCaptureSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USjmaxcAudioCaptureSubsystem, STATGROUP_Tickables);
}

bool USjmaxcAudioCaptureSubsystem::IsTickable() const
{
	return !IsTemplate();
}

void USjmaxcAudioCaptureSubsystem::StartCapturingAudio()
{
/*
	if (InAudios.Num() > 0)
	{
		for (auto& SoundPtr : InAudios)
		{
			delete SoundPtr;
			SoundPtr = nullptr;
		}
		InAudios.Empty();
	}*/

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

	FScopeLock Lock(&SjmaxcAudioCriticalSection);
	if (AudioData.Num() > 0)
	{
		AudioData.Empty();
	}



}

bool USjmaxcAudioCaptureSubsystem::IsCapturingAudio()
{
	return SjmaxcAudioCapture->IsCapturingAudio();
}

bool USjmaxcAudioCaptureSubsystem::GetAudioCaptureDeviceInfo(FAudioCaptureDeviceInfo& OutInfo)
{	

	bool bGet =	 SjmaxcAudioCapture->GetAudioCaptureDeviceInfo(OutInfo);
	UE_LOG(LogTemp, Warning, TEXT("DeviceName:[%s]---NumInputChannels:[%d]----SampleRate:[%d]"), *(OutInfo.DeviceName.ToString()), OutInfo.NumInputChannels, OutInfo.SampleRate);
	return bGet;
}

//需要优化噢 48k 双通道的 1024 32位  21.3333ms
//目标数据时16k 单通道 16 位			64.000ms
void USjmaxcAudioCaptureSubsystem::OnAudioGenerate(const float* InAudio, int32 NumSamples)
{
/*
	UE_LOG(LogTemp, Display, TEXT("OnAudioCapture %f-----%d"), *InAudio, NumSamples);

	SjmaxcAudio = const_cast<float*>(InAudio);
	SjmaxcNumSamples = NumSamples;

	SjmaxcAudio = new float[NumSamples];

	for (int i = 0; i < NumSamples; i++)
	{
		SjmaxcAudio[i] = InAudio[i];
	}

	
	InAudios.Add(SjmaxcAudio);*/
	
/*
	GWorld->GetGameInstance();
	//UGameInstance* MyGameInstance = UGameplayStatics::GetGameInstance();

	UGameInstance* MyGameInstance = GWorld->GetGameInstance();

	if (MyGameInstance)
	{
		USjmaxcKeDaXunFeiSocketSubsystem* SocketSubsystem=	MyGameInstance->GetSubsystem<USjmaxcKeDaXunFeiSocketSubsystem>();
		SocketSubsystem->SendVoiceData(InAudio, NumSamples);

	}//有问题 内存泄露
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