// Copyright Epic Games, Inc. All Rights Reserved.

#include "P02PlaytestSystem.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HAL/IConsoleManager.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogP02Playtest, Log, All);

namespace P02PlaytestCVars
{
	static TAutoConsoleVariable<int32> Enabled(
		TEXT("p02.Playtest.Enabled"), 1,
		TEXT("Enable the P02 PIE playtest camera, dash, attack, health, and measurement layer."));

	static TAutoConsoleVariable<int32> SpawnTestCourse(
		TEXT("p02.Playtest.TestCourse"), 1,
		TEXT("Spawn the transient MCP_TEST course in Lvl_ThirdPerson PIE only."));

	static TAutoConsoleVariable<float> CameraPitch(TEXT("p02.Camera.Pitch"), -50.0f, TEXT("Quarter-view camera pitch."));
	static TAutoConsoleVariable<float> CameraYaw(TEXT("p02.Camera.Yaw"), 0.0f, TEXT("Quarter-view camera yaw."));
	static TAutoConsoleVariable<float> CameraDistance(TEXT("p02.Camera.Distance"), 1700.0f, TEXT("Quarter-view camera distance."));
	static TAutoConsoleVariable<float> CameraFOV(TEXT("p02.Camera.FOV"), 45.0f, TEXT("Quarter-view perspective FOV."));

	static TAutoConsoleVariable<float> DashDistance(TEXT("p02.Dash.Distance"), 450.0f, TEXT("Dash travel distance in uu."));
	static TAutoConsoleVariable<float> DashSpeed(TEXT("p02.Dash.Speed"), 1600.0f, TEXT("Dash speed in uu/s."));
	static TAutoConsoleVariable<float> DashCooldown(TEXT("p02.Dash.Cooldown"), 0.8f, TEXT("Dash cooldown in seconds."));

	static TAutoConsoleVariable<float> AttackDamage(TEXT("p02.Attack.Damage"), 34.0f, TEXT("Transient test attack damage."));
	static TAutoConsoleVariable<float> AttackRange(TEXT("p02.Attack.Range"), 140.0f, TEXT("Transient test attack forward offset."));
	static TAutoConsoleVariable<float> AttackRadius(TEXT("p02.Attack.Radius"), 170.0f, TEXT("Transient test attack radius."));
	static TAutoConsoleVariable<float> AttackCooldown(TEXT("p02.Attack.Cooldown"), 0.35f, TEXT("Transient test attack cooldown."));
	static TAutoConsoleVariable<float> PlayerMaxHealth(TEXT("p02.Player.MaxHealth"), 100.0f, TEXT("Transient test player health."));
}

AP02QuarterViewCameraRig::AP02QuarterViewCameraRig()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("QuarterViewSpringArm"));
	SpringArm->SetupAttachment(SceneRoot);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = false;
	SpringArm->ProbeSize = 18.0f;
	SpringArm->ProbeChannel = ECC_Camera;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("QuarterViewCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	ApplyCameraSettings(CameraPitch, CameraYaw, CameraDistance, CameraFOV);
}

void AP02QuarterViewCameraRig::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TrackedPawn.IsValid())
	{
		SetActorLocation(TrackedPawn->GetActorLocation());
	}
}

void AP02QuarterViewCameraRig::SetTrackedPawn(APawn* InPawn)
{
	TrackedPawn = InPawn;
	if (InPawn)
	{
		SetActorLocation(InPawn->GetActorLocation());
	}
}

float AP02QuarterViewCameraRig::GetActualCameraDistance() const
{
	return Camera ? FVector::Distance(Camera->GetComponentLocation(), GetActorLocation()) : 0.0f;
}

bool AP02QuarterViewCameraRig::IsCameraCollisionFixApplied() const
{
	return SpringArm && SpringArm->IsCollisionFixApplied();
}

