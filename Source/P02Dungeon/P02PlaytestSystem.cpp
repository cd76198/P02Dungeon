// Copyright Epic Games, Inc. All Rights Reserved.

#include "P02PlaytestSystem.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/TriggerBox.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
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
#include "Blueprint/UserWidget.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "Fonts/CompositeFont.h"
#include "Components/WidgetComponent.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogP02Playtest, Log, All);

// ponytail: no actor tags anywhere. A wall opts in by carrying a material that declares the
// WallFadeOut parameter, and a region opts in by being a TriggerBox named P02_WallFade*.
// Both are settable over MCP, which cannot write actor tags at all.
static const FName P02WallFadeParam(TEXT("WallFadeOut"));
static const FName P02SurfaceColorParam(TEXT("SurfaceColor"));

/** Stamps a run milestone from any of the encounter actors. */
static void P02Milestone(UWorld* World, const FString& Label)
{
	if (UP02PlaytestSubsystem* Playtest = World ? World->GetSubsystem<UP02PlaytestSubsystem>() : nullptr)
	{
		Playtest->RecordMilestone(Label);
	}
}
/** Damage tint for the weak span. The span keeps the bridge material; only this colour moves. */
static const FLinearColor P02BridgeDamageColor(0.50f, 0.13f, 0.05f);
static const TCHAR* P02WallFadeVolumePrefix = TEXT("P02_WallFade");
static const TCHAR* P02FadeTargetVolumePrefix = TEXT("P02_FadeTarget");
// Same-ground walls need the height test skipped: a rock standing on the floor you walk on
// never has its underside overhead, but it still blocks the camera.
static const TCHAR* P02FadeAlwaysVolumePrefix = TEXT("P02_FadeAlways");
// Never fades, whatever box it sits in. Candidate 2 was dropped 600uu so the hint blocks fall
// outside the frustum from every walkable point down there - the fade is redundant, and it was
// firing the moment the player stepped onto the stairs, yanking the hint away mid-descent.
static const TCHAR* P02NeverFadePrefix = TEXT("P02_ColorHint");

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
	static TAutoConsoleVariable<float> CameraDistance(TEXT("p02.Camera.Distance"), 2000.0f, TEXT("Quarter-view camera distance."));
	static TAutoConsoleVariable<float> CameraFOV(TEXT("p02.Camera.FOV"), 45.0f, TEXT("Quarter-view perspective FOV."));

	static TAutoConsoleVariable<float> DashDistance(TEXT("p02.Dash.Distance"), 450.0f, TEXT("Dash travel distance in uu."));
	static TAutoConsoleVariable<float> DashSpeed(TEXT("p02.Dash.Speed"), 1600.0f, TEXT("Dash speed in uu/s."));
	static TAutoConsoleVariable<float> DashCooldown(TEXT("p02.Dash.Cooldown"), 0.8f, TEXT("Dash cooldown in seconds."));

	static TAutoConsoleVariable<float> AttackDamage(TEXT("p02.Attack.Damage"), 34.0f, TEXT("Transient test attack damage."));
	static TAutoConsoleVariable<float> AttackRange(TEXT("p02.Attack.Range"), 140.0f, TEXT("Transient test attack forward offset."));
	static TAutoConsoleVariable<float> AttackRadius(TEXT("p02.Attack.Radius"), 170.0f, TEXT("Transient test attack radius."));
	static TAutoConsoleVariable<float> AttackCooldown(TEXT("p02.Attack.Cooldown"), 0.35f, TEXT("Transient test attack cooldown."));
	static TAutoConsoleVariable<float> PlayerMaxHealth(TEXT("p02.Player.MaxHealth"), 100.0f, TEXT("Transient test player health."));

	static TAutoConsoleVariable<int32> WallFadeEnabled(
		TEXT("p02.WallFade.Enabled"), 1,
		TEXT("Fade the fade-material walls while the player is inside a P02_WallFade trigger box."));
	static TAutoConsoleVariable<float> WallFadeAmount(
		TEXT("p02.WallFade.Amount"), 1.0f,
		TEXT("How far an occluding wall fades, 0..1. 1 = fully invisible."));
	static TAutoConsoleVariable<float> WallFadeHeadRoom(
		TEXT("p02.WallFade.HeadRoom"), 100.0f,
		TEXT("A target only fades while its underside sits this far above the pawn origin."));
	static TAutoConsoleVariable<float> WallFadeSpeed(
		TEXT("p02.WallFade.Speed"), 4.0f,
		TEXT("Fade rate per second. 4 = ~0.25s for a full fade."));

	static TAutoConsoleVariable<int32> BridgeHitCount(
		TEXT("p02.Bridge.HitCount"), 8,
		TEXT("Player hits needed to drop the weak bridge span. TEST VALUE - 8 decided by the level designer."));
	static TAutoConsoleVariable<float> WrongAnswerMaxSeconds(
		TEXT("p02.WrongAnswer.MaxSeconds"), 60.0f,
		TEXT("Force-clears the wrong-answer fight so an unreachable enemy cannot lock the run."));
	static TAutoConsoleVariable<int32> PromptEnabled(
		TEXT("p02.Prompt"), 1,
		TEXT("Interaction prompts (big E and the door countdown). Separate from p02.HUD so captures keep them."));
	static TAutoConsoleVariable<int32> PromptTest(
		TEXT("p02.Prompt.Test"), 0,
		TEXT("1 draws a prompt over the player at all times, to separate 'drawing broken' from 'never in range'."));
	static TAutoConsoleVariable<float> PromptScale(
		TEXT("p02.Prompt.Scale"), 2.0f,
		TEXT("Interaction prompt size multiplier over a 28pt base. Tunable without a build."));
	static TAutoConsoleVariable<int32> HudEnabled(
		TEXT("p02.HUD"), 1,
		TEXT("0 hides the playtest HUD text and every UMG widget, life bars included. For captures."));
	static TAutoConsoleVariable<int32> RunReport(
		TEXT("p02.Run.Report"), 0,
		TEXT("Set to 1 to dump the milestone table mid-run. Clears itself."));
	static TAutoConsoleVariable<float> PlayerKillZ(
		TEXT("p02.Player.KillZ"), -1750.0f,
		TEXT("Below this Z the player has fallen out of the level. Lowest floor bottom is -1660 (Candidate 2)."));
	static TAutoConsoleVariable<float> BridgeDropDistance(
		TEXT("p02.Bridge.DropDistance"), 1400.0f,
		TEXT("How far the weak span falls once broken."));
	static TAutoConsoleVariable<float> BridgeDropDuration(
		TEXT("p02.Bridge.DropDuration"), 0.6f,
		TEXT("Seconds the weak span takes to fall."));

	static TAutoConsoleVariable<float> OccluderTestPlayerY(
		TEXT("p02.OccluderTest.PlayerY"), -99999.0f,
		TEXT("One-shot PIE test teleport to the requested world Y; -99999 disables it."));
}

/** Screen text is off while p02.HUD is 0, so captures do not need DisableAllScreenMessages. */
static bool P02HudVisible()
{
	return P02PlaytestCVars::HudEnabled.GetValueOnGameThread() != 0;
}

/**
 * Interaction prompt drawn above the player's head. Placing it on the object read badly under
 * the quarter view - the door is far away and often behind geometry, while the player is always
 * centred. DrawDebugString takes a font scale; on-screen debug messages do not, and they always
 * stack top-left.
 */
static void P02DrawPrompt(UWorld* World, const FVector& Location, const FString& Text,
	const FColor& Color, float RelativeScale)
{
	if (!World || P02PlaytestCVars::PromptEnabled.GetValueOnGameThread() == 0)
	{
		return;
	}
	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	AP02PlaytestHUD* HUD = PC ? Cast<AP02PlaytestHUD>(PC->GetHUD()) : nullptr;
	if (!HUD)
	{
		return;
	}
	HUD->AddPrompt(Location, Text, Color,
		RelativeScale * P02PlaytestCVars::PromptScale.GetValueOnGameThread());
}

