// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

DECLARE_LOG_CATEGORY_EXTERN(MyLog, Log, All);

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Networking.h"

#include "VoxelSender.generated.h"



UCLASS()
class RENDER_SAVER_API AVoxelSender : public AActor
{
	GENERATED_BODY()
	
	

public:	
	// Sets default values for this actor's properties
	AVoxelSender();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool StartTCPReceiver(const FString& YourChosenSocketName);

	FSocket* CreateTCPConnectionListener(
		const FString& YourChosenSocketName,
		const int32 ReceiveBufferSize = 2 * 1024 * 1024
	);

	UFUNCTION(BlueprintCallable)
	bool SendVoxel(FVector pos, FVector color);

	//Timer functions, could be threads
	void TCPConnectionListener();   //can thread this eventually
	//void TCPSocketListener();       //can thread this eventually

private:
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	bool connected = false;
	bool first = true;

};
