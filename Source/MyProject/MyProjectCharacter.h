// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Collectible.h"
#include "DetectionWidget.h"
#include "Engine/TriggerBox.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MyProjectCharacter.generated.h"


struct FInputActionValue;
class UPointLightComponent;
class UMyPlayerStats;
class UMyUserWidget;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USoundCue;
DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AMyProjectCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShadowVisionAction;

	//temp tp
	class AShadowTP* CurrentTP;
	//light and tp
	class UWidgetComponent* CurrentLightWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"))
    UPointLightComponent* CurrentPointLight;
	void ResetTP();
	void ResetPointLight();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testing, meta = (AllowPrivateAccess = "true"))
	bool bSaveLoadEnabled;
public:
	AMyProjectCharacter();

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult) override;

	void Sprint();
	void UnSprint();
	void MyCrouch();
	void CastRayForInteraction();
	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShadowToggle)
	bool bIsShadowMode;
	void ToggleShadowVision();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = ShadowToggle)
	void SetShadowVision(bool b);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Light)
	bool bIsPlayerInLight;

	//collectibles
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Collectibles)
	int CountOfCollectiblesCollected = -1;
	void CollectibleCollided();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Collectibles)
	void UpdateCollectibleDisplay();

	//npc ai
	void SpottedByNPC(double Angle);
    void NPCLostSight();
    void HeardByNPC();
	bool IsPlayerInLight() const;
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetIsPlayerInLight(bool b);

	//detection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	TObjectPtr<UDetectionWidget> DetectionWidget;
	bool bIsPlayerSpotted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float SpottedTimeforWalk = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
	float SpottedTimeforSprint = 2.5f;
	void StartDetecting(float DeltaSeconds);

	//game over
	UFUNCTION(BlueprintImplementableEvent, Category ="GameOverSpotted")
	void GameEnd();
	UPROPERTY(EditDefaultsOnly, Category = "GameOverSpotted")
	TObjectPtr<USoundWave> GameOverSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GameOverSpotted")
	float Volume;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = GameOver)
	bool bSpottedGameOver;
	UFUNCTION(BlueprintCallable, Category = GameOver)
	void GameOverSpotted();
	UFUNCTION(BlueprintCallable, Category = GameOver)
	void GameOverReachedEnd();

	UFUNCTION(BlueprintCallable, Category = GameEnd)
	void MakePlayerLookAtExit();
	void RotatePlayerToExit(float DeltaSeconds);
	float t, T;
	bool bRotatePlayerToExit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameEnd)
	TObjectPtr<AActor> EndGameIndicator;


	//crouch
	void CrouchMovement(float DeltaSeconds);
	void TPMovement();
	void RotateWidgetTowardsPlayer();
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	FVector CrouchEyeOffset = FVector::ZeroVector;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	float CrouchSpeed = 6.f;

	//audio
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TObjectPtr<UAudioComponent> FootstepAudioComponent;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> WalkSoundCue;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundWave> TPSound;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> LightExtinguishCue;
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundCue> NoChargeCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TObjectPtr<UMyUserWidget> MyUserWidget;

	//pause
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pause)
	bool bIsPauseMenuOpen;

	//save load
	UPROPERTY(EditAnywhere, Category = SaveLoad)
	TArray<ATriggerBox*> Triggers;
	UPROPERTY(EditAnywhere, Category = SaveLoad)
	TArray<ACollectible*> Collectibles;

	UFUNCTION(BlueprintCallable, Category = SaveLoad)
	void SetTriggers(ATriggerBox* Trigger);
	UFUNCTION(BlueprintCallable, Category = SaveLoad)
	void SetCollectibles(ACollectible* Collectible);

	TArray<ATriggerBox*> GetTriggers();
	TArray<ACollectible*> GetCollectibles();


	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = Waypoints)
	FVector ExitDestination;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float MoveSpeed = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	float SprintSpeed = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
	float Loudness = 1.1f;
	FName TagForPlayerSound = TEXT("Player Footsteps");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool isSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Raycast")
	float RayLength = 2000.f;

	//teleporting
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TP")
	bool bCanTP;
	float StartingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TP")
	float TPDuration = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TP")
	float TPThreshold = 300.f; //threshold value of when player should stop before reaching teleport
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TP")
    TSubclassOf<UCameraShakeBase> TPCameraShake;
	bool bIsTping;
	float StartTime;
	FVector StartLocation;
	FHitResult Hit;

	//light
	bool bCanExtinguishLight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<UCameraShakeBase> LightCameraShake;
	//stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stats)
	TObjectPtr<UMyPlayerStats> MyPlayerStats;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	//temp bool, fix this later
	bool bIsWidgetSet;

private:

};