void AP02PlaytestHUD::AddPrompt(const FVector& WorldLocation, const FString& Text,
	const FColor& Color, float Scale)
{
	UE_LOG(LogP02Playtest, VeryVerbose, TEXT("Prompt queued: %s"), *Text);
	FP02Prompt& Entry = Prompts.AddDefaulted_GetRef();
	Entry.Location = WorldLocation;
	Entry.Text = Text;
	Entry.Color = Color;
	Entry.Scale = Scale;
}

void AP02PlaytestHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!bFontLookupDone)
	{
		bFontLookupDone = true;
		// UE5 imports a .ttf as a UFontFace. FSlateFontInfo needs a UFont - it reads the composite
		// font off it - and a Face has none, so handing the Face over draws nothing at all, with no
		// warning. Wrap the Face in a transient runtime UFont instead of asking for a second asset.
		UObject* Face = LoadObject<UObject>(nullptr, TEXT("/Game/Portfolio02/UI/ONE_Mobile_POP.ONE_Mobile_POP"));
		if (UFont* AlreadyFont = Cast<UFont>(Face))
		{
			PromptFont = AlreadyFont;
		}
		else if (Face)
		{
			UFont* Wrapper = NewObject<UFont>(this);
			Wrapper->FontCacheType = EFontCacheType::Runtime;
			FTypefaceEntry& Entry = Wrapper->CompositeFont.DefaultTypeface.Fonts.AddDefaulted_GetRef();
			Entry.Name = TEXT("Regular");
			Entry.Font = FFontData(Face);
			PromptFont = Wrapper;
		}
		UE_LOG(LogP02Playtest, Display, TEXT("Prompt font: %s (source %s)"),
			PromptFont ? TEXT("ready") : TEXT("MISSING - falling back to engine font"),
			Face ? *Face->GetClass()->GetName() : TEXT("none"));
	}

	if (!Canvas)
	{
		Prompts.Reset();
		return;
	}

	for (const FP02Prompt& Entry : Prompts)
	{
		const FVector Screen = Project(Entry.Location);
		if (Screen.Z <= 0.0f)
		{
			continue;
		}

		const FVector2D At(Screen.X, Screen.Y);
		const int32 Size = FMath::Max(1, FMath::RoundToInt(28.0f * Entry.Scale));

		if (PromptFont)
		{
			FCanvasTextItem Item(At, FText::FromString(Entry.Text),
				FSlateFontInfo(PromptFont.Get(), Size), FLinearColor(Entry.Color));
			Item.bCentreX = true;
			Item.bCentreY = true;
			Item.EnableShadow(FLinearColor::Black);
			Canvas->DrawItem(Item);
		}
		else if (GEngine && GEngine->GetLargeFont())
		{
			FCanvasTextItem Item(At, FText::FromString(Entry.Text),
				GEngine->GetLargeFont(), FLinearColor(Entry.Color));
			Item.Scale = FVector2D(Entry.Scale, Entry.Scale);
			Item.bCentreX = true;
			Item.bCentreY = true;
			Item.EnableShadow(FLinearColor::Black);
			Canvas->DrawItem(Item);
		}
	}
	Prompts.Reset();
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

	UpdateWallFade(DeltaSeconds);
}

/**
 * Fades meshes inside a P02_FadeTarget* box while the player stands inside a P02_WallFade*
 * box. Two box sets instead of tags: MCP cannot write actor tags, and keying off the material
 * broke once every actor shared one.
 *
 * Each target is judged on its own: inside a P02_FadeTarget* box it fades only while its
 * underside is above the pawn, which is exactly when it can sit between a top-down camera and
 * the player. That gives the layering for free - a staircase stays solid while you walk down it
 * and fades once you are below it. Inside a P02_FadeAlways* box the height test is skipped, for
 * walls that stand on the same floor as the player and so are never overhead.
 * Render-only, collision is untouched.
 */
void AP02QuarterViewCameraRig::UpdateWallFade(float DeltaSeconds)
{
	UWorld* World = GetWorld();
	if (!World || !TrackedPawn.IsValid())
	{
		return;
	}

	if (!bWallFadeTargetsCached)
	{
		bWallFadeTargetsCached = true;
		// Boxes mark which geometry fades, so every actor can share one material.
		TArray<FBox> TargetVolumes;
		TArray<FBox> AlwaysVolumes;
		for (TActorIterator<ATriggerBox> VolumeIt(World); VolumeIt; ++VolumeIt)
		{
			const FString VolumeName = VolumeIt->GetName();
			if (VolumeName.Contains(P02FadeAlwaysVolumePrefix))
			{
				AlwaysVolumes.Add(VolumeIt->GetComponentsBoundingBox(true));
			}
			else if (VolumeName.Contains(P02FadeTargetVolumePrefix))
			{
				TargetVolumes.Add(VolumeIt->GetComponentsBoundingBox(true));
			}
		}

		for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
		{
			if (ActorIt->GetName().StartsWith(P02NeverFadePrefix))
			{
				continue;
			}
			ActorIt->ForEachComponent<UMeshComponent>(false,
				[this, &TargetVolumes, &AlwaysVolumes](UMeshComponent* Mesh)
			{
				const FBox MeshBox = Mesh->Bounds.GetBox();
				for (const FBox& Volume : AlwaysVolumes)
				{
					if (Volume.Intersect(MeshBox))
					{
						WallFadeTargets.Add(Mesh);
						WallFadeAlphas.Add(0.0f);
						WallFadeIgnoresHeight.Add(1);
						return;
					}
				}
				for (const FBox& Volume : TargetVolumes)
				{
					if (Volume.Intersect(MeshBox))
					{
						WallFadeTargets.Add(Mesh);
						WallFadeAlphas.Add(0.0f);
						WallFadeIgnoresHeight.Add(0);
						return;
					}
				}
			});
		}
		UE_LOG(LogP02Playtest, Display, TEXT("Wall fade: %d fadeable walls found (%s excluded)"),
			WallFadeTargets.Num(), P02NeverFadePrefix);
	}

	bool bPlayerInsideVolume = false;
	if (P02PlaytestCVars::WallFadeEnabled.GetValueOnGameThread() != 0)
	{
		const FVector PlayerLocation = TrackedPawn->GetActorLocation();
		for (TActorIterator<ATriggerBox> VolumeIt(World); VolumeIt; ++VolumeIt)
		{
			if (VolumeIt->GetName().Contains(P02WallFadeVolumePrefix) &&
				VolumeIt->GetComponentsBoundingBox(true).IsInsideOrOn(PlayerLocation))
			{
				bPlayerInsideVolume = true;
				break;
			}
		}
	}

	const float MaxFade = FMath::Clamp(P02PlaytestCVars::WallFadeAmount.GetValueOnGameThread(), 0.0f, 1.0f);
	const float Speed = FMath::Max(0.1f, P02PlaytestCVars::WallFadeSpeed.GetValueOnGameThread());
	const float HeadRoom = P02PlaytestCVars::WallFadeHeadRoom.GetValueOnGameThread();
	const float PawnZ = TrackedPawn->GetActorLocation().Z;

	for (int32 Index = 0; Index < WallFadeTargets.Num(); ++Index)
	{
		UMeshComponent* Mesh = WallFadeTargets[Index].Get();
		if (!Mesh)
		{
			continue;
		}

		const bool bOverhead = WallFadeIgnoresHeight[Index] != 0 ||
			Mesh->Bounds.GetBox().Min.Z > PawnZ + HeadRoom;
		const float Target = (bPlayerInsideVolume && bOverhead) ? MaxFade : 0.0f;

		float& Alpha = WallFadeAlphas[Index];
		const float PreviousAlpha = Alpha;
		Alpha = FMath::FInterpConstantTo(Alpha, Target, DeltaSeconds, Speed);
		if (Alpha == PreviousAlpha)
		{
			continue;
		}

		if (UMaterialInstanceDynamic* FadeMaterial = Mesh->CreateDynamicMaterialInstance(0))
		{
			FadeMaterial->SetScalarParameterValue(P02WallFadeParam, Alpha);
		}
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

AP02OccluderTestTrigger::AP02OccluderTestTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OcclusionTestTrigger"));
	RootComponent = TriggerBox;
	TriggerBox->SetBoxExtent(FVector(300.0f, 300.0f, 200.0f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AP02OccluderTestTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AP02OccluderTestTrigger::HandleBeginOverlap);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AP02OccluderTestTrigger::HandleEndOverlap);

	TArray<AActor*> TaggedActors;
	UGameplayStatics::GetAllActorsWithTag(this, UpperSectionTag, TaggedActors);
	for (AActor* Actor : TaggedActors)
	{
		UpperSections.Add(Actor);
	}
	SetUpperSectionsHidden(false);
}

void AP02OccluderTestTrigger::HandleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APawn>(OtherActor))
	{
		SetUpperSectionsHidden(true);
	}
}

