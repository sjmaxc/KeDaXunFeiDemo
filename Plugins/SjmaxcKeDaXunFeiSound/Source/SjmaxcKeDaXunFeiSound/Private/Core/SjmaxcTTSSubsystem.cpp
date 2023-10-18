// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SjmaxcTTSSubsystem.h"
#include "WebSocketsModule.h"
#include "Core/SjmaxcAudioCaptureSubsystem.h"
#include "Type/SjmaxcKeDaXunFeiSoundSettings.h"
#include "Algorithm/hmacsha256.h" 

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
	FString APPID = USjmaxcKeDaXunFeiSoundSettings::GetDataBaseSettings()->AppID;
	FString APIKey = USjmaxcKeDaXunFeiSoundSettings::GetDataBaseSettings()->APIKeyTTSStream;
	FString APISecret = USjmaxcKeDaXunFeiSoundSettings::GetDataBaseSettings()->APISecretTTSStream;
	
	//wss://tts-api.xfyun.cn/v2/tts?authorization=aG1hYyB1c2VybmFtZT0iZGE0ZjMyOWUyZmQwMGQ1NjE4NjVjNjRkZjU3NDNiMjAiLCBhbGdvcml0aG09ImhtYWMtc2hhMjU2IiwgaGVhZGVycz0iaG9zdCBkYXRlIHJlcXVlc3QtbGluZSIsIHNpZ25hdHVyZT0ic1RtbzRobDBMdmRLWTRLRjltcGJKV0htRFFzNC8xZ2ZPdUgwZnBZbVdnbz0i&date=Thu%2C%2001%20Aug%202019%2001%3A53%3A21%20GMT&host=tts-api.xfyun.cn
	FString Date = FDateTime::Now().UtcNow().ToHttpDate();
	//Thu, 01 Aug 2019 01:53:21 GMT
	ServerURL =TEXT("");
	

	FString host = TEXT("host: tts-api.xfyun.cn\n");
	FString date = TEXT("date: ") + Date + TEXT("\n");
	FString requireLine = TEXT("GET /v2/tts HTTP/1.1");

	FString signature_origin = host + date + requireLine;

	//拿到签字
	FString signature = HMACSHA256(APISecret, signature_origin);
	
	
	//api_key = "$api_key", algorithm = "hmac-sha256", headers = "host date request-line", signature = "$signature"
	FString authorization_origin = TEXT("api_key=");

	authorization_origin += TEXT("\"");
	authorization_origin += APIKey;
	authorization_origin += TEXT("\"");
	authorization_origin += TEXT(",algorithm=\"hmac-sha256\", headers=\"host date request-line\", signature=");
	authorization_origin += TEXT("\"");
	authorization_origin += signature;
	authorization_origin += TEXT("\"");

	// 获得真正的令牌
	FString authorization = FBase64::Encode(authorization_origin);
	
	Date.ReplaceInline(TEXT(","), TEXT("%2C"));
	Date.ReplaceInline(TEXT(" "), TEXT("%20"));
	Date.ReplaceInline(TEXT(":"), TEXT("%3A"));
	
	ServerURL = TEXT("ws://tts-api.xfyun.cn/v2/tts?authorization=");
	ServerURL += authorization;
	ServerURL += TEXT("&date=");
	ServerURL += Date;
	ServerURL += TEXT("&host=tts-api.xfyun.cn");


	
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

FString USjmaxcTTSSubsystem::HMACSHA256(FString APPSecreet, FString Data)
{
	uint8_t md[SHA256_DIGESTLEN] = { 0 };
	uint8_t* secret = (uint8_t*)TCHAR_TO_UTF8(*APPSecreet);
	uint8_t* indata = (uint8_t*)TCHAR_TO_UTF8(*Data);

	int len1 = strlen((char*)secret);
	int len2 = strlen((char*)indata);

	HMAC_SHA256_CTX hmac;

	hmac_sha256_init(&hmac, secret, len1);
	hmac_sha256_update(&hmac, indata, len2);
	hmac_sha256_final(&hmac, md);

	FString RetStr = TEXT("");

	RetStr = FBase64::Encode(md, SHA256_DIGESTLEN);

	return RetStr;
}