// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MyGameInstance.h"
#include "MyPlayerStats.h"
#include "ShadowTP.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"
DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//this class is from the 1st person c++ template of UE5, which i used as a base starting point and
//modified and added code for my game

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	//stats
	MyPlayerStats = CreateDefaultSubobject<UMyPlayerStats>(TEXT("MyStats"));
	
	TPThreshold = 300.f;

	StartingSpeed = MoveSpeed;

	//footsteps audio setup
	FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComponent"));
	FootstepAudioComponent->SetupAttachment(RootComponent);
	FootstepAudioComponent->bAutoActivate = false;
	FootstepAudioComponent->VolumeMultiplier = 0.7f;
}

void AMyProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context and load game
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (auto GameInstance = Cast<UMyGameInstance>(PlayerController->GetGameInstance()))
		{
			if (bSaveLoadEnabled) //DEV VARIABLE
				GameInstance->LoadGame();
		}
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		};
	}

	//setup pointers and values which will be used dynamically
	CurrentTP = nullptr;
	CurrentPointLight = nullptr;
	CurrentLightWidget = nullptr;
	bSpottedGameOver = false;

	CountOfCollectiblesCollected = -1;
}

void AMyProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if (bRotatePlayerToExit) //if player reached exit hint trigger
	{
		RotatePlayerToExit(DeltaSeconds);
	}
	if (bSpottedGameOver)
	{
		GameOverSpotted();
		return;
	}
	if (MyUserWidget && !bIsWidgetSet) //basic HUD with shadow charges
	{
		bIsWidgetSet = true;
		MyPlayerStats->SetUserWidget(MyUserWidget);
	}
	if(bIsPlayerSpotted && DetectionWidget) //if spotted by ai, called by AI Controller
	{
		StartDetecting(DeltaSeconds);
	}
	else if(!bIsPlayerSpotted && DetectionWidget) //else when not spotted reset detection meter
	{
		DetectionWidget->ResetProgress((100 / SpottedTimeforWalk) * DeltaSeconds);
	}
	if(!bIsPlayerInLight && DetectionWidget)
	{
		DetectionWidget->ResetProgress((100 / SpottedTimeforWalk) * DeltaSeconds);
	}
	//crouch
	CrouchMovement(DeltaSeconds);

	//interaction raycast
	CastRayForInteraction();

	//if player currently teleporting logic
	if(bIsTping)
		TPMovement();

	//if player currently can extinguish a fire and looking at it
	if(bCanExtinguishLight)
		RotateWidgetTowardsPlayer();
}

void AMyProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyProjectCharacter::MyCrouch);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMyProjectCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyProjectCharacter::UnSprint);

		//TP
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMyProjectCharacter::Interact);

		//Shadow vision
		EnhancedInputComponent->BindAction(ShadowVisionAction, ETriggerEvent::Started, this, &AMyProjectCharacter::ToggleShadowVision);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMyProjectCharacter::Move(const FInputActionValue& Value)
{
	if (bSpottedGameOver)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y * MoveSpeed);
		AddMovementInput(GetActorRightVector(), MovementVector.X * MoveSpeed);
	    if (MovementVector.Y != 0.0f || MovementVector.X != 0.0f)
		{
			FootstepAudioComponent->Sound = WalkSoundCue;
			if (isSprinting)
			{
				//UE_LOG(LogTemp, Warning, TEXT("SPRINTING"));
				FootstepAudioComponent->SetPitchMultiplier(SprintSpeed*2.45f);
				Loudness = 1.75f; //increased loudness when sprinting
			}
			else if (WalkSoundCue)
			{
				//UE_LOG(LogTemp, Warning, TEXT("WALKING"));
				FootstepAudioComponent->SetPitchMultiplier(MoveSpeed*2.45f); // Reset pitch for walking
				Loudness = 0.8f; //decreased loudness when walking
			}

			if (!FootstepAudioComponent->IsPlaying())
			{
				//UE_LOG(LogTemp, Warning, TEXT("PLAYING"));
				FootstepAudioComponent->Play();
			}
			if(!bIsCrouched)
			    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this, 3000.f, TagForPlayerSound); //report footsteps event to AI
		}
	}
}