void AP02OccluderTestTrigger::HandleEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
{
	if (Cast<APawn>(OtherActor))
	{
		SetUpperSectionsHidden(false);
	}
}

void AP02OccluderTestTrigger::SetUpperSectionsHidden(bool bShouldHide)
{
	int32 ChangedCount = 0;
	for (const TWeakObjectPtr<AActor>& ActorPtr : UpperSections)
	{
		if (AActor* Actor = ActorPtr.Get())
		{
			Actor->SetActorHiddenInGame(bShouldHide);
			++ChangedCount;
		}
	}
	UE_LOG(LogP02Playtest, Display, TEXT("Occluder test: upper hidden=%s, sections=%d"),
		bShouldHide ? TEXT("true") : TEXT("false"), ChangedCount);
}

AP02TransformStairInteraction::AP02TransformStairInteraction()
{
	PrimaryActorTick.bCanEverTick = true;

	MechanismMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MechanismMesh"));
	RootComponent = MechanismMesh;
	MechanismMesh->SetCollisionProfileName(TEXT("NoCollision"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> GridMaterial(
		TEXT("/Game/Portfolio02/Graybox/MI_Grid_1Second_400uu_TEST.MI_Grid_1Second_400uu_TEST"));
	if (CylinderMesh.Succeeded())
	{
		MechanismMesh->SetStaticMesh(CylinderMesh.Object);
	}
	MechanismMesh->SetRelativeScale3D(FVector(0.6f, 0.6f, 0.4f));

	LeverHandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverHandleMesh"));
	LeverHandleMesh->SetupAttachment(MechanismMesh);
	LeverHandleMesh->SetCollisionProfileName(TEXT("NoCollision"));
	if (CubeMesh.Succeeded())
	{
		LeverHandleMesh->SetStaticMesh(CubeMesh.Object);
	}
	LeverHandleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 85.0f));
	LeverHandleMesh->SetRelativeRotation(FRotator(-25.0f, 0.0f, 0.0f));
	LeverHandleMesh->SetRelativeScale3D(FVector(0.18f, 0.18f, 1.2f));

	if (GridMaterial.Succeeded())
	{
		MechanismMesh->SetMaterial(0, GridMaterial.Object);
		LeverHandleMesh->SetMaterial(0, GridMaterial.Object);
	}
}

void AP02TransformStairInteraction::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AP02PlaytestEnemy> It(GetWorld()); It; ++It)
	{
		AP02PlaytestEnemy* Enemy = *It;
		if (IsValid(Enemy))
		{
			TrackedEnemies.Add(Enemy);
			Enemy->OnDestroyed.RemoveDynamic(this, &AP02TransformStairInteraction::HandleEnemyDestroyed);
			Enemy->OnDestroyed.AddDynamic(this, &AP02TransformStairInteraction::HandleEnemyDestroyed);
		}
	}

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || Actor == this)
		{
			continue;
		}

		if (Actor->GetName().StartsWith(TEXT("MCP_TEST_TransformStair_Step")))
		{
			// These graybox pieces are animated after the lever is used. Level-placed
			// StaticMeshActors default to Static mobility, which rejects runtime movement.
			if (USceneComponent* Root = Actor->GetRootComponent())
			{
				Root->SetMobility(EComponentMobility::Movable);
			}
			MovableStairs.Add(Actor);
			StairStartLocations.Add(Actor->GetActorLocation());
		}
		else if (Actor->GetName().StartsWith(TEXT("MCP_TEST_TransformStair_Blocker")))
		{
			Blockers.Add(Actor);
		}
		else if (AP02BridgeBreaker* Breaker = Cast<AP02BridgeBreaker>(Actor))
		{
			Bridge = Breaker;
		}
	}

	MechanismMesh->SetVisibility(false, true);
	if (TrackedEnemies.IsEmpty())
	{
		SetInteractionAvailable();
	}

	UE_LOG(LogP02Playtest, Display,
		TEXT("Candidate mechanism ready: enemies=%d stairs=%d blockers=%d"),
		TrackedEnemies.Num(), MovableStairs.Num(), Blockers.Num());
}

void AP02TransformStairInteraction::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player)
	{
		return;
	}

	const float PlayerDistance = FVector::Dist2D(Player->GetActorLocation(), GetActorLocation());
	if (!bEncounterActivated && PlayerDistance <= EnemyActivationRadius)
	{
		bEncounterActivated = true;
		for (const TWeakObjectPtr<AP02PlaytestEnemy>& EnemyPtr : TrackedEnemies)
		{
			if (AP02PlaytestEnemy* Enemy = EnemyPtr.Get())
			{
				Enemy->SetEncounterActive(true);
			}
		}
		UE_LOG(LogP02Playtest, Display, TEXT("Candidate Mid Boss activated"));
	}

	if (AutoStartCountdown >= 0.0f && !bActivated)
	{
		AutoStartCountdown -= DeltaSeconds;
		if (AutoStartCountdown <= 0.0f)
		{
			AutoStartCountdown = -1.0f;
			StartMechanism();
		}
	}

	// The stairs lower on their own, so the lever's real job is the bridge: it stays usable
	// after the descent and is the only way back across once the span is down.
	const bool bBridgeDown = Bridge.IsValid() && Bridge->IsBroken();
	if (bInteractionAvailable && PlayerDistance <= InteractionRadius)
	{
		P02DrawPrompt(GetWorld(), Player->GetActorLocation() + FVector(0.0f, 0.0f, 200.0f),
			TEXT("E"), FColor::White, 1.0f);
		if (P02HudVisible() && GEngine && bBridgeDown)
		{
			GEngine->AddOnScreenDebugMessage(2200206, 0.1f, FColor::Green,
				TEXT("LEVER  press E to rebuild the bridge"));
		}
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			if (PC->WasInputKeyJustPressed(EKeys::E))
			{
				// The lever does one thing. The stairs lower on their own after the Mid Boss dies,
				// so calling StartMechanism here was a dead second path to the same descent.
				if (AP02BridgeBreaker* Breaker = Bridge.Get())
				{
					Breaker->Restore();
				}
			}
		}
	}

	if (!bActivated || MoveElapsed >= MoveDuration)
	{
		return;
	}

	MoveElapsed = FMath::Min(MoveDuration, MoveElapsed + DeltaSeconds);
	const float Alpha = MoveDuration > KINDA_SMALL_NUMBER ? MoveElapsed / MoveDuration : 1.0f;
	for (int32 Index = 0; Index < MovableStairs.Num() && Index < StairStartLocations.Num(); ++Index)
	{
		if (AActor* Stair = MovableStairs[Index].Get())
		{
			Stair->SetActorLocation(StairStartLocations[Index] + FVector(0.0f, 0.0f, -LowerDistance * Alpha));
		}
	}

	for (const TWeakObjectPtr<AActor>& BlockerPtr : Blockers)
	{
		if (AActor* Blocker = BlockerPtr.Get())
		{
			Blocker->ForEachComponent<UMeshComponent>(false, [Alpha](UMeshComponent* Mesh)
			{
				if (UMaterialInstanceDynamic* PlateFade = Mesh->CreateDynamicMaterialInstance(0))
				{
					PlateFade->SetScalarParameterValue(P02WallFadeParam, Alpha);
				}
			});
		}
	}

	if (MoveElapsed >= MoveDuration)
	{
		for (const TWeakObjectPtr<AActor>& BlockerPtr : Blockers)
		{
			if (AActor* Blocker = BlockerPtr.Get())
			{
				Blocker->SetActorHiddenInGame(true);
			}
		}
		UE_LOG(LogP02Playtest, Display, TEXT("Candidate plate faded out over %.2fs"), MoveDuration);
	}
}

