
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Collectible.h"
#include "MyProjectCharacter.h"
#include "Engine/TriggerBox.h"
#include "MySaveGame.h"

void UMyGameInstance::Init()
{
    //Super::Init();
    UE_LOG(LogTemp, Warning, TEXT("Game Instance init"));
    //LoadGame(); //cannot call here since controller is not yet initialized
}

void UMyGameInstance::CreateSaveFile() 
{
    UE_LOG(LogTemp, Warning, TEXT("Save file created"));
    UMySaveGame* SaveData = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
    UGameplayStatics::SaveGameToSlot(SaveData, "Slot 1", 0);
    bGameIsFirstTime = true;
    LoadGame();
}

void UMyGameInstance::SaveGame()
{
    //if save data exists, overwrite 
    if(UMySaveGame* SaveData = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("Slot 1", 0)))
    {
        APlayerController* PlayerController = GetFirstGamePlayer()->GetPlayerController(GetWorld());
        APawn* PlayerPawn = PlayerController->GetPawn();
        AMyProjectCharacter* Player = Cast<AMyProjectCharacter>(PlayerPawn);

        SaveData->Triggers = Player->GetTriggers();
        SaveData->Collectibles = Player->GetCollectibles();
        SaveData->SavedPlayerPos = Player->GetActorLocation();
        SaveData->CountOfCollectiblesCollected = Player->CountOfCollectiblesCollected;
        SaveData->bIsBeforeFirstSave = false; //will be false after first save
        UGameplayStatics::SaveGameToSlot(SaveData, "Slot 1", 0);
        UE_LOG(LogTemp, Warning, TEXT("Game saved"));
    }
    //else create new save file
    else if(!UGameplayStatics::DoesSaveGameExist("Slot 1",0)) 
    {
        CreateSaveFile();
    }
}

void UMyGameInstance::LoadGame()
{
    //if save data exists, load values from that 
    if(UMySaveGame* SaveData = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot("Slot 1", 0)))
    {
        if (APlayerController* PlayerController = GetFirstGamePlayer()->GetPlayerController(GetWorld()))
        {
            APawn* PlayerPawn = PlayerController->GetPawn();
            AMyProjectCharacter* Player = Cast<AMyProjectCharacter>(PlayerPawn);
            if(SaveData->SavedPlayerPos==FVector::Zero())
            {
                Player->SetActorLocation(_PLAYER_START_LOCATION);
            }
            else
            {
                Player->SetActorLocation(SaveData->SavedPlayerPos);
            }
            Player->CountOfCollectiblesCollected = SaveData->CountOfCollectiblesCollected;
            bGameIsFirstTime = SaveData->bIsBeforeFirstSave;
            UE_LOG(LogTemp, Warning, TEXT("trigger count %d"), SaveData->Triggers.Num());

            for (auto& Element : SaveData->Triggers)
            {
                UE_LOG(LogTemp, Warning, TEXT("inside trig for"));
                if (Element)
                {
                    Element->Destroy();
                    UE_LOG(LogTemp, Warning, TEXT("found trig"));
                }
            }

            UE_LOG(LogTemp, Warning, TEXT("collec count %d"), SaveData->Collectibles.Num());
            for (auto& Element : SaveData->Collectibles)
            {
                UE_LOG(LogTemp, Warning, TEXT("inside collect for"));
                if (Element)
                {
                    Element->Destroy();
                    UE_LOG(LogTemp, Warning, TEXT("found trig"));
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("controller null"));
        }  
        UE_LOG(LogTemp, Warning, TEXT("Game loaded"));
    }
    //else create and setup new save file
    else if (!UGameplayStatics::DoesSaveGameExist("Slot 1", 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("No savefile found. Creating new save file."));
        bGameIsFirstTime = true;
        CreateSaveFile();
    }

}