void AMyProjectCharacter::Look(const FInputActionValue& Value)
{
	if (bSpottedGameOver)
		return;
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

//smooth crouch transitions
void AMyProjectCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) 
{
	if (HalfHeightAdjust == 0.0f)
		return;

	float const StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z = StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AMyProjectCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.0f)
		return;
	float const StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	CrouchEyeOffset.Z = StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;
	FirstPersonCameraComponent->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
}

void AMyProjectCharacter::CrouchMovement(float DeltaSeconds)
{
	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaSeconds);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
	if ((GetVelocity().X == 0 && GetVelocity().Y == 0) || bIsCrouched)
		FootstepAudioComponent->Stop();
}

void AMyProjectCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	//Super::CalcCamera(DeltaTime, OutResult);
	if (FirstPersonCameraComponent)
	{
		FirstPersonCameraComponent->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

//toggle crouch
void AMyProjectCharacter::MyCrouch()
{
	if (bSpottedGameOver)
		return;
	if(!bIsCrouched)
	    Crouch();
	else
		UnCrouch();
}

//toggle sprint
void AMyProjectCharacter::Sprint()
{
	if (bIsCrouched)
		UnCrouch();
	MoveSpeed = SprintSpeed;
	isSprinting = true;
	//UE_LOG(LogTemp, Warning, TEXT("sprinting %f"), MoveSpeed);
}

void AMyProjectCharacter::UnSprint()
{
    MoveSpeed = StartingSpeed;
	isSprinting = false;
	//UE_LOG(LogTemp, Warning, TEXT("not sprinting %f"), MoveSpeed);
}

void AMyProjectCharacter::CastRayForInteraction()
{
	if (bIsTping) //stop casting ray when teleporting
	{
		bCanTP = false;
		return;
	}
	//shoot ray from player
	FVector Start = GetActorLocation() + FVector(0, 0, 10.f);
	FVector Dir = FirstPersonCameraComponent->GetForwardVector();
	Start = FVector(Start.X + (Dir.X * 100), Start.Y + (Dir.Y * 100), Start.Z + (Dir.Z * 150));
	FVector End = Start + (Dir * RayLength);
	bool ActorHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel1, FCollisionQueryParams(), FCollisionResponseParams());

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0.f, 5.f);
	//if (Hit.GetActor())
		//UE_LOG(LogTemp, Warning, TEXT("obj %s"), *Hit.GetActor()->GetActorNameOrLabel());
	//if no actor hit, reset values
	if(!ActorHit)
	{
		ResetTP();
		ResetPointLight();
		//UE_LOG(LogTemp, Warning, TEXT("no hit"));
	}
	else if (ActorHit && Hit.GetActor()->IsA<AShadowTP>()) //if actor hit was a TP
	{
		ResetPointLight();//reset light pointer if any
		CurrentTP = Cast<AShadowTP>(Hit.GetActor());
		if (!CurrentTP->CanBeUsed()) //if the hit tp cannot be used
		{
			bCanTP = false;
		}
		else
		{
			bCanTP = true;
			CurrentTP->EnableTP();
			CurrentTP->isRayCasting(true, GetActorLocation()); //show widget at TP
		}
	}
	else if (ActorHit && Hit.GetActor()->GetComponentByClass<UPointLightComponent>())//if actor hit was a light
	{
		ResetTP();//reset tp pointer if any
		if (!Hit.GetActor()->GetActorEnableCollision())//if the hit light cannot be used
			return;
		if (Hit.GetActor()->GetComponentByClass<UWidgetComponent>())//if the hit light has desired widget
		{
			CurrentLightWidget = Hit.GetActor()->GetComponentByClass<UWidgetComponent>();
			CurrentLightWidget->SetVisibility(true);
		}
		else
			return;
		CurrentPointLight = (Hit.GetActor()->GetComponentByClass<UPointLightComponent>());//set current light pointer to hit light
		bCanExtinguishLight = true;
	}
	if (CurrentTP && Hit.GetActor() != CurrentTP)
		ResetTP();
}

