#include "Thread/ConsumeSoundRunnable.h"
#include "Core/SjmaxcKeDaXunFeiSocketSubsystem.h"
#include "Core/SjmaxcAudioCaptureSubsystem.h"

FCriticalSection FConsumeSoundRunnable::CriticalSection;
bool FConsumeSoundRunnable::Init()
{
	return true;
}

uint32 FConsumeSoundRunnable::Run()
{
	while (true)
	{

		FScopeLock Lock(&CriticalSection);
		if (!bRunning)
		{
			break;;
		}
		Lock.Unlock();
		FPlatformProcess::Sleep(0.04);
		
		if (USjmaxcAudioCaptureSubsystem::AudioData.Num()>1024)
		{
			FScopeLock DataLock(&USjmaxcAudioCaptureSubsystem::SjmaxcAudioCriticalSection);
			TArray<float> SendData;
			SendData.Append(USjmaxcAudioCaptureSubsystem::AudioData.GetData(), 1024);
			USjmaxcAudioCaptureSubsystem::AudioData.RemoveAt(0, 1024);

			USjmaxcKeDaXunFeiSocketSubsystem::SendVoiceData(SendData.GetData(), 1024);
		}
		
	}
	
	return 0;
}

void FConsumeSoundRunnable::Exit()
{

}

void FConsumeSoundRunnable::Stop()
{
	UE_LOG(LogTemp, Display, TEXT("heelo[%s]stopsssssss"), *MyThreadName);
	FScopeLock Lock(&CriticalSection);
	bRunning = false;

}
