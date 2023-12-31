// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/SjmaxcKeDaXunFeiSocketSubsystem.h"
#include "WebSocketsModule.h"
#include "Core/SjmaxcAudioCaptureSubsystem.h"
#include "Type/SjmaxcKeDaXunFeiSoundSettings.h"

bool USjmaxcKeDaXunFeiSocketSubsystem::bSending = false;
TSharedPtr<IWebSocket> USjmaxcKeDaXunFeiSocketSubsystem::Socket = {};

USjmaxcAudioCaptureSubsystem* USjmaxcKeDaXunFeiSocketSubsystem::SjmaxcAudioCaptureSubsystem = nullptr;

bool USjmaxcKeDaXunFeiSocketSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USjmaxcKeDaXunFeiSocketSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Appid = USjmaxcKeDaXunFeiSoundSettings::GetDataBaseSettings()->AppID;
	APIKey = USjmaxcKeDaXunFeiSoundSettings::GetDataBaseSettings()->APIKeySTT;
	
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

void USjmaxcKeDaXunFeiSocketSubsystem::SjmaxcBeginSpeachToText(FSjmaxcBeginSpeachToTextDelegate InSjmaxcBeginSpeachToTextDelegate,FSjmaxcSpeachToTextDelegate InSjmaxcSpeachToTextDelegate)
{
	if (bSpeechToText)
	{
		return;
	}
	bSpeechToText = true;
	SjmaxcBeginSpeachToTextDelegate = InSjmaxcBeginSpeachToTextDelegate;
	SjmaxcSpeachToTextDelegate = InSjmaxcSpeachToTextDelegate;
	
	CreateSocket();
}

void USjmaxcKeDaXunFeiSocketSubsystem::SjmaxcStopSpeachToText()
{
	if (!bSpeechToText)
	{
		return;
	}
	bSpeechToText = false;
	bSending = false;
	SjmaxcBeginSpeachToTextDelegate.Clear();
	SjmaxcSpeachToTextDelegate.Clear();

	SjmaxcAudioCaptureSubsystem->StopCapturingAudio();
	if (VoiceRunnable.IsValid())
	{
		VoiceRunnable->Stop();
	}
	EndSendVoiceData();
}

void USjmaxcKeDaXunFeiSocketSubsystem::EndSendVoiceData()
{
	if (Socket.IsValid() && Socket->IsConnected() && !bSending)
	{
		FString EndStr = TEXT("{\"end\": true}");

		const char * CharValue = TCHAR_TO_UTF8(*EndStr);
		int32 Length = strlen(CharValue);
		Socket->Send(CharValue, Length, true);
	}
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
	
	ServerURL += TEXT("&lang=cn&transType=normal&transStrategy=2&targetLang=en");	// 拼接后缀
	
	Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
	Socket->OnConnected().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnConnected);
	Socket->OnConnectionError().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnConnectionError);
	Socket->OnClosed().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnClosed);
	Socket->OnMessage().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnMessage);
	Socket->OnMessageSent().AddUObject(this, &USjmaxcKeDaXunFeiSocketSubsystem::OnMessageSent);
	Socket->Connect();

	bSending = true;
}

void USjmaxcKeDaXunFeiSocketSubsystem::CloseSocket()
{
	bSending = false;
	if (Socket.IsValid()&& Socket->IsConnected())
	{
		Socket->Close();
	}

	bSpeechToText = false;
	SjmaxcBeginSpeachToTextDelegate.Clear();
	SjmaxcSpeachToTextDelegate.Clear();
}

void USjmaxcKeDaXunFeiSocketSubsystem::SendVoiceData(const float* InAudio, int32 NumSamples)
{
	if (Socket.IsValid()&& Socket->IsConnected()&& bSending)
	{
		//我们拿到的数据实际上2048个采样点 每个采样点都是float类型,4个字节,32位
		//现在每64ms 发送1024个采样点信息,一个采样点是2个字节,16位.
		//发送2048个字节
		TArray<int16> ToChangeAuidoData;
		TArray<uint8> BinaryDataToSend;
		int32 i = 0;
		for (; i < 1024;)
		{	//-1.0~1.0
			ToChangeAuidoData.Add((int16)FMath::Clamp<int32>(FMath::FloorToInt(32767.0f * InAudio[i]), -32768, 32767));

			uint8 Bytes[2];
			//取出数据的高位数据
			Bytes[0] = (uint8)(ToChangeAuidoData[i] & 0xFF);
			//取出数据的低位数据
			Bytes[1] = (uint8)((ToChangeAuidoData[i] >> 8) & 0xFF);
			
			//大端存储把高位数据放到低字节
			BinaryDataToSend.Add(Bytes[0]);
			//大端存储把地位数据放到高字节
			BinaryDataToSend.Add(Bytes[1]);

			i++;
		}
		Socket->Send(BinaryDataToSend.GetData(), BinaryDataToSend.Num(),false);
	}
}