void AMyProjectCharacter::ResetTP() //reset current tp pointer and values
{
	bCanTP = false;
	if (CurrentTP)
	{
		CurrentTP->DisableTP();
		CurrentTP->isRayCasting(false, FVector::Zero());
		CurrentTP = nullptr;
	}
}

void AMyProjectCharacter::ResetPointLight()//reset current light pointer and values
{
	bCanExtinguishLight = false;
	if (CurrentPointLight)
	{
		CurrentLightWidget->SetVisibility(false);
		CurrentLightWidget = nullptr;
		CurrentPointLight = nullptr;
	}
}

void AMyProjectCharacter::Interact() //when interaction button pressed
{
	if (bSpottedGameOver)
		return;
    if (!MyPlayerStats || MyPlayerStats->GetCharges() <= 0) 
    {
		if (NoChargeCue)
			UGameplayStatics::PlaySound2D(this, NoChargeCue);
		return; //early return if no charges
    }
	//if player looking at TP
	if (bCanTP)
	{
		MyPlayerStats->UseCharge();
		bIsTping = true;
		if(CurrentTP)
		{
			CurrentTP->DisableTP(); //disable the tp player just used
		}
		//play sound
		if(TPSound)
		    UGameplayStatics::PlaySound2D(this, TPSound);
		bCanTP = false;
		//camera shake
		StartTime = GetWorld()->GetTimeSeconds();
	    StartLocation = GetActorLocation();
		UGameplayStatics::PlayWorldCameraShake(this, TPCameraShake, GetActorLocation(), 0, 500);
	}
	//if player looking at light
	else if(bCanExtinguishLight)
	{
		MyPlayerStats->UseCharge();
		bCanExtinguishLight = false;
		//UE_LOG(LogTemp, Warning, TEXT("light extinguished"));
		//camera shake
		UGameplayStatics::PlayWorldCameraShake(this, LightCameraShake, GetActorLocation(), 0, 500);

		if (LightExtinguishCue && CurrentPointLight)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), LightExtinguishCue, CurrentPointLight->GetOwner()->GetActorLocation()); //play sound from light so AI can also hear
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), Loudness, this, 800.f); //report noise to AI
		}

		if (bIsPlayerInLight)
			SetIsPlayerInLight(false);
		//UE_LOG(LogTemp, Warning, TEXT("in light : %hhd"), bIsPlayerInLight);
		//disable this light so cannot be used again
		CurrentPointLight->SetIntensity(0.f);
		CurrentPointLight->GetOwner()->SetActorTickEnabled(false);
		CurrentPointLight->GetOwner()->SetActorEnableCollision(false);
		ResetPointLight();
	}
}

void AMyProjectCharacter::ToggleShadowVision() //switch to shadow vision mode
{
	bIsShadowMode = !bIsShadowMode;
	//UE_LOG(LogTemp, Warning, TEXT("shadow mode : %hhd"), bIsShadowMode);
	SetShadowVision(bIsShadowMode);
}

void AMyProjectCharacter::CollectibleCollided() //called when player has collided with a scroll/collectible
{
	CountOfCollectiblesCollected++;
    UpdateCollectibleDisplay();
	//if(CountOfCollectiblesCollected==MyPlayerStats->GetCollectiblesCount())
	//{
	    //all collected
	//}
}

void AMyProjectCharacter::SpottedByNPC(double Angle) //called when spotted by ai and starts detecting
{
	bIsPlayerSpotted = true;
	DetectionWidget->UpdateAngle(Angle);
}