void AP02QuarterViewCameraRig::ApplyCameraSettings(
	float InPitch, float InYaw, float InDistance, float InFOV)
{
	CameraPitch = InPitch;
	CameraYaw = InYaw;
	CameraDistance = FMath::Max(100.0f, InDistance);
	CameraFOV = FMath::Clamp(InFOV, 20.0f, 120.0f);
	bEnableCameraCollision = false;

	if (SpringArm)
	{
		SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.0f));
		SpringArm->TargetArmLength = CameraDistance;
		SpringArm->bDoCollisionTest = false;
	}
	if (Camera)
	{
		Camera->SetFieldOfView(CameraFOV);
	}
}

AP02PlaytestDoor::AP02PlaytestDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(SceneRoot);
	DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DoorMesh->SetStaticMesh(CubeMesh.Object);
	}
	DoorMesh->SetRelativeScale3D(DoorScale);
}

void AP02PlaytestDoor::BeginPlay()
{
	Super::BeginPlay();
	ClosedRelativeLocation = DoorMesh->GetRelativeLocation();
	DoorMesh->SetRelativeScale3D(DoorScale);
	SetDoorOpen(false);
}

void AP02PlaytestDoor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FVector TargetLocation = ClosedRelativeLocation + (bOpen ? OpenOffset : FVector::ZeroVector);
	const FVector NewLocation = FMath::VInterpConstantTo(
		DoorMesh->GetRelativeLocation(), TargetLocation, DeltaSeconds, OpenSpeed);
	DoorMesh->SetRelativeLocation(NewLocation);

	const bool bFullyOpen = bOpen && NewLocation.Equals(TargetLocation, 1.0f);
	DoorMesh->SetCollisionProfileName(bFullyOpen ? TEXT("NoCollision") : TEXT("BlockAll"));
}

void AP02PlaytestDoor::SetDoorOpen(bool bShouldOpen)
{
	bOpen = bShouldOpen;
	if (!bOpen)
	{
		DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
	}
	UE_LOG(LogP02Playtest, Display, TEXT("Door %s -> %s"), *GetName(), bOpen ? TEXT("Open") : TEXT("Closed"));
}

AP02PlaytestEnemy::AP02PlaytestEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));
	RootComponent = EnemyMesh;
	EnemyMesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		EnemyMesh->SetStaticMesh(CubeMesh.Object);
	}
	EnemyMesh->SetRelativeScale3D(FVector(0.8f, 0.8f, 1.6f));
	SetActorEnableCollision(true);
}

void AP02PlaytestEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = FMath::Max(1.0f, MaxHealth);
}

void AP02PlaytestEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bEncounterActive || CurrentHealth <= 0.0f)
	{
		return;
	}

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player)
	{
		return;
	}

	FVector ToPlayer = Player->GetActorLocation() - GetActorLocation();
	ToPlayer.Z = 0.0f;
	const float Distance = ToPlayer.Size();
	if (Distance > AttackRange)
	{
		const FVector Direction = ToPlayer.GetSafeNormal();
		const float MoveDistance = FMath::Min(MoveSpeed * DeltaSeconds, Distance - AttackRange);
		FHitResult Hit;
		AddActorWorldOffset(Direction * MoveDistance, true, &Hit);
		SetActorRotation(Direction.Rotation());
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now >= NextAttackTime)
	{
		NextAttackTime = Now + AttackCooldown;
		if (UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>())
		{
			Playtest->ApplyPlayerDamage(AttackDamage);
		}
	}
}

float AP02PlaytestEnemy::TakeDamage(
	float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = FMath::Max(0.0f, DamageAmount);
	CurrentHealth = FMath::Clamp(CurrentHealth - AppliedDamage, 0.0f, MaxHealth);
	UE_LOG(LogP02Playtest, Display, TEXT("Enemy %s damaged %.1f, HP %.1f / %.1f"),
		*GetName(), AppliedDamage, CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}
	return AppliedDamage;
}

AP02EncounterTrigger::AP02EncounterTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EncounterTrigger"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(180.0f, 420.0f, 180.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AP02EncounterTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AP02EncounterTrigger::HandleTriggerOverlap);
	BindEnemies();
	if (LinkedDoor)
	{
		LinkedDoor->SetDoorOpen(false);
	}
}

