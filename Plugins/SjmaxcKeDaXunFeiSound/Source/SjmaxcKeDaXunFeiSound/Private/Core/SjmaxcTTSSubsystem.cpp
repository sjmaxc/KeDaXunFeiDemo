// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SjmaxcTTSSubsystem.h"
#include "WebSocketsModule.h"
#include "Core/SjmaxcAudioCaptureSubsystem.h"
#include "Type/SjmaxcKeDaXunFeiSoundSettings.h"

bool USjmaxcTTSSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USjmaxcTTSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USjmaxcTTSSubsystem::Deinitialize()
{
	
}

void USjmaxcTTSSubsystem::Tick(float DeltaTime)
{
}

bool USjmaxcTTSSubsystem::IsTickable() const
{
	return !IsTemplate(); //不是CDO才Tick
}

TStatId USjmaxcTTSSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USjmaxcTTSSubsystem, STATGROUP_Tickables); 
}

void USjmaxcTTSSubsystem::CreateSocket()
{
	
	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
	Socket->OnConnected().AddUObject(this, &USjmaxcTTSSubsystem::OnConnected);
	Socket->OnConnectionError().AddUObject(this, &USjmaxcTTSSubsystem::OnConnectionError);
	Socket->OnClosed().AddUObject(this, &USjmaxcTTSSubsystem::OnClosed);
	Socket->OnMessage().AddUObject(this, &USjmaxcTTSSubsystem::OnMessage);
	Socket->OnMessageSent().AddUObject(this, &USjmaxcTTSSubsystem::OnMessageSent);
	Socket->Connect();
	
}

void USjmaxcTTSSubsystem::CloseSocket()
{
}

void USjmaxcTTSSubsystem::OnConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__));
}

void USjmaxcTTSSubsystem::OnConnectionError(const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Error:%s"), *FString(__FUNCTION__), *Error);
}

void USjmaxcTTSSubsystem::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogTemp, Warning, TEXT("%s StatusCode:%d Reason:%s bWasClean:%d"),
	*FString(__FUNCTION__), StatusCode, *Reason, bWasClean);
}

void USjmaxcTTSSubsystem::OnMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Message:%s"), *FString(__FUNCTION__), *Message);
}

void USjmaxcTTSSubsystem::OnMessageSent(const FString& MessageString)
{
	UE_LOG(LogTemp, Warning, TEXT("%s MessageString:%s"), *FString(__FUNCTION__), *MessageString);
}