void AP02TransformStairInteraction::HandleEnemyDestroyed(AActor* DestroyedActor)
{
	for (int32 Index = TrackedEnemies.Num() - 1; Index >= 0; --Index)
	{
		if (!TrackedEnemies[Index].IsValid() || TrackedEnemies[Index].Get() == DestroyedActor)
		{
			TrackedEnemies.RemoveAt(Index);
		}
	}

	if (TrackedEnemies.IsEmpty())
	{
		SetInteractionAvailable();
	}
}

void AP02TransformStairInteraction::SetInteractionAvailable()
{
	if (bInteractionAvailable)
	{
		return;
	}

	bInteractionAvailable = true;
	MechanismMesh->SetVisibility(true, true);
	P02Milestone(GetWorld(), TEXT("MID BOSS DOWN (lever appears)"));
	// The lever still appears on death, but it no longer gates the descent.
	AutoStartCountdown = FMath::Max(0.0f, AutoStartDelay);
	UE_LOG(LogP02Playtest, Display,
		TEXT("Candidate interaction enabled after Mid Boss death, stairs lower in %.2fs"), AutoStartCountdown);
}

void AP02TransformStairInteraction::StartMechanism()
{
	if (bActivated)
	{
		return;
	}

	bActivated = true;
	MoveElapsed = 0.0f;
	LeverHandleMesh->SetRelativeRotation(FRotator(35.0f, 0.0f, 0.0f));
	P02Milestone(GetWorld(), TEXT("STAIRS DOWN"));
	// The plate fades out over MoveDuration instead of popping, so the staircase behind it
	// is revealed rather than swapped in. Collision goes immediately.
	for (const TWeakObjectPtr<AActor>& BlockerPtr : Blockers)
	{
		if (AActor* Blocker = BlockerPtr.Get())
		{
			Blocker->SetActorEnableCollision(false);
		}
	}
	UE_LOG(LogP02Playtest, Display, TEXT("Candidate interaction used: lowering stairs"));
}

AP02ColorLockPuzzle::AP02ColorLockPuzzle()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);
	// Answer order decided by the level designer: Blue -> Red -> Green.
	Answer = { TEXT("Blue"), TEXT("Red"), TEXT("Green") };
}

void AP02ColorLockPuzzle::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			continue;
		}

		const FString Name = Actor->GetName();
		if (Name.Equals(TEXT("P02_ColorPad_Red")))
		{
			Pads.Add(TEXT("Red"), Actor);
		}
		else if (Name.Equals(TEXT("P02_ColorPad_Green")))
		{
			Pads.Add(TEXT("Green"), Actor);
		}
		else if (Name.Equals(TEXT("P02_ColorPad_Blue")))
		{
			Pads.Add(TEXT("Blue"), Actor);
		}
		else if (Name.StartsWith(TEXT("MCP_TEST_Candidate1_BigDoor")))
		{
			DoorParts.Add(Actor);
			DoorPartStarts.Add(Actor->GetActorLocation());
			// Level-placed StaticMeshActors are Static, which rejects runtime movement.
			if (USceneComponent* Root = Actor->GetRootComponent())
			{
				Root->SetMobility(EComponentMobility::Movable);
			}
		}
		else if (Name.Equals(TEXT("P02_EndTrigger")))
		{
			EndVolume = Actor;
		}
		else if (Name.StartsWith(TEXT("P02_WrongAnswerSpawn")))
		{
			// Plain level actors, so the level designer moves the spawn ring by dragging boxes.
			PunisherSpawns.Add(Actor->GetComponentsBoundingBox(true).GetCenter());
		}
	}

	UE_LOG(LogP02Playtest, Display,
		TEXT("Color lock ready: pads=%d door parts=%d end=%s spawns=%d answer=Blue,Red,Green"),
		Pads.Num(), DoorParts.Num(),
		EndVolume.IsValid() ? TEXT("yes") : TEXT("MISSING"), PunisherSpawns.Num());
}

void AP02ColorLockPuzzle::SpawnPunishers()
{
	if (bPunishActive || PunisherSpawns.IsEmpty())
	{
		return;
	}

	Punishers.Reset();
	for (const FVector& Spawn : PunisherSpawns)
	{
		// Deferred so MaxHealth lands before BeginPlay copies it into CurrentHealth.
		AP02PlaytestEnemy* Enemy = GetWorld()->SpawnActorDeferred<AP02PlaytestEnemy>(
			AP02PlaytestEnemy::StaticClass(), FTransform(FRotator::ZeroRotator, Spawn),
			nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!Enemy)
		{
			continue;
		}
		Enemy->MaxHealth = PunisherHealth;
		Enemy->MoveSpeed = PunisherMoveSpeed;
		Enemy->AttackDamage = PunisherAttackDamage;
		Enemy->FinishSpawning(FTransform(FRotator::ZeroRotator, Spawn));
		Enemy->SetEncounterActive(true);
		Punishers.Add(Enemy);
	}

	P02Milestone(GetWorld(), FString::Printf(TEXT("WRONG ANSWER #%d"), WrongAnswerCount));
	bPunishActive = true;
	bFirstContactMeasured = false;
	LastPlayerHealth = -1.0f;
	WrongAnswerTime = GetWorld()->GetTimeSeconds();

	if (const APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		float Nearest = TNumericLimits<float>::Max();
		float Farthest = 0.0f;
		for (const FVector& Spawn : PunisherSpawns)
		{
			const float Distance = FVector::Dist2D(Spawn, Player->GetActorLocation());
			Nearest = FMath::Min(Nearest, Distance);
			Farthest = FMath::Max(Farthest, Distance);
		}
		UE_LOG(LogP02Playtest, Display,
			TEXT("Wrong answer #%d: %d enemies at speed %.0f, %.0f..%.0fuu away (%.1f..%.1fs)"),
			WrongAnswerCount, Punishers.Num(), PunisherMoveSpeed, Nearest, Farthest,
			Nearest / FMath::Max(1.0f, PunisherMoveSpeed), Farthest / FMath::Max(1.0f, PunisherMoveSpeed));
	}
}