void AP02EncounterTrigger::ConfigureRuntimeTest(
	const TArray<AP02PlaytestEnemy*>& InEnemies, AP02PlaytestDoor* InDoor)
{
	EncounterEnemies.Reset();
	for (AP02PlaytestEnemy* Enemy : InEnemies)
	{
		EncounterEnemies.Add(Enemy);
	}
	LinkedDoor = InDoor;
	BindEnemies();
	if (LinkedDoor)
	{
		LinkedDoor->SetDoorOpen(false);
	}
}

void AP02EncounterTrigger::BindEnemies()
{
	for (AP02PlaytestEnemy* Enemy : EncounterEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->OnDestroyed.RemoveDynamic(this, &AP02EncounterTrigger::HandleEnemyDestroyed);
			Enemy->OnDestroyed.AddDynamic(this, &AP02EncounterTrigger::HandleEnemyDestroyed);
		}
	}
}

void AP02EncounterTrigger::ActivateEncounter()
{
	if (bActivated || bCleared)
	{
		return;
	}

	bActivated = true;
	for (AP02PlaytestEnemy* Enemy : EncounterEnemies)
	{
		if (IsValid(Enemy))
		{
			Enemy->SetEncounterActive(true);
		}
	}

	const int32 Remaining = GetRemainingEnemies();
	if (UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>())
	{
		Playtest->NotifyEncounterStarted(Remaining);
	}
	if (Remaining == 0)
	{
		ClearEncounter();
	}
}

int32 AP02EncounterTrigger::GetRemainingEnemies() const
{
	int32 Remaining = 0;
	for (const AP02PlaytestEnemy* Enemy : EncounterEnemies)
	{
		Remaining += IsValid(Enemy) ? 1 : 0;
	}
	return Remaining;
}

void AP02EncounterTrigger::HandleTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APawn>(OtherActor))
	{
		ActivateEncounter();
	}
}

void AP02EncounterTrigger::HandleEnemyDestroyed(AActor* DestroyedActor)
{
	EncounterEnemies.Remove(Cast<AP02PlaytestEnemy>(DestroyedActor));
	const int32 Remaining = GetRemainingEnemies();
	if (UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>())
	{
		Playtest->NotifyEncounterUpdated(Remaining);
	}
	if (Remaining == 0)
	{
		ClearEncounter();
	}
}

void AP02EncounterTrigger::ClearEncounter()
{
	if (bCleared)
	{
		return;
	}

	bCleared = true;
	if (LinkedDoor)
	{
		LinkedDoor->SetDoorOpen(true);
	}
	if (UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>())
	{
		Playtest->NotifyEncounterCleared();
	}
}

AP02StartMarker::AP02StartMarker()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneRoot;
}

AP02EndTrigger::AP02EndTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EndTrigger"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(160.0f, 420.0f, 200.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AP02EndTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AP02EndTrigger::HandleEndOverlap);
}

void AP02EndTrigger::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggered || !Cast<APawn>(OtherActor))
	{
		return;
	}
	if (RequiredEncounter && !RequiredEncounter->IsCleared())
	{
		UE_LOG(LogP02Playtest, Display, TEXT("END blocked: encounter is not clear"));
		return;
	}

	bTriggered = true;
	if (UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>())
	{
		Playtest->CompleteRun();
	}
}

bool UP02PlaytestSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	const UWorld* World = Cast<UWorld>(Outer);
	return World && (World->WorldType == EWorldType::PIE || World->WorldType == EWorldType::Game);
}

void UP02PlaytestSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	RunStartTime = InWorld.GetTimeSeconds();
	UE_LOG(LogP02Playtest, Display, TEXT("P02 playtest layer started in %s"), *InWorld.GetMapName());
}

