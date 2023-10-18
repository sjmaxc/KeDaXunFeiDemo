// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SjmaxcKeDaXunFeiSoundBPLibrary.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FSjmaxcBeginSpeachToTextDelegate,bool,bBeginSpeachToText);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FSjmaxcSpeachToTextDelegate, FString, OutSrcText,FString, OutDstText);

UCLASS()
class USjmaxcKeDaXunFeiSoundBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "BeginSpeachToText", Keywords = "KeDaXunFei ", WorldContext = "WorldContextObject"), Category = "SjmaxcKeDaXunFeiSoundTesting")
	static void SjmaxcBeginSpeachToText(const UObject* WorldContextObject, FSjmaxcBeginSpeachToTextDelegate SjmaxcBeginSpeachToTextDelegate, FSjmaxcSpeachToTextDelegate SjmaxcSpeachToTextDelegate);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "StopSpeachToText", Keywords = "KeDaXunFei ", WorldContext = "WorldContextObject"), Category = "SjmaxcKeDaXunFeiSoundTesting")
	static void SjmaxcStopSpeachToText(const UObject* WorldContextObject);
};