void AP02ColorLockPuzzle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Player)
	{
		return;
	}
	const FVector PlayerLocation = Player->GetActorLocation();

	UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>();

	// Distance is the failure cost: the enemies start at the far side of the room and the pads
	// stay dead until they are down, so guessing is always slower than reading the hint.
	if (bPunishActive)
	{
		const float Now = GetWorld()->GetTimeSeconds();
		const float Health = Playtest ? Playtest->GetPlayerHealth() : -1.0f;
		if (!bFirstContactMeasured && LastPlayerHealth >= 0.0f && Health < LastPlayerHealth)
		{
			bFirstContactMeasured = true;
			UE_LOG(LogP02Playtest, Display,
				TEXT("Wrong answer MEASURED: first contact %.2fs after the wrong input"),
				Now - WrongAnswerTime);
		}
		LastPlayerHealth = Health;

		int32 Alive = 0;
		for (const TWeakObjectPtr<AP02PlaytestEnemy>& EnemyPtr : Punishers)
		{
			if (const AP02PlaytestEnemy* Enemy = EnemyPtr.Get())
			{
				Alive += Enemy->GetHealth() > 0.0f ? 1 : 0;
			}
		}

		const float MaxSeconds = P02PlaytestCVars::WrongAnswerMaxSeconds.GetValueOnGameThread();
		const bool bTimedOut = MaxSeconds > 0.0f && Now - WrongAnswerTime > MaxSeconds;
		if (Alive == 0 || bTimedOut)
		{
			if (bTimedOut)
			{
				// An enemy that cannot reach the player would otherwise lock the pads for good.
				for (const TWeakObjectPtr<AP02PlaytestEnemy>& EnemyPtr : Punishers)
				{
					if (AP02PlaytestEnemy* Enemy = EnemyPtr.Get())
					{
						Enemy->Destroy();
					}
				}
			}
			bPunishActive = false;
			Punishers.Reset();
			P02Milestone(GetWorld(), FString::Printf(TEXT("RETRY AVAILABLE (after wrong #%d)"), WrongAnswerCount));
			UE_LOG(LogP02Playtest, Display,
				TEXT("Wrong answer MEASURED: retry available %.2fs after the wrong input%s"),
				Now - WrongAnswerTime, bTimedOut ? TEXT(" (TIMED OUT)") : TEXT(""));
		}
	}

	if (!bSolved && !bPunishActive)
	{
		// Prompt sits on whichever pad you are standing on, so the input target is unambiguous.
		for (const TPair<FName, TWeakObjectPtr<AActor>>& Pair : Pads)
		{
			const AActor* Pad = Pair.Value.Get();
			if (Pad && FVector::Dist(PlayerLocation, Pad->GetActorLocation()) <= PadRadius)
			{
				P02DrawPrompt(GetWorld(), PlayerLocation + FVector(0.0f, 0.0f, 200.0f),
					TEXT("E"), FColor::White, 1.0f);
				break;
			}
		}

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (PC && PC->WasInputKeyJustPressed(EKeys::E))
		{
			for (const TPair<FName, TWeakObjectPtr<AActor>>& Pair : Pads)
			{
				AActor* Pad = Pair.Value.Get();
				if (!Pad || FVector::Dist(PlayerLocation, Pad->GetActorLocation()) > PadRadius)
				{
					continue;
				}

				const int32 Step = Entered.Num();
				if (Answer.IsValidIndex(Step) && Answer[Step] == Pair.Key)
				{
					Entered.Add(Pair.Key);
					if (Entered.Num() == Answer.Num())
					{
						bSolved = true;
						OpenElapsed = 0.0f;
						for (const TWeakObjectPtr<AActor>& PartPtr : DoorParts)
						{
							if (AActor* Part = PartPtr.Get())
							{
								Part->SetActorEnableCollision(false);
							}
						}
						P02Milestone(GetWorld(), TEXT("FINAL BOSS DOOR OPEN"));
						UE_LOG(LogP02Playtest, Display, TEXT("Color lock: correct, opening Final Boss Door"));
					}
					else
					{
						UE_LOG(LogP02Playtest, Display, TEXT("Color lock: %s accepted (%d/%d)"),
							*Pair.Key.ToString(), Entered.Num(), Answer.Num());
					}
				}
				else
				{
					++WrongAnswerCount;
					UE_LOG(LogP02Playtest, Display, TEXT("Color lock: %s wrong at step %d, sequence reset (wrong #%d)"),
						*Pair.Key.ToString(), Step + 1, WrongAnswerCount);
					Entered.Reset();
					SpawnPunishers();
				}
				break;
			}
		}
	}

	if (OpenElapsed >= 0.0f && OpenElapsed < DoorOpenDuration)
	{
		OpenElapsed = FMath::Min(DoorOpenDuration, OpenElapsed + DeltaSeconds);
		const float Alpha = DoorOpenDuration > KINDA_SMALL_NUMBER ? OpenElapsed / DoorOpenDuration : 1.0f;
		for (int32 Index = 0; Index < DoorParts.Num() && Index < DoorPartStarts.Num(); ++Index)
		{
			AActor* Part = DoorParts[Index].Get();
			if (!Part)
			{
				continue;
			}
			Part->SetActorLocation(DoorPartStarts[Index] - FVector(0.0f, 0.0f, DoorOpenDistance * Alpha));

			// Same reason as the Mid Boss door: a door sunk into the floor is still in frame under
			// a top-down camera, so it fades on the way down and is hidden when it lands.
			Part->ForEachComponent<UMeshComponent>(false, [Alpha](UMeshComponent* Mesh)
			{
				if (UMaterialInstanceDynamic* DoorFade = Mesh->CreateDynamicMaterialInstance(0))
				{
					DoorFade->SetScalarParameterValue(P02WallFadeParam, Alpha);
				}
			});

			if (OpenElapsed >= DoorOpenDuration)
			{
				Part->SetActorHiddenInGame(true);
			}
		}
		if (OpenElapsed >= DoorOpenDuration)
		{
			UE_LOG(LogP02Playtest, Display,
				TEXT("Final Boss door (%d parts) faded out over %.2fs and hidden"),
				DoorParts.Num(), DoorOpenDuration);
		}
	}

	if (bSolved && !bReachedEnd)
	{
		if (const AActor* Volume = EndVolume.Get())
		{
			if (Volume->GetComponentsBoundingBox(true).IsInsideOrOn(PlayerLocation))
			{
				bReachedEnd = true;
				UE_LOG(LogP02Playtest, Display, TEXT("Color lock: END reached"));
				// Without this the run never closes: no END milestone and no run report.
				if (Playtest)
				{
					Playtest->CompleteRun();
				}
			}
		}
	}

	if (P02HudVisible() && GEngine)
	{
		FString State = bReachedEnd ? TEXT("END") : (bSolved ? TEXT("DOOR OPEN") :
			(bPunishActive ? TEXT("WRONG - CLEAR THE ROOM") : TEXT("LOCKED")));
		FString Progress;
		for (const FName& Color : Entered)
		{
			Progress += Color.ToString() + TEXT(" ");
		}
		GEngine->AddOnScreenDebugMessage(2200203, 0.1f, FColor::Yellow,
			FString::Printf(TEXT("COLOR LOCK [%s]  %d/%d  %s   (walk onto a pad, press E)"),
				*State, Entered.Num(), Answer.Num(), *Progress));
	}
}

AP02BridgeBreaker::AP02BridgeBreaker()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);
}

void AP02BridgeBreaker::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor) || !Actor->GetName().StartsWith(TEXT("P02_BridgeWeakSection")))
		{
			continue;
		}

		// Level-placed StaticMeshActors are Static, which rejects runtime movement.
		if (USceneComponent* Root = Actor->GetRootComponent())
		{
			Root->SetMobility(EComponentMobility::Movable);
		}
		Sections.Add(Actor);
		SectionStartLocations.Add(Actor->GetActorLocation());
		SectionBounds += Actor->GetComponentsBoundingBox(true);

		// The span wears the same material as the rest of the bridge, so the base colour is
		// read off the asset instead of hardcoded - recolouring the bridge keeps working.
		Actor->ForEachComponent<UMeshComponent>(false, [this](UMeshComponent* Mesh)
		{
			if (UMaterialInstanceDynamic* Tint = Mesh->CreateDynamicMaterialInstance(0))
			{
				FLinearColor BaseColor = FLinearColor::White;
				Tint->GetVectorParameterValue(P02SurfaceColorParam, BaseColor);
				SectionMeshes.Add(Mesh);
				SectionBaseColors.Add(BaseColor);
			}
		});
	}

	UE_LOG(LogP02Playtest, Display,
		TEXT("Bridge breaker ready: sections=%d span X %.0f..%.0f hits=%d"),
		Sections.Num(), SectionBounds.Min.X, SectionBounds.Max.X,
		P02PlaytestCVars::BridgeHitCount.GetValueOnGameThread());
}

bool AP02BridgeBreaker::ApplyHit(const FVector& AttackCenter, float AttackRadius, const FVector& PlayerLocation)
{
	if (bBroken || Sections.IsEmpty())
	{
		return false;
	}

	// Only from the far side. Dropping the span out from under the player would strand them.
	if (PlayerLocation.X < SectionBounds.Max.X)
	{
		return false;
	}
	if (SectionBounds.ComputeSquaredDistanceToPoint(AttackCenter) > FMath::Square(AttackRadius))
	{
		return false;
	}

	++HitsTaken;
	ShakeRemaining = 0.12f;
	ApplyDamageTint();
	const int32 Needed = FMath::Max(1, P02PlaytestCVars::BridgeHitCount.GetValueOnGameThread());
	UE_LOG(LogP02Playtest, Display, TEXT("Bridge hit %d/%d"), HitsTaken, Needed);

	if (HitsTaken >= Needed)
	{
		bBroken = true;
		DropElapsed = 0.0f;
		for (const TWeakObjectPtr<AActor>& SectionPtr : Sections)
		{
			if (AActor* Section = SectionPtr.Get())
			{
				Section->SetActorEnableCollision(false);
			}
		}
		// The shake resets sections to SectionStartLocations. Left pending, it fired the frame the
		// drop finished and teleported the whole span back up - visible, but with collision off.
		ShakeRemaining = 0.0f;
		P02Milestone(GetWorld(), FString::Printf(TEXT("BRIDGE BROKEN (%d hits)"), HitsTaken));
		UE_LOG(LogP02Playtest, Display, TEXT("Bridge broken after %d hits, approach cut"), HitsTaken);
	}
	return true;
}