void UP02PlaytestSubsystem::Tick(float DeltaSeconds)
{
	if (P02PlaytestCVars::Enabled.GetValueOnGameThread() == 0)
	{
		return;
	}

	if (!bPlayerInitialized)
	{
		TryInitializePlayer();
		return;
	}

	if (!PlayerPawn.IsValid() || !PlayerController.IsValid())
	{
		bPlayerInitialized = false;
		return;
	}

	const float Pitch = P02PlaytestCVars::CameraPitch.GetValueOnGameThread();
	const float Yaw = P02PlaytestCVars::CameraYaw.GetValueOnGameThread();
	if (CameraRig.IsValid())
	{
		CameraRig->ApplyCameraSettings(
			Pitch,
			Yaw,
			P02PlaytestCVars::CameraDistance.GetValueOnGameThread(),
			P02PlaytestCVars::CameraFOV.GetValueOnGameThread());
	}
	PlayerController->SetControlRotation(FRotator(0.0f, Yaw, 0.0f));

	const FVector CurrentLocation = PlayerPawn->GetActorLocation();
	if (!bRunComplete)
	{
		TravelDistance += FVector::Distance(CurrentLocation, LastPlayerLocation);
	}
	LastPlayerLocation = CurrentLocation;

	TickDash(DeltaSeconds);
	TryBasicAttack();
	DrawPlaytestHUD();
}

TStatId UP02PlaytestSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UP02PlaytestSubsystem, STATGROUP_Tickables);
}

void UP02PlaytestSubsystem::TryInitializePlayer()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	if (!PC || !Pawn)
	{
		return;
	}

	PlayerController = PC;
	PlayerPawn = Pawn;
	StartLocation = Pawn->GetActorLocation();
	LastPlayerLocation = StartLocation;
	PlayerHealth = P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread();
	bPlayerInitialized = true;
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		// This quarter-view test layer has no jump. The template still binds Space
		// to Jump, so disable the character jump capability at runtime and reuse
		// Space as the primary dash input.
		Character->JumpMaxCount = 0;
		Character->StopJumping();
		Character->GetCharacterMovement()->JumpZVelocity = 0.0f;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = TEXT("MCP_TEST_QuarterViewCameraRig");
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AP02QuarterViewCameraRig* Rig = GetWorld()->SpawnActor<AP02QuarterViewCameraRig>(
		AP02QuarterViewCameraRig::StaticClass(), Pawn->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	if (Rig)
	{
		Rig->SetTrackedPawn(Pawn);
		CameraRig = Rig;
		PC->SetViewTarget(Rig);
	}

	const bool bShouldSpawnCourse =
		GetWorld()->WorldType == EWorldType::PIE &&
		P02PlaytestCVars::SpawnTestCourse.GetValueOnGameThread() != 0 &&
		GetWorld()->GetMapName().Contains(TEXT("Lvl_ThirdPerson"));
	if (bShouldSpawnCourse)
	{
		SpawnTransientTestCourse();
	}

	UE_LOG(LogP02Playtest, Display, TEXT("Player initialized: %s"), *Pawn->GetName());
}