void USjmaxcKeDaXunFeiSocketSubsystem::StopSendVoiceData()
{
	bSending = false;
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnConnected()
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(__FUNCTION__));
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnConnectionError(const FString& Error)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Error:%s"), *FString(__FUNCTION__), *Error);

	bSpeechToText = false;
	SjmaxcBeginSpeachToTextDelegate.ExecuteIfBound(false);
	SjmaxcBeginSpeachToTextDelegate.Clear();
	SjmaxcSpeachToTextDelegate.Clear();
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnClosed(int32 StatusCode, const FString& Reason, bool bWasClean)
{
	UE_LOG(LogTemp, Warning, TEXT("%s StatusCode:%d Reason:%s bWasClean:%d"),
	*FString(__FUNCTION__), StatusCode, *Reason, bWasClean);

	bSending = false;
	bSpeechToText = false;
	SjmaxcBeginSpeachToTextDelegate.Clear();
	SjmaxcSpeachToTextDelegate.Clear();

	SjmaxcAudioCaptureSubsystem->StopCapturingAudio();
	if (VoiceRunnable.IsValid())
	{
		VoiceRunnable->Stop();
	}
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnMessage(const FString& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Message:%s"), *FString(__FUNCTION__), *Message);
	if (!Message.IsEmpty())
	{
		TSharedPtr<FJsonObject> ResultObj;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);
		FJsonSerializer::Deserialize(Reader, ResultObj);

		FString MessageAction;
		
		if (ResultObj->TryGetStringField("action", MessageAction))
		{
			if (MessageAction.Equals(TEXT("started")))
			{
				bSending = true;
				bSpeechToText = true;

				SjmaxcAudioCaptureSubsystem->StartCapturingAudio();
				VoiceRunnable = MakeShareable(  new FConsumeSoundRunnable(TEXT("VoiceRunnable")));
				FRunnableThread* RunnableThread1 = FRunnableThread::Create(VoiceRunnable.Get(), *VoiceRunnable->MyThreadName);
			}
			
			if (MessageAction.Equals(TEXT("error")))
			{
				bSending = false;
				bSpeechToText = false;

				SjmaxcBeginSpeachToTextDelegate.ExecuteIfBound(false);
				SjmaxcBeginSpeachToTextDelegate.Clear();
				SjmaxcSpeachToTextDelegate.Clear();
			}
			if (MessageAction.Equals(TEXT("result")))
			{
				FString OutText;
				if (ResultObj->TryGetStringField("data", OutText))
				{
					if (!OutText.IsEmpty())
					{
						TSharedPtr<FJsonObject> DataObj;
						TSharedRef<TJsonReader<>> Reader2 = TJsonReaderFactory<>::Create(OutText);
						FJsonSerializer::Deserialize(Reader2, DataObj);

						FString dst;
						FString src;
						FString end;

						bool bDest = DataObj->TryGetStringField(TEXT("dst"), dst);
						bool bSrc = DataObj->TryGetStringField(TEXT("src"), src);
						bool bEnd = DataObj->TryGetStringField(TEXT("ed"), end);
						int32 EndNum = FCString::Atoi(*end);
						bEnd = EndNum > 0 ? true : false;

						if (bDest && bSrc&& bEnd)
						{
							UE_LOG(LogTemp, Warning, TEXT("dst :%s Message [src]:%s"), *dst, *src);
							SjmaxcSpeachToTextDelegate.ExecuteIfBound(src,dst);
						}
					}
				}
			}
		}
	}
}

void USjmaxcKeDaXunFeiSocketSubsystem::OnMessageSent(const FString& MessageString)
{
	UE_LOG(LogTemp, Warning, TEXT("%s MessageString:%s"), *FString(__FUNCTION__), *MessageString);
}