void AMyProjectCharacter::NPCLostSight() //called when AI lost sight of player and detection starts reseting
{
	bIsPlayerSpotted = false;
}

void AMyProjectCharacter::StartDetecting(float DeltaSeconds) 
{
	//if player is sprinting detection is faster
	if (isSprinting)
		DetectionWidget->UpdateProgress((100 / SpottedTimeforSprint) * DeltaSeconds);
	else
		DetectionWidget->UpdateProgress((100 / SpottedTimeforWalk) * DeltaSeconds);
}

void AMyProjectCharacter::HeardByNPC() //logic handled in AI Controller
{
	//UE_LOG(LogTemp, Warning, TEXT("heard by npc"));
}

bool AMyProjectCharacter::IsPlayerInLight() const
{
	return bIsPlayerInLight;
}

void AMyProjectCharacter::SetIsPlayerInLight(bool b)
{
	bIsPlayerInLight = b;
}

void AMyProjectCharacter::SetTriggers(ATriggerBox* Trigger) //add trigger when collided with player to save to save file
{
	Triggers.Add(Trigger);
}

void AMyProjectCharacter::SetCollectibles(ACollectible* Collectible)//add collectible when collided with player to save to save file
{
	Collectibles.Add(Collectible);
}

TArray<ATriggerBox*> AMyProjectCharacter::GetTriggers()
{
	return Triggers;
}

TArray<ACollectible*> AMyProjectCharacter::GetCollectibles()
{
	return Collectibles;
}

void AMyProjectCharacter::GameOverSpotted()
{
	FootstepAudioComponent->Stop();
	DisableInput(Cast<APlayerController>(GetController()));
	//play audio
	UGameplayStatics::PlaySound2D(GetWorld(), GameOverSound, Volume);
	GameEnd();
}
void AMyProjectCharacter::GameOverReachedEnd()
{
	FootstepAudioComponent->Stop();
	DisableInput(Cast<APlayerController>(GetController()));
}

void AMyProjectCharacter::MakePlayerLookAtExit()
{
	bRotatePlayerToExit = true;
}

void AMyProjectCharacter::RotatePlayerToExit(float DeltaSeconds) //called when player reached near exit
{
	DisableInput(Cast<APlayerController>(GetController()));
	FootstepAudioComponent->Stop();
	if(T<2 && EndGameIndicator) //rotate player in 2 secs
	{
		EndGameIndicator->SetActorHiddenInGame(false);
		T += DeltaSeconds;
		t = T / 2;
		FRotator TargetRotation = FMath::Lerp(GetActorRotation(), EndGameIndicator->GetActorRotation(), t);
		TargetRotation *= -1;
		GetController()->SetControlRotation(TargetRotation);
	}
	else //then give back control
	{
		bRotatePlayerToExit = false;
		EnableInput(Cast<APlayerController>(GetController()));
	}
}

void AMyProjectCharacter::TPMovement() //when player uses a teleport
{
	//slide player to TP over time
	DisableInput(Cast<APlayerController>(GetController()));
	float Alpha = (GetWorld()->GetTimeSeconds() - StartTime) / TPDuration;
	Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f);
	FVector NewLocation = FMath::Lerp(StartLocation, Hit.GetActor()->GetActorLocation(), Alpha);
	SetActorLocation(NewLocation);
	if (FVector::Distance(GetActorLocation(), Hit.GetActor()->GetActorLocation()) < TPThreshold) //when reached threshold, stop
	{
		bIsTping = false;
		EnableInput(Cast<APlayerController>(GetController()));
	}
}

void AMyProjectCharacter::RotateWidgetTowardsPlayer() //rotate current light widget to always face player
{
	FVector Direction = GetActorLocation() - CurrentLightWidget->GetComponentLocation();
	FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CurrentLightWidget->GetComponentLocation(), GetActorLocation());
	CurrentLightWidget->SetWorldRotation(Rot);
}