void UP02PlaytestSubsystem::TickDash(float DeltaSeconds)
{
	ACharacter* Character = Cast<ACharacter>(PlayerPawn.Get());
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
	const float Now = GetWorld()->GetTimeSeconds();
	if (bIsDashing)
	{
		if (Now < DashEndTime)
		{
			const float ZVelocity = Movement->Velocity.Z;
			Movement->Velocity = DashDirection * P02PlaytestCVars::DashSpeed.GetValueOnGameThread();
			Movement->Velocity.Z = ZVelocity;
		}
		else
		{
			bIsDashing = false;
			Movement->Velocity.X = 0.0f;
			Movement->Velocity.Y = 0.0f;
		}
	}

	const bool bDashPressed =
		PlayerController->WasInputKeyJustPressed(EKeys::SpaceBar) ||
		PlayerController->WasInputKeyJustPressed(EKeys::LeftShift) ||
		PlayerController->WasInputKeyJustPressed(EKeys::Gamepad_FaceButton_Right);
	if (!bDashPressed || bIsDashing || bRunComplete || Now < NextDashTime)
	{
		return;
	}

	FVector Direction = Movement->Velocity;
	Direction.Z = 0.0f;
	if (Direction.SizeSquared() < FMath::Square(50.0f))
	{
		Direction = Character->GetActorForwardVector();
		Direction.Z = 0.0f;
	}
	DashDirection = Direction.GetSafeNormal();

	const float Speed = FMath::Max(1.0f, P02PlaytestCVars::DashSpeed.GetValueOnGameThread());
	const float Distance = FMath::Max(0.0f, P02PlaytestCVars::DashDistance.GetValueOnGameThread());
	DashEndTime = Now + (Distance / Speed);
	NextDashTime = Now + FMath::Max(0.0f, P02PlaytestCVars::DashCooldown.GetValueOnGameThread());
	bIsDashing = true;
	UE_LOG(LogP02Playtest, Display, TEXT("Dash started: direction=%s distance=%.1f speed=%.1f"),
		*DashDirection.ToCompactString(), Distance, Speed);
}

void UP02PlaytestSubsystem::TryBasicAttack()
{
	const bool bAttackPressed =
		PlayerController->WasInputKeyJustPressed(EKeys::LeftMouseButton) ||
		PlayerController->WasInputKeyJustPressed(EKeys::Gamepad_FaceButton_Bottom);
	const float Now = GetWorld()->GetTimeSeconds();
	if (!bAttackPressed || bRunComplete || Now < NextAttackTime || !PlayerPawn.IsValid())
	{
		return;
	}

	NextAttackTime = Now + FMath::Max(0.0f, P02PlaytestCVars::AttackCooldown.GetValueOnGameThread());
	const float Range = P02PlaytestCVars::AttackRange.GetValueOnGameThread();
	const float Radius = P02PlaytestCVars::AttackRadius.GetValueOnGameThread();
	const FVector AttackCenter = PlayerPawn->GetActorLocation() + PlayerPawn->GetActorForwardVector() * Range;
	int32 HitCount = 0;

	for (TActorIterator<AP02PlaytestEnemy> It(GetWorld()); It; ++It)
	{
		AP02PlaytestEnemy* Enemy = *It;
		if (IsValid(Enemy) && FVector::DistSquared(Enemy->GetActorLocation(), AttackCenter) <= FMath::Square(Radius))
		{
			UGameplayStatics::ApplyDamage(
				Enemy,
				P02PlaytestCVars::AttackDamage.GetValueOnGameThread(),
				PlayerController.Get(),
				PlayerPawn.Get(),
				nullptr);
			++HitCount;
		}
	}

	DrawDebugSphere(GetWorld(), AttackCenter, Radius, 16, HitCount > 0 ? FColor::Green : FColor::Yellow, false, 0.15f);
	UE_LOG(LogP02Playtest, Display, TEXT("Basic attack: hits=%d"), HitCount);
}

void UP02PlaytestSubsystem::NotifyEncounterStarted(int32 EnemyCount)
{
	bEncounterActive = true;
	bEncounterCleared = false;
	EncounterRemaining = EnemyCount;
	UE_LOG(LogP02Playtest, Display, TEXT("Encounter started at %.2fs, enemies=%d"), GetElapsedTime(), EnemyCount);
}

void UP02PlaytestSubsystem::NotifyEncounterUpdated(int32 EnemyCount)
{
	EncounterRemaining = EnemyCount;
}

void UP02PlaytestSubsystem::NotifyEncounterCleared()
{
	bEncounterActive = false;
	bEncounterCleared = true;
	EncounterRemaining = 0;
	UE_LOG(LogP02Playtest, Display, TEXT("Encounter cleared at %.2fs"), GetElapsedTime());
}

