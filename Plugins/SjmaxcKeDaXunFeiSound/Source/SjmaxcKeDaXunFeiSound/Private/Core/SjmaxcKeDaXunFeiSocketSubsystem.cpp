// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SjmaxcKeDaXunFeiSocketSubsystem.h"
#include "WebSocketsModule.h"

TSharedPtr<IWebSocket> USjmaxcKeDaXunFeiSocketSubsystem::Socket = {};

bool USjmaxcKeDaXunFeiSocketSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USjmaxcKeDaXunFeiSocketSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	
}

void USjmaxcKeDaXunFeiSocketSubsystem::Deinitialize()
{

}

void USjmaxcKeDaXunFeiSocketSubsystem::Tick(float DeltaTime)
{
}

bool USjmaxcKeDaXunFeiSocketSubsystem::IsTickable() const
{
	return !IsTemplate(); //不是CDO才Tick
}

TStatId USjmaxcKeDaXunFeiSocketSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USjmaxcKeDaXunFeiSocketSubsystem, STATGROUP_Tickables); 
}

void USjmaxcKeDaXunFeiSocketSubsystem::CreateSocket()
{
	FModuleManager::Get().LoadModuleChecked("WebSockets");

	FString BaseString = TEXT("");
	FString TS = TEXT("1512041814");
	TS = FString::FromInt(FDateTime::Now().UtcNow().ToUnixTimestamp());
	BaseString = Appid + TS;
	
	FString MD5BaseString = FMD5::HashAnsiString(*BaseString);
	
	FSHAHash Hash;
	FSHA1 SHA1;
	SHA1.HMACBuffer(TCHAR_TO_ANSI(*APIKey), APIKey.Len(), TCHAR_TO_ANSI(*MD5BaseString), MD5BaseString.Len(), Hash.Hash);
	/*
	FString WrongSigna1 = Hash.ToString();
	FString WrongSigna2 = FBase64::Encode(WrongSigna1);
	*/
	FString Signa = FBase64::Encode(Hash.Hash, 20);
	Signa.ReplaceInline(TEXT("="), TEXT("%3D"));

	ServerProtocol = TEXT("ws");
	// 示例URL: ws://rtasr.xfyun.cn/v1/ws?appid=595f23df&ts=1512041814&signa=IrrzsJeOFk1NGfJHW6SkHUoN9CU=&pd=edu
	ServerURL = TEXT("ws://rtasr.xfyun.cn/v1/ws?");
	ServerURL += TEXT("appid=");	//拼接appid
	ServerURL += Appid;

	ServerURL += TEXT("&");
	ServerURL += TEXT("ts=");	// 拼接时间戳
	ServerURL += TS;

	ServerURL += TEXT("&");
	ServerURL += TEXT("signa=");	 // 拼接数字令牌
	ServerURL += Signa;
	
	ServerURL += TEXT("&");
	ServerURL += TEXT("pd=edu");	// 拼接类型
	
	// ServerURL += TEXT("&lang=cn&transType=normal&transStrategy=2&targetLang=en");	// 拼接后缀
	
	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
	Socket->OnConnected().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnConnected);
	Socket->OnConnectionError().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnConnectionError);
	Socket->OnClosed().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnClosed);
	Socket->OnMessage().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnMessage);
	Socket->OnMessageSent().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnMessageSent);
	Socket->Connect();

}

void USjmaxcKeDaXunFeiSocketSubsystem::CloseSocket()
{
	if (Socket.IsValid()&& Socket->IsConnected())
	{
		Socket->Close();
	}
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__));
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnConnectionError(const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Error:%s"), *FString(__FUNCTION__), *Error);
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogTemp, Warning, TEXT("%s StatusCode:%d Reason:%s bWasClean:%d"),
	*FString(__FUNCTION__), StatusCode, *Reason, bWasClean);
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Message:%s"), *FString(__FUNCTION__), *Message);
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnMessageSent(const FString& MessageString)
{
	UE_LOG(LogTemp, Warning, TEXT("%s MessageString:%s"), *FString(__FUNCTION__), *MessageString);
}
