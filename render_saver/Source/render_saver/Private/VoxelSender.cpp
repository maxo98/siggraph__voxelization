// Fill out your copyright notice in the Description page of Project Settings.


#include "VoxelSender.h"

DEFINE_LOG_CATEGORY(MyLog);


// Sets default values
AVoxelSender::AVoxelSender()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxelSender::BeginPlay()
{
	Super::BeginPlay();
	
    if (!StartTCPReceiver("SocketListener"))
    {
        return;
    }
}

// Called every frame
void AVoxelSender::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (connected == false)
    {
        TCPConnectionListener();
    }
    else {
        if (first == true)
        {
            SendVoxel(FVector(3, 3, 3), FVector(1, 1, 1));
        }
    }
}

//Start TCP Receiver
bool AVoxelSender::StartTCPReceiver(
    const FString& YourChosenSocketName) 
{
    //CreateTCPConnectionListener
    ListenerSocket = CreateTCPConnectionListener(YourChosenSocketName);

    //Not created?
    if (!ListenerSocket)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created!")));
        return false;
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket created!")));

    //Start the Listener! //thread this eventually
    //GetWorldTimerManager().SetTimer(this, &AVoxelSender::TCPConnectionListener, 0.01, true);

    return true;
}

//Create TCP Connection Listener
FSocket* AVoxelSender::CreateTCPConnectionListener(const FString& YourChosenSocketName, const int32 ReceiveBufferSize)
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //Create Socket
    FIPv4Endpoint Endpoint(FIPv4Address(192, 168, 0, 12), 3333);
    FSocket* ListenSocket = FTcpSocketBuilder(*YourChosenSocketName)
        .AsReusable()
        .BoundToEndpoint(Endpoint)
        .Listening(8);

    //Set Buffer Size
    int32 NewSize = 0;
    ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

    //Done!
    return ListenSocket;
}

//TCP Connection Listener
void AVoxelSender::TCPConnectionListener()
{
    //~~~~~~~~~~~~~
    if (!ListenerSocket) return;
    //~~~~~~~~~~~~~

    //Remote address
    TSharedRef RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    bool Pending;

    // handle incoming connections
    if (ListenerSocket->HasPendingConnection(Pending) &&Pending)
    {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        //Already have a Connection? destroy previous
        if (ConnectionSocket)
        {
            ConnectionSocket->Close();
            ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        //New Connection receive!
        ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("TCP Received Socket Connection"));

        if (ConnectionSocket != NULL)
        {
            //Global cache of current Remote Address
            RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);

            connected = true;

            if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connected")));
            //UE_LOG "Accepted Connection! WOOOHOOOO!!!";

            //can thread this too
            //GetWorldTimerManager().SetTimer(this,
            //    &AVoxelSender::TCPSocketListener, 0.01, true);
        }
    }
}

//TCP Socket Listener
//void AVoxelSender::TCPSocketListener()
//{
    //~~~~~~~~~~~~~
    //if (!ConnectionSocket) return;
    ////~~~~~~~~~~~~~


    ////Binary Array!
    //TArray<TCHAR> ReceivedData;

    //uint32 Size;
    //while (ConnectionSocket->HasPendingData(Size))
    //{
    //    ReceivedData.Init(0, FMath::Min(Size, 65507u));

    //    int32 Read = 0;
    //    ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

    //    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
    //}
    ////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    //if (ReceivedData.Num()) AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));


    //    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //    //                      String From Binary Array
    //    const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
    //    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    //    VShow("As String!!!!! ~>", ReceivedUE4String);
    //    if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
//}

bool AVoxelSender::SendVoxel(FVector pos, FVector color)
{
    if (!ConnectionSocket) return false;
    int32 BytesSent;

    FTimespan waitTime = FTimespan(10);

    int32 size = sizeof(float) * 6;
    float buffer[6];

    buffer[0] = pos.X;
    buffer[1] = pos.Y;
    buffer[2] = pos.Z;
    buffer[3] = color.X;
    buffer[4] = color.Y;
    buffer[5] = color.Z;

    bool success = ConnectionSocket->Send((uint8*)buffer, size, BytesSent);
//    UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);

    if (success && BytesSent > 0) return true;
    else return false;
}