void UP02PlaytestSubsystem::CompleteRun()
{
	if (bRunComplete)
	{
		return;
	}
	bRunComplete = true;
	RunEndTime = GetWorld()->GetTimeSeconds();
	if (PlayerController.IsValid())
	{
		PlayerController->SetIgnoreMoveInput(true);
	}
	if (ACharacter* Character = Cast<ACharacter>(PlayerPawn.Get()))
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
	}
	UE_LOG(LogP02Playtest, Display, TEXT("Run complete: time=%.2fs distance=%.1fuu"),
		GetElapsedTime(), TravelDistance);
}

void UP02PlaytestSubsystem::ApplyPlayerDamage(float DamageAmount)
{
	if (bRunComplete)
	{
		return;
	}
	PlayerHealth = FMath::Clamp(PlayerHealth - FMath::Max(0.0f, DamageAmount), 0.0f,
		P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread());
	UE_LOG(LogP02Playtest, Display, TEXT("Player damaged %.1f, HP %.1f"), DamageAmount, PlayerHealth);
	if (PlayerHealth <= 0.0f)
	{
		ResetPlayerAfterDefeat();
	}
}

float UP02PlaytestSubsystem::GetElapsedTime() const
{
	if (!GetWorld())
	{
		return 0.0f;
	}
	const float EndTime = bRunComplete ? RunEndTime : GetWorld()->GetTimeSeconds();
	return FMath::Max(0.0f, EndTime - RunStartTime);
}

void UP02PlaytestSubsystem::ResetPlayerAfterDefeat()
{
	PlayerHealth = P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread();
	if (PlayerPawn.IsValid())
	{
		PlayerPawn->SetActorLocation(StartLocation, false, nullptr, ETeleportType::TeleportPhysics);
		if (ACharacter* Character = Cast<ACharacter>(PlayerPawn.Get()))
		{
			Character->GetCharacterMovement()->StopMovementImmediately();
		}
		LastPlayerLocation = StartLocation;
	}
	UE_LOG(LogP02Playtest, Warning, TEXT("Player defeated: health and position reset"));
}

void UP02PlaytestSubsystem::DrawPlaytestHUD() const
{
	if (!GEngine)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	const FString DashState = Now >= NextDashTime ? TEXT("READY") : FString::Printf(TEXT("%.1fs"), NextDashTime - Now);
	FString EncounterState = TEXT("IDLE");
	if (bEncounterCleared)
	{
		EncounterState = TEXT("CLEAR");
	}
	else if (bEncounterActive)
	{
		EncounterState = FString::Printf(TEXT("ACTIVE (%d)"), EncounterRemaining);
	}

	const FString HUDText = FString::Printf(
		TEXT("P02 PLAYTEST | HP %.0f/%.0f | Time %.1fs | Distance %.0fuu | Cam %.0f/%.0f%s | Dash %s | Encounter %s\n")
		TEXT("Move: WASD/Stick  Dash: Space/Shift/B  Attack: LMB/A  Jump: Disabled"),
		PlayerHealth,
		P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread(),
		GetElapsedTime(),
		TravelDistance,
		CameraRig.IsValid() ? CameraRig->GetActualCameraDistance() : 0.0f,
		P02PlaytestCVars::CameraDistance.GetValueOnGameThread(),
		CameraRig.IsValid() && CameraRig->IsCameraCollisionFixApplied() ? TEXT(" CLAMP") : TEXT(""),
		*DashState,
		*EncounterState);
	GEngine->AddOnScreenDebugMessage(2200201, 0.1f, FColor::Cyan, HUDText);

	if (bRunComplete)
	{
		GEngine->AddOnScreenDebugMessage(
			2200202, 0.1f, FColor::Green,
			FString::Printf(TEXT("CLEAR | %.2fs | %.0fuu"), GetElapsedTime(), TravelDistance));
	}
}