void AP02BridgeBreaker::ApplyDamageTint()
{
	const int32 Needed = FMath::Max(1, P02PlaytestCVars::BridgeHitCount.GetValueOnGameThread());
	const float Damage = FMath::Clamp(static_cast<float>(HitsTaken) / static_cast<float>(Needed), 0.0f, 1.0f);
	for (int32 Index = 0; Index < SectionMeshes.Num() && Index < SectionBaseColors.Num(); ++Index)
	{
		UMeshComponent* Mesh = SectionMeshes[Index].Get();
		if (!Mesh)
		{
			continue;
		}
		if (UMaterialInstanceDynamic* Tint = Mesh->CreateDynamicMaterialInstance(0))
		{
			Tint->SetVectorParameterValue(P02SurfaceColorParam,
				FMath::Lerp(SectionBaseColors[Index], P02BridgeDamageColor, Damage));
		}
	}
}

void AP02BridgeBreaker::Restore()
{
	for (int32 Index = 0; Index < Sections.Num() && Index < SectionStartLocations.Num(); ++Index)
	{
		if (AActor* Section = Sections[Index].Get())
		{
			Section->SetActorLocation(SectionStartLocations[Index]);
			Section->SetActorEnableCollision(true);
		}
	}

	HitsTaken = 0;
	DropElapsed = -1.0f;
	ShakeRemaining = 0.0f;
	bBroken = false;
	ApplyDamageTint();
	P02Milestone(GetWorld(), TEXT("BRIDGE RESTORED (lever)"));
	UE_LOG(LogP02Playtest, Display, TEXT("Bridge restored by the Mid Boss lever, damage reset"));
}

void AP02BridgeBreaker::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float DropDuration = FMath::Max(0.05f, P02PlaytestCVars::BridgeDropDuration.GetValueOnGameThread());
	const float DropDistance = P02PlaytestCVars::BridgeDropDistance.GetValueOnGameThread();

	if (DropElapsed >= 0.0f && DropElapsed < DropDuration)
	{
		DropElapsed = FMath::Min(DropDuration, DropElapsed + DeltaSeconds);
		const float Alpha = DropElapsed / DropDuration;
		for (int32 Index = 0; Index < Sections.Num() && Index < SectionStartLocations.Num(); ++Index)
		{
			if (AActor* Section = Sections[Index].Get())
			{
				Section->SetActorLocation(SectionStartLocations[Index] - FVector(0.0f, 0.0f, DropDistance * Alpha));
			}
		}
		return;
	}

	// Once broken the sections stay where they dropped: this block writes SectionStartLocations,
	// so running it after the drop would put the span back in the air.
	if (bBroken)
	{
		return;
	}

	// Hit feedback: a short dip, so a hit reads without particles or sound.
	if (ShakeRemaining > 0.0f)
	{
		ShakeRemaining -= DeltaSeconds;
		const float Dip = ShakeRemaining > 0.0f ? 15.0f : 0.0f;
		for (int32 Index = 0; Index < Sections.Num() && Index < SectionStartLocations.Num(); ++Index)
		{
			if (AActor* Section = Sections[Index].Get())
			{
				Section->SetActorLocation(SectionStartLocations[Index] - FVector(0.0f, 0.0f, Dip));
			}
		}
	}

	if (P02HudVisible() && GEngine && !bBroken && !Sections.IsEmpty())
	{
		const int32 Needed = FMath::Max(1, P02PlaytestCVars::BridgeHitCount.GetValueOnGameThread());
		GEngine->AddOnScreenDebugMessage(2200205, 0.1f, FColor::Red,
			FString::Printf(TEXT("BRIDGE  %d/%d hits   (attack the weak span from the east side)"),
				HitsTaken, Needed));
	}
}

AP02MidBossDoorGate::AP02MidBossDoorGate()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);
}

void AP02MidBossDoorGate::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			continue;
		}

		const FString Name = Actor->GetName();
		if (Name.Equals(TEXT("P02_MidBossDoor")))
		{
			Door = Actor;
			DoorClosedLocation = Actor->GetActorLocation();
			// Approach side is -X: the player arrives from the bridge. Z comes off the door's own
			// bottom, which sits on the floor, so no height is guessed.
			const FBox DoorBox = Actor->GetComponentsBoundingBox(true);
			DoorApproachLocation = FVector(DoorBox.GetCenter().X - 200.0f, DoorBox.GetCenter().Y,
				DoorBox.Min.Z + 100.0f);
			if (USceneComponent* Root = Actor->GetRootComponent())
			{
				Root->SetMobility(EComponentMobility::Movable);
			}
		}
		else if (AP02BridgeBreaker* Breaker = Cast<AP02BridgeBreaker>(Actor))
		{
			Bridge = Breaker;
		}
		else if (Name.Equals(TEXT("P02_MidBossEnemySpawn")))
		{
			// The box does double duty: the player crosses its east face to arm the chase, and the
			// chaser appears at its west face. Widening the box in the editor is how the head start
			// gets tuned - spawning at the trigger line itself put the enemy on top of the player.
			SpawnVolume = Actor;
			const FBox Box = Actor->GetComponentsBoundingBox(true);
			SpawnLocation = FVector(Box.Min.X, Box.GetCenter().Y, Box.GetCenter().Z);
			ArmLineX = Box.Max.X;
		}
	}

	UE_LOG(LogP02Playtest, Display,
		TEXT("Mid Boss door gate ready: door=%s spawn=%s bridge=%s hold=%.1fs"),
		Door.IsValid() ? TEXT("yes") : TEXT("MISSING"),
		SpawnVolume.IsValid() ? TEXT("yes") : TEXT("MISSING"),
		Bridge.IsValid() ? TEXT("yes") : TEXT("MISSING"), HoldSeconds);
}

void AP02MidBossDoorGate::SpawnChaser()
{
	// Deferred so MaxHealth lands before BeginPlay copies it into CurrentHealth.
	AP02PlaytestEnemy* Enemy = GetWorld()->SpawnActorDeferred<AP02PlaytestEnemy>(
		AP02PlaytestEnemy::StaticClass(), FTransform(FRotator::ZeroRotator, SpawnLocation),
		nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Enemy)
	{
		return;
	}

	Enemy->MaxHealth = EnemyHealth;
	Enemy->MoveSpeed = EnemyMoveSpeed;
	Enemy->AttackDamage = EnemyAttackDamage;
	Enemy->FinishSpawning(FTransform(FRotator::ZeroRotator, SpawnLocation));
	Enemy->SetEncounterActive(true);
	Chaser = Enemy;
	ChaserSpawnTime = GetWorld()->GetTimeSeconds();
	bArrivalMeasured = false;
	++SpawnCount;
}

