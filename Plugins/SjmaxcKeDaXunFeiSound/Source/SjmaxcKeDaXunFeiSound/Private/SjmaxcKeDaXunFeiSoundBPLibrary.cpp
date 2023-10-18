// Copyright Epic Games, Inc. All Rights Reserved.

#include "SjmaxcKeDaXunFeiSoundBPLibrary.h"
#include "SjmaxcKeDaXunFeiSound.h"
#include "Core/SjmaxcKeDaXunFeiSocketSubsystem.h"
#include "Kismet/GameplayStatics.h"

USjmaxcKeDaXunFeiSoundBPLibrary::USjmaxcKeDaXunFeiSoundBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void USjmaxcKeDaXunFeiSoundBPLibrary::SjmaxcBeginSpeachToText(const UObject* WorldContextObject, FSjmaxcBeginSpeachToTextDelegate SjmaxcBeginSpeachToTextDelegate, FSjmaxcSpeachToTextDelegate SjmaxcSpeachToTextDelegat)
{
	if (!WorldContextObject)
	{
		return;
	}
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	USjmaxcKeDaXunFeiSocketSubsystem* SocketSystem = GameInstance->GetSubsystem<USjmaxcKeDaXunFeiSocketSubsystem>();

	SocketSystem->SjmaxcBeginSpeachToText(SjmaxcBeginSpeachToTextDelegate, SjmaxcSpeachToTextDelegat);

}

void USjmaxcKeDaXunFeiSoundBPLibrary::SjmaxcStopSpeachToText(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return;
	}
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject);
	USjmaxcKeDaXunFeiSocketSubsystem* SocketSystem = GameInstance->GetSubsystem<USjmaxcKeDaXunFeiSocketSubsystem>();
	SocketSystem->SjmaxcStopSpeachToText();

}