AActor* UP02PlaytestSubsystem::SpawnTestBlock(
	const FName& ActorName, const FVector& Location, const FVector& Scale)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Name = ActorName;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AStaticMeshActor* Block = GetWorld()->SpawnActor<AStaticMeshActor>(
		AStaticMeshActor::StaticClass(), Location, FRotator::ZeroRotator, SpawnParams);
	if (!Block)
	{
		return nullptr;
	}

	UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	Block->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	Block->GetStaticMeshComponent()->SetStaticMesh(Cube);
	Block->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
	Block->SetActorScale3D(Scale);
	return Block;
}

void UP02PlaytestSubsystem::SpawnTransientTestCourse()
{
	if (bCourseSpawned || !PlayerPawn.IsValid())
	{
		return;
	}
	bCourseSpawned = true;

	// These values define only an isolated runtime smoke-test course. They are not Portfolio layout values.
	const FVector TestOrigin(25000.0f, 25000.0f, 0.0f);
	SpawnTestBlock(TEXT("MCP_TEST_Floor"), TestOrigin + FVector(1100.0f, 0.0f, -50.0f), FVector(28.0f, 10.0f, 1.0f));
	SpawnTestBlock(TEXT("MCP_TEST_WallLeft"), TestOrigin + FVector(1100.0f, -550.0f, 150.0f), FVector(28.0f, 1.0f, 3.0f));
	SpawnTestBlock(TEXT("MCP_TEST_WallRight"), TestOrigin + FVector(1100.0f, 550.0f, 150.0f), FVector(28.0f, 1.0f, 3.0f));

	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.Name = TEXT("MCP_TEST_Start");
	GetWorld()->SpawnActor<AP02StartMarker>(
		AP02StartMarker::StaticClass(), TestOrigin + FVector(0.0f, 0.0f, 100.0f), FRotator::ZeroRotator, SpawnParams);

	SpawnParams.Name = TEXT("MCP_TEST_Door");
	AP02PlaytestDoor* Door = GetWorld()->SpawnActor<AP02PlaytestDoor>(
		AP02PlaytestDoor::StaticClass(), TestOrigin + FVector(1400.0f, 0.0f, 150.0f), FRotator::ZeroRotator, SpawnParams);

	SpawnParams.Name = TEXT("MCP_TEST_Enemy");
	AP02PlaytestEnemy* Enemy = GetWorld()->SpawnActor<AP02PlaytestEnemy>(
		AP02PlaytestEnemy::StaticClass(), TestOrigin + FVector(900.0f, 0.0f, 80.0f), FRotator(0.0f, 180.0f, 0.0f), SpawnParams);

	SpawnParams.Name = TEXT("MCP_TEST_Encounter");
	AP02EncounterTrigger* Encounter = GetWorld()->SpawnActor<AP02EncounterTrigger>(
		AP02EncounterTrigger::StaticClass(), TestOrigin + FVector(420.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
	if (Encounter)
	{
		TArray<AP02PlaytestEnemy*> Enemies;
		Enemies.Add(Enemy);
		Encounter->ConfigureRuntimeTest(Enemies, Door);
		ActiveEncounter = Encounter;
	}

	SpawnParams.Name = TEXT("MCP_TEST_End");
	AP02EndTrigger* EndTrigger = GetWorld()->SpawnActor<AP02EndTrigger>(
		AP02EndTrigger::StaticClass(), TestOrigin + FVector(1950.0f, 0.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
	if (EndTrigger)
	{
		EndTrigger->SetRequiredEncounter(Encounter);
	}

	StartLocation = TestOrigin + FVector(0.0f, 0.0f, 100.0f);
	PlayerPawn->SetActorLocation(StartLocation, false, nullptr, ETeleportType::TeleportPhysics);
	PlayerPawn->SetActorRotation(FRotator::ZeroRotator);
	if (ACharacter* Character = Cast<ACharacter>(PlayerPawn.Get()))
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
	}
	LastPlayerLocation = StartLocation;
	TravelDistance = 0.0f;
	RunStartTime = GetWorld()->GetTimeSeconds();
	PlayerHealth = P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread();

	UE_LOG(LogP02Playtest, Display, TEXT("Transient MCP_TEST course spawned; no map assets were modified"));
}