void AP02MidBossDoorGate::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	APawn* Player = UGameplayStatics::GetPlayerPawn(this, 0);
	UP02PlaytestSubsystem* Playtest = GetWorld()->GetSubsystem<UP02PlaytestSubsystem>();
	if (!Player || !Playtest)
	{
		return;
	}
	const FVector PlayerLocation = Player->GetActorLocation();
	const float Health = Playtest->GetPlayerHealth();
	const bool bTookDamage = LastPlayerHealth >= 0.0f && Health < LastPlayerHealth;
	LastPlayerHealth = Health;

	// Crossing the spawn point on the way to the door starts the chase.
	if (!bArmed && !bChaseRetired && SpawnVolume.IsValid() && PlayerLocation.X >= ArmLineX)
	{
		bArmed = true;
		SpawnChaser();
		UE_LOG(LogP02Playtest, Display,
			TEXT("Mid Boss door: chase armed at player X=%.0f, chaser spawns %.0fuu behind"),
			PlayerLocation.X, PlayerLocation.X - SpawnLocation.X);
	}

	// Latched, not polled: the lever puts the bridge back, but the chase stays gone.
	if (!bChaseRetired && Bridge.IsValid() && Bridge->IsBroken())
	{
		bChaseRetired = true;
		bArmed = false;
		RespawnCountdown = -1.0f;
		if (AP02PlaytestEnemy* Enemy = Chaser.Get())
		{
			Enemy->Destroy();
		}
		Playtest->SetRespawnLocation(DoorApproachLocation);
		UE_LOG(LogP02Playtest, Display,
			TEXT("Mid Boss door: bridge broken, chaser retired, respawn moved to the door at %s"),
			*DoorApproachLocation.ToCompactString());
	}

	if (bArmed && !bDoorOpen && !bChaseRetired)
	{
		if (!Chaser.IsValid() || Chaser->GetHealth() <= 0.0f)
		{
			if (RespawnCountdown < 0.0f)
			{
				RespawnCountdown = RespawnDelay;
			}
			else
			{
				RespawnCountdown -= DeltaSeconds;
				if (RespawnCountdown <= 0.0f)
				{
					RespawnCountdown = -1.0f;
					SpawnChaser();
				}
			}
		}
	}

	// Measured, not assumed: spawn point to first hit on the player.
	if (bTookDamage && !bArrivalMeasured && Chaser.IsValid())
	{
		bArrivalMeasured = true;
		const float Elapsed = GetWorld()->GetTimeSeconds() - ChaserSpawnTime;
		const float SpawnToPlayer = FVector::Dist2D(SpawnLocation, PlayerLocation);
		UE_LOG(LogP02Playtest, Display,
			TEXT("Mid Boss door MEASURED: spawn->first hit %.2fs over %.0fuu at speed %.0f (spawn #%d)"),
			Elapsed, SpawnToPlayer, EnemyMoveSpeed, SpawnCount);
	}

	if (!bDoorOpen && Door.IsValid())
	{
		const float DoorDistance = FVector::Dist2D(PlayerLocation, DoorClosedLocation);
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		const bool bHolding = PC && PC->IsInputKeyDown(EKeys::E) && DoorDistance <= InteractRadius;

		if (DoorDistance <= InteractRadius)
		{
			const FVector PromptAt = PlayerLocation + FVector(0.0f, 0.0f, 200.0f);
			if (bHolding)
			{
				// Counts down, not up: the reset on damage has to read as losing progress.
				// Colour tracks how close the hold is to finishing, so a reset is visible as a
				// colour jump backwards even in a still frame.
				const float Remaining = FMath::Max(0.0f, HoldSeconds - HoldElapsed);
				const FColor Tint = Remaining > 4.0f ? FColor::Orange
					: (Remaining > 1.0f ? FColor::Yellow : FColor::Green);
				P02DrawPrompt(GetWorld(), PromptAt,
					FString::Printf(TEXT("%.1f"), Remaining), Tint, 0.8f);
			}
			else
			{
				P02DrawPrompt(GetWorld(), PromptAt, TEXT("E"), FColor::White, 1.0f);
			}
		}

		if (bTookDamage && HoldElapsed > 0.0f)
		{
			UE_LOG(LogP02Playtest, Display,
				TEXT("Mid Boss door: hold cancelled by damage at %.2fs / %.2fs"), HoldElapsed, HoldSeconds);
			HoldElapsed = 0.0f;
		}
		else if (bHolding)
		{
			HoldElapsed += DeltaSeconds;
			if (HoldElapsed >= HoldSeconds)
			{
				bDoorOpen = true;
				OpenElapsed = 0.0f;
				if (AActor* OpeningDoor = Door.Get())
				{
					OpeningDoor->SetActorEnableCollision(false);
				}
				if (AP02PlaytestEnemy* Enemy = Chaser.Get())
				{
					Enemy->Destroy();
				}
				P02Milestone(GetWorld(), FString::Printf(TEXT("MID BOSS DOOR OPEN (%d chasers)"), SpawnCount));
				UE_LOG(LogP02Playtest, Display,
					TEXT("Mid Boss door opened after %.1fs hold, %d chasers spawned"), HoldSeconds, SpawnCount);
			}
		}
		else
		{
			HoldElapsed = 0.0f;
		}
	}

	if (OpenElapsed >= 0.0f && OpenElapsed < DoorOpenDuration)
	{
		OpenElapsed = FMath::Min(DoorOpenDuration, OpenElapsed + DeltaSeconds);
		const float Alpha = DoorOpenDuration > KINDA_SMALL_NUMBER ? OpenElapsed / DoorOpenDuration : 1.0f;
		if (AActor* OpeningDoor = Door.Get())
		{
			OpeningDoor->SetActorLocation(DoorClosedLocation - FVector(0.0f, 0.0f, DoorOpenDistance * Alpha));

			// Sinking alone hides nothing: the quarter view looks down at -50 degrees, so a door
			// dropped into the floor stays in frame. It fades as it drops - the same treatment the
			// stair plate gets - and is hidden outright once the drop finishes. Until now the only
			// thing that cleared it was the wall fade, which needs the player below the door and so
			// did not engage until past the stair switchback.
			OpeningDoor->ForEachComponent<UMeshComponent>(false, [Alpha](UMeshComponent* Mesh)
			{
				if (UMaterialInstanceDynamic* DoorFade = Mesh->CreateDynamicMaterialInstance(0))
				{
					DoorFade->SetScalarParameterValue(P02WallFadeParam, Alpha);
				}
			});

			if (OpenElapsed >= DoorOpenDuration)
			{
				OpeningDoor->SetActorHiddenInGame(true);
				UE_LOG(LogP02Playtest, Display,
					TEXT("Mid Boss door faded out over %.2fs and hidden"), DoorOpenDuration);
			}
		}
	}

	if (P02HudVisible() && GEngine && !bDoorOpen && Door.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(2200204, 0.1f, FColor::Orange,
			FString::Printf(TEXT("MID BOSS DOOR  hold %.1f/%.1fs   chasers %d   (hold E at the door)"),
				HoldElapsed, HoldSeconds, SpawnCount));
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

	const float RequestedTestY = P02PlaytestCVars::OccluderTestPlayerY.GetValueOnGameThread();
	if (RequestedTestY > -99990.0f && PlayerPawn.IsValid())
	{
		FVector TestLocation = PlayerPawn->GetActorLocation();
		TestLocation.Y = RequestedTestY;
		PlayerPawn->SetActorLocation(TestLocation, false, nullptr, ETeleportType::TeleportPhysics);
		P02PlaytestCVars::OccluderTestPlayerY.AsVariable()->Set(-99999.0f, ECVF_SetByCode);
		UE_LOG(LogP02Playtest, Display, TEXT("Occluder test: player teleported to Y=%.1f"), RequestedTestY);
	}

	if (!PlayerPawn.IsValid() || !PlayerController.IsValid())
	{
		bPlayerInitialized = false;
		return;
	}

	// Re-applied on a timer as well as on change: enemies spawn mid-run with their own bars.
	const int32 HudState = P02HudVisible() ? 1 : 0;
	const float Now = GetWorld()->GetTimeSeconds();
	if (HudState != LastHudState || (HudState == 0 && Now >= HudReapplyTime))
	{
		LastHudState = HudState;
		HudReapplyTime = Now + 0.5f;
		ApplyHudVisibility(HudState != 0);
	}

	if (P02PlaytestCVars::PromptTest.GetValueOnGameThread() != 0 && PlayerPawn.IsValid())
	{
		P02DrawPrompt(GetWorld(), PlayerPawn->GetActorLocation() + FVector(0.0f, 0.0f, 200.0f),
			TEXT("E"), FColor::White, 1.0f);
	}

	if (P02PlaytestCVars::RunReport.GetValueOnGameThread() != 0)
	{
		P02PlaytestCVars::RunReport.AsVariable()->Set(0, ECVF_SetByCode);
		LogRunReport();
	}

	// Falling out of the level is a death. Height alone, no footprint test: every walkable floor
	// in the map sits above this line, and the only way under it is through a hole.
	if (PlayerHealth > 0.0f)
	{
		const float KillZ = P02PlaytestCVars::PlayerKillZ.GetValueOnGameThread();
		if (PlayerPawn->GetActorLocation().Z < KillZ)
		{
			UE_LOG(LogP02Playtest, Warning, TEXT("Player fell out of the level at Z=%.0f (kill below %.0f)"),
				PlayerPawn->GetActorLocation().Z, KillZ);
			ApplyPlayerDamage(P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread() * 10.0f);
		}
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
	// Our own HUD, installed at runtime so no Blueprint GameMode has to be edited.
	PC->ClientSetHUD(AP02PlaytestHUD::StaticClass());

	StartLocation = Pawn->GetActorLocation();
	RespawnLocation = StartLocation;
	LastPlayerLocation = StartLocation;
	Milestones.Reset();
	RecordMilestone(TEXT("START"));
	PlayerHealth = P02PlaytestCVars::PlayerMaxHealth.GetValueOnGameThread();
	bPlayerInitialized = true;
	if (ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		// This quarter-view test layer has no jump. The template still binds Space
		// to Jump, so disable the character jump capability at runtime and reuse
		// Space as the primary dash input.
		Character->JumpMaxCount = 0;
		Character->StopJumping();
		UCharacterMovementComponent* Movement = Character->GetCharacterMovement();
		Movement->JumpZVelocity = 0.0f;

		// Face the direction of travel while keeping the fixed quarter-view camera
		// independent from character rotation. CharacterMovement keeps the last
		// movement-facing rotation after input is released.
		Character->bUseControllerRotationPitch = false;
		Character->bUseControllerRotationYaw = false;
		Character->bUseControllerRotationRoll = false;
		Movement->bUseControllerDesiredRotation = false;
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
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

	if (GetWorld()->GetMapName().Contains(TEXT("L_Interior_FinalBossCandidates_TEST")))
	{
		SpawnFinalBossCandidateTestFlow();
	}

	UE_LOG(LogP02Playtest, Display, TEXT("Player initialized: %s"), *Pawn->GetName());
}

void UP02PlaytestSubsystem::SpawnFinalBossCandidateTestFlow()
{
	if (bCandidateFlowSpawned)
	{
		return;
	}
	bCandidateFlowSpawned = true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags |= RF_Transient;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.Name = TEXT("MCP_TEST_MidBossEnemy");
	AP02PlaytestEnemy* Enemy = GetWorld()->SpawnActor<AP02PlaytestEnemy>(
		AP02PlaytestEnemy::StaticClass(), FVector(7000.0f, 1100.0f, 340.0f),
		FRotator(0.0f, 180.0f, 0.0f), SpawnParams);
	if (Enemy)
	{
		Enemy->MaxHealth = 34.0f;
		Enemy->MoveSpeed = 120.0f;
		Enemy->AttackDamage = 0.0f;
	}

	SpawnParams.Name = TEXT("MCP_TEST_BridgeBreaker");
	GetWorld()->SpawnActor<AP02BridgeBreaker>(
		AP02BridgeBreaker::StaticClass(), FVector(5250.0f, 1100.0f, 320.0f),
		FRotator::ZeroRotator, SpawnParams);

	SpawnParams.Name = TEXT("MCP_TEST_MidBossDoorGate");
	GetWorld()->SpawnActor<AP02MidBossDoorGate>(
		AP02MidBossDoorGate::StaticClass(), FVector(6300.0f, 1100.0f, 320.0f),
		FRotator::ZeroRotator, SpawnParams);

	SpawnParams.Name = TEXT("MCP_TEST_ColorLock");
	GetWorld()->SpawnActor<AP02ColorLockPuzzle>(
		AP02ColorLockPuzzle::StaticClass(), FVector(3225.0f, -500.0f, -900.0f),
		FRotator::ZeroRotator, SpawnParams);

	SpawnParams.Name = TEXT("MCP_TEST_MidBossInteraction");
	GetWorld()->SpawnActor<AP02TransformStairInteraction>(
		AP02TransformStairInteraction::StaticClass(), FVector(9150.0f, 1100.0f, 380.0f),
		FRotator::ZeroRotator, SpawnParams);

	UE_LOG(LogP02Playtest, Display, TEXT("Final Boss candidate test flow spawned"));
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
	// Holding an interaction locks out the swing. The Mid Boss door leans on this: you cannot
	// hold the door open and clear the chaser at the same time, so the only way through is to
	// cut the approach off instead of out-fighting it.
	if (PlayerController->IsInputKeyDown(EKeys::E))
	{
		return;
	}

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

	for (TActorIterator<AP02BridgeBreaker> It(GetWorld()); It; ++It)
	{
		if (IsValid(*It) && It->ApplyHit(AttackCenter, Radius, PlayerPawn->GetActorLocation()))
		{
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
	RecordMilestone(TEXT("END"));
	UE_LOG(LogP02Playtest, Display, TEXT("Run complete: time=%.2fs distance=%.1fuu"),
		GetElapsedTime(), TravelDistance);
	LogRunReport();
}

void UP02PlaytestSubsystem::RecordMilestone(const FString& Label)
{
	FP02RunMilestone& Entry = Milestones.AddDefaulted_GetRef();
	Entry.Label = Label;
	Entry.Time = GetElapsedTime();
	Entry.Distance = TravelDistance;
	UE_LOG(LogP02Playtest, Display, TEXT("Milestone %6.2fs %8.0fuu  %s"),
		Entry.Time, Entry.Distance, *Label);
}

void UP02PlaytestSubsystem::ApplyHudVisibility(bool bVisible)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// The template's life bar is a UMG widget, not a screen message, so DisableAllScreenMessages
	// never touched it. Collapse every widget in this world instead.
	for (TObjectIterator<UUserWidget> It; It; ++It)
	{
		UUserWidget* Widget = *It;
		if (IsValid(Widget) && Widget->GetWorld() == World)
		{
			Widget->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}

	// Enemy life bars ride on widget components, and enemies keep spawning during a run.
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		It->ForEachComponent<UWidgetComponent>(false, [bVisible](UWidgetComponent* Component)
		{
			Component->SetVisibility(bVisible);
		});
	}
}

void UP02PlaytestSubsystem::LogRunReport() const
{
	UE_LOG(LogP02Playtest, Display, TEXT("===== RUN REPORT ====="));
	float PreviousTime = 0.0f;
	for (const FP02RunMilestone& Entry : Milestones)
	{
		UE_LOG(LogP02Playtest, Display, TEXT("  %7.2fs  %8.0fuu  (+%6.2fs)  %s"),
			Entry.Time, Entry.Distance, Entry.Time - PreviousTime, *Entry.Label);
		PreviousTime = Entry.Time;
	}
	UE_LOG(LogP02Playtest, Display, TEXT("  total %.2fs  %.0fuu  %s"),
		GetElapsedTime(), TravelDistance, bRunComplete ? TEXT("(complete)") : TEXT("(in progress)"));
	UE_LOG(LogP02Playtest, Display, TEXT("======================"));
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
		PlayerPawn->SetActorLocation(RespawnLocation, false, nullptr, ETeleportType::TeleportPhysics);
		if (ACharacter* Character = Cast<ACharacter>(PlayerPawn.Get()))
		{
			Character->GetCharacterMovement()->StopMovementImmediately();
		}
		LastPlayerLocation = RespawnLocation;
	}
	UE_LOG(LogP02Playtest, Warning, TEXT("Player defeated: respawned at %s"),
		*RespawnLocation.ToCompactString());
}

void UP02PlaytestSubsystem::DrawPlaytestHUD() const
{
	if (!GEngine || !P02HudVisible())
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
	RespawnLocation = StartLocation;
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
