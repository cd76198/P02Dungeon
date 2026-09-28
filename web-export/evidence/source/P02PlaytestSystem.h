// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/HUD.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "P02PlaytestSystem.generated.h"

class ACharacter;
class APawn;
class UBoxComponent;
class UCameraComponent;
class UMeshComponent;
class UPrimitiveComponent;
class USceneComponent;
class USpringArmComponent;
class UStaticMeshComponent;

/** Fixed quarter-view camera that can follow any player pawn. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02QuarterViewCameraRig : public AActor
{
	GENERATED_BODY()

public:
	AP02QuarterViewCameraRig();
	virtual void Tick(float DeltaSeconds) override;

	void SetTrackedPawn(APawn* InPawn);
	void ApplyCameraSettings(float InPitch, float InYaw, float InDistance, float InFOV);
	float GetActualCameraDistance() const;
	bool IsCameraCollisionFixApplied() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera")
	float CameraPitch = -50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera")
	float CameraYaw = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera", meta = (ClampMin = "100.0"))
	float CameraDistance = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera", meta = (ClampMin = "20.0", ClampMax = "120.0"))
	float CameraFOV = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Camera")
	bool bEnableCameraCollision = false;

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Camera")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "P02|Camera")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "P02|Camera")
	TObjectPtr<UCameraComponent> Camera;

	TWeakObjectPtr<APawn> TrackedPawn;

	/** Fades the fade-material walls while the player stands in a P02_WallFade trigger box. */
	void UpdateWallFade(float DeltaSeconds);

	TArray<TWeakObjectPtr<UMeshComponent>> WallFadeTargets;
	TArray<float> WallFadeAlphas;
	TArray<uint8> WallFadeIgnoresHeight;
	bool bWallFadeTargetsCached = false;
};

/** Minimal moving door for encounter-clear tests. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02PlaytestDoor : public AActor
{
	GENERATED_BODY()

public:
	AP02PlaytestDoor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "P02|Door")
	void SetDoorOpen(bool bShouldOpen);

	UFUNCTION(BlueprintPure, Category = "P02|Door")
	bool IsDoorOpen() const { return bOpen; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Door")
	FVector OpenOffset = FVector(0.0f, 0.0f, 400.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Door", meta = (ClampMin = "1.0"))
	float OpenSpeed = 350.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Door")
	FVector DoorScale = FVector(0.5f, 10.0f, 3.0f);

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Door")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "P02|Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	FVector ClosedRelativeLocation = FVector::ZeroVector;
	bool bOpen = false;
};

/** Simple collision-driven enemy used only by the transient test course. */
UCLASS()
class P02DUNGEON_API AP02PlaytestEnemy : public AActor
{
	GENERATED_BODY()

public:
	AP02PlaytestEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	void SetEncounterActive(bool bActive) { bEncounterActive = bActive; }
	float GetHealth() const { return CurrentHealth; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float MoveSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float AttackDamage = 10.0f;

	/** Seconds between enemy hits. Level designer decision, not a balance derivation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	float AttackCooldown = 1.5f;

	/** Box fill colour. sRGB hex fixed by the deck: normal monsters #C97A7A, Mid Boss #A84F4F.
	 *  Set in the constructor rather than inline so UHT does not have to parse the conversion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Enemy")
	FLinearColor BodyColor;

	/** Repaints the box straight away. A spawn site cannot just assign BodyColor: SpawnActor
	 *  runs BeginPlay before it returns, so the property would be set after the paint. */
	UFUNCTION(BlueprintCallable, Category = "P02|Enemy")
	void SetBodyColor(const FLinearColor& InColor);

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Enemy")
	TObjectPtr<UStaticMeshComponent> EnemyMesh;

	float CurrentHealth = 100.0f;
	float NextAttackTime = 0.0f;
	bool bEncounterActive = false;
};

/** Starts an encounter, tracks enemy destruction, and opens a linked door. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02EncounterTrigger : public AActor
{
	GENERATED_BODY()

public:
	AP02EncounterTrigger();
	virtual void BeginPlay() override;

	void ConfigureRuntimeTest(const TArray<AP02PlaytestEnemy*>& InEnemies, AP02PlaytestDoor* InDoor);

	UFUNCTION(BlueprintCallable, Category = "P02|Encounter")
	void ActivateEncounter();

	UFUNCTION(BlueprintPure, Category = "P02|Encounter")
	int32 GetRemainingEnemies() const;

	UFUNCTION(BlueprintPure, Category = "P02|Encounter")
	bool IsCleared() const { return bCleared; }

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "P02|Encounter")
	TArray<TObjectPtr<AP02PlaytestEnemy>> EncounterEnemies;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "P02|Encounter")
	TObjectPtr<AP02PlaytestDoor> LinkedDoor;

private:
	UFUNCTION()
	void HandleTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEnemyDestroyed(AActor* DestroyedActor);

	void BindEnemies();
	void ClearEncounter();

	UPROPERTY(VisibleAnywhere, Category = "P02|Encounter")
	TObjectPtr<UBoxComponent> TriggerBox;

	bool bActivated = false;
	bool bCleared = false;
};

/** Marker for the start of a measured playtest run. Placement is designer-owned. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02StartMarker : public AActor
{
	GENERATED_BODY()

public:
	AP02StartMarker();

private:
	UPROPERTY(VisibleAnywhere, Category = "P02|Start")
	TObjectPtr<USceneComponent> SceneRoot;
};

/** Completes a run after the required encounter has cleared. */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02EndTrigger : public AActor
{
	GENERATED_BODY()

public:
	AP02EndTrigger();
	virtual void BeginPlay() override;
	void SetRequiredEncounter(AP02EncounterTrigger* InEncounter) { RequiredEncounter = InEncounter; }

private:
	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, Category = "P02|End")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditInstanceOnly, Category = "P02|End")
	TObjectPtr<AP02EncounterTrigger> RequiredEncounter;

	bool bTriggered = false;
};

/**
 * Minimal wall-occlusion comparison trigger.
 * It only changes rendering for tagged upper wall sections; collision is retained.
 */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02OccluderTestTrigger : public AActor
{
	GENERATED_BODY()

public:
	AP02OccluderTestTrigger();
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	void SetUpperSectionsHidden(bool bShouldHide);

	UPROPERTY(VisibleAnywhere, Category = "P02|Occlusion Test")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, Category = "P02|Occlusion Test")
	FName UpperSectionTag = TEXT("MCP_TEST_OCCLUDER_UPPER");

	TArray<TWeakObjectPtr<AActor>> UpperSections;
};

/**
 * Minimal Final Boss candidate-flow interaction used only by the comparison map.
 * A test Mid Boss death reveals the device; pressing E nearby lowers the tagged
 * stair pieces and removes the temporary blocker.
 */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02TransformStairInteraction : public AActor
{
	GENERATED_BODY()

public:
	AP02TransformStairInteraction();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure, Category = "P02|Candidate Test")
	bool IsInteractionAvailable() const { return bInteractionAvailable; }

	UFUNCTION(BlueprintPure, Category = "P02|Candidate Test")
	bool IsActivated() const { return bActivated; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Candidate Test")
	float InteractionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Candidate Test")
	float EnemyActivationRadius = 2600.0f;

	/** Steps sit at their final height already, so nothing moves; the plate just fades. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Candidate Test")
	float LowerDistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Candidate Test")
	float MoveDuration = 1.25f;

	/** Seconds after the Mid Boss dies before the stairs lower on their own. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Candidate Test")
	float AutoStartDelay = 2.0f;

private:
	UFUNCTION()
	void HandleEnemyDestroyed(AActor* DestroyedActor);

	void SetInteractionAvailable();
	void StartMechanism();

	UPROPERTY(VisibleAnywhere, Category = "P02|Candidate Test")
	TObjectPtr<UStaticMeshComponent> MechanismMesh;

	UPROPERTY(VisibleAnywhere, Category = "P02|Candidate Test")
	TObjectPtr<UStaticMeshComponent> LeverHandleMesh;

	/** The lever's second job: putting the weak span back after the Mid Boss is down. */
	TWeakObjectPtr<class AP02BridgeBreaker> Bridge;
	TArray<TWeakObjectPtr<AP02PlaytestEnemy>> TrackedEnemies;
	TArray<TWeakObjectPtr<AActor>> MovableStairs;
	TArray<FVector> StairStartLocations;
	TArray<TWeakObjectPtr<AActor>> Blockers;
	float MoveElapsed = 0.0f;
	float AutoStartCountdown = -1.0f;
	bool bEncounterActivated = false;
	bool bInteractionAvailable = false;
	bool bActivated = false;
};

/**
 * PIE-only playtest layer. It leaves map assets untouched and provides camera,
 * dash, basic damage, timing, distance logging, and a transient end-to-end test course.
 */
/**
 * The weak span of the bridge. Every actor named P02_BridgeWeakSection* drops together, so the
 * deck and its rails are split in the editor rather than in code.
 *
 * Breaking it is the only answer to the Mid Boss door: the hold cannot be finished while the
 * chasers keep arriving, so the player has to remove the approach instead of out-fighting it.
 */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02BridgeBreaker : public AActor
{
	GENERATED_BODY()

public:
	AP02BridgeBreaker();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** Returns true once the span has started dropping. */
	UFUNCTION(BlueprintPure, Category = "P02|Bridge")
	bool IsBroken() const { return bBroken; }

	/** Called by the playtest attack sweep. Returns true when the hit landed. */
	bool ApplyHit(const FVector& AttackCenter, float AttackRadius, const FVector& PlayerLocation);

	/** Puts the span back where it started, undamaged. Driven by the Mid Boss lever. */
	void Restore();

private:
	/** Tints every section by HitsTaken / hits needed, so damage reads without a second material. */
	void ApplyDamageTint();

	TArray<TWeakObjectPtr<AActor>> Sections;
	TArray<FVector> SectionStartLocations;
	TArray<TWeakObjectPtr<UMeshComponent>> SectionMeshes;
	TArray<FLinearColor> SectionBaseColors;
	FBox SectionBounds = FBox(ForceInit);
	float DropElapsed = -1.0f;
	float ShakeRemaining = 0.0f;
	int32 HitsTaken = 0;
	bool bBroken = false;
};

/**
 * Mid Boss Door pressure. The door and the spawn point are plain level actors found by name -
 * P02_MidBossDoor and P02_MidBossEnemySpawn - so both stay movable from the editor.
 *
 * Crossing the spawn point's X while heading for the door arms the chase; the door needs an
 * uninterrupted hold, and any damage resets it. That is the whole encounter: the player has to
 * decide between holding and turning to fight.
 */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02MidBossDoorGate : public AActor
{
	GENERATED_BODY()

public:
	AP02MidBossDoorGate();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	/** TEST VALUE - tuned against the measured enemy arrival time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float HoldSeconds = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float InteractRadius = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float DoorOpenDistance = 620.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float DoorOpenDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float RespawnDelay = 0.1f;

	/** Measured 1799uu spawn-to-door, less the 150uu attack range, arriving in 4.0s. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float EnemyMoveSpeed = 410.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float EnemyHealth = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Mid Boss Door")
	float EnemyAttackDamage = 10.0f;

private:
	void SpawnChaser();

	TWeakObjectPtr<AActor> Door;
	TWeakObjectPtr<AActor> SpawnVolume;
	TWeakObjectPtr<AP02PlaytestEnemy> Chaser;
	TWeakObjectPtr<AP02BridgeBreaker> Bridge;
	FVector DoorClosedLocation = FVector::ZeroVector;
	/** Standing spot on the approach side of the door, captured before the door moves. */
	FVector DoorApproachLocation = FVector::ZeroVector;
	FVector SpawnLocation = FVector::ZeroVector;
	float ArmLineX = 0.0f;
	float HoldElapsed = 0.0f;
	float OpenElapsed = -1.0f;
	float RespawnCountdown = -1.0f;
	float LastPlayerHealth = -1.0f;
	float ChaserSpawnTime = 0.0f;
	int32 SpawnCount = 0;
	bool bArmed = false;
	bool bDoorOpen = false;
	bool bArrivalMeasured = false;
	/** Latched on the first break. Restoring the bridge brings back the crossing, not the chase. */
	bool bChaseRetired = false;
};

/**
 * Color-order lock in front of the Final Boss Door. Pads, door and END volume are found by
 * name so the whole thing stays placeable and movable from the editor:
 *   P02_ColorPad_Red / _Green / _Blue, MCP_TEST_Candidate1_BigDoor, P02_EndTrigger
 */
UCLASS(BlueprintType)
class P02DUNGEON_API AP02ColorLockPuzzle : public AActor
{
	GENERATED_BODY()

public:
	AP02ColorLockPuzzle();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Color Lock")
	float PadRadius = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Color Lock")
	float DoorOpenDistance = 620.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Color Lock")
	float DoorOpenDuration = 1.0f;

	/** Wrong-answer enemies. Weak test enemies, no new class - see the plan's ban on new types.
	 *  410 matches the Mid Boss chaser: at 250 the walk over read as dead time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Color Lock")
	float PunisherMoveSpeed = 410.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Color Lock")
	float PunisherHealth = 34.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "P02|Color Lock")
	float PunisherAttackDamage = 10.0f;

private:
	void SpawnPunishers();

	TArray<FName> Answer;
	TArray<FName> Entered;
	TMap<FName, TWeakObjectPtr<AActor>> Pads;
	/** Every actor named MCP_TEST_Candidate1_BigDoor* moves together, so the gate can be built
	 *  from several pieces (slab + arch) instead of one box. */
	TArray<TWeakObjectPtr<AActor>> DoorParts;
	TArray<FVector> DoorPartStarts;
	TWeakObjectPtr<AActor> EndVolume;
	TArray<FVector> PunisherSpawns;
	TArray<TWeakObjectPtr<AP02PlaytestEnemy>> Punishers;
	float OpenElapsed = -1.0f;
	float WrongAnswerTime = 0.0f;
	float LastPlayerHealth = -1.0f;
	int32 WrongAnswerCount = 0;
	bool bPunishActive = false;
	bool bFirstContactMeasured = false;
	bool bSolved = false;
	bool bReachedEnd = false;
};

/**
 * Canvas HUD for interaction prompts. DrawDebugString cannot take a font, and scaling the
 * engine's tiny debug font up just magnifies its bitmap - hence a real Canvas pass.
 * Prompts are pushed each frame and consumed by DrawHUD.
 */
UCLASS()
class P02DUNGEON_API AP02PlaytestHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	void AddPrompt(const FVector& WorldLocation, const FString& Text, const FColor& Color, float Scale);

private:
	struct FP02Prompt
	{
		FVector Location = FVector::ZeroVector;
		FString Text;
		FColor Color = FColor::White;
		float Scale = 1.0f;
	};

	TArray<FP02Prompt> Prompts;

	/** Built at runtime around the imported FontFace - see DrawHUD for why. */
	UPROPERTY()
	TObjectPtr<UFont> PromptFont;

	bool bFontLookupDone = false;
};

/** One logged point in a run. The three test paths are compared by diffing these. */
struct FP02RunMilestone
{
	FString Label;
	float Time = 0.0f;
	float Distance = 0.0f;
};

UCLASS()
class P02DUNGEON_API UP02PlaytestSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual TStatId GetStatId() const override;

	void NotifyEncounterStarted(int32 EnemyCount);
	void NotifyEncounterUpdated(int32 EnemyCount);
	void NotifyEncounterCleared();
	void CompleteRun();
	void ApplyPlayerDamage(float DamageAmount);

	/** Moves the defeat respawn point. Set once the bridge drops, so a fall does not send the
	 *  player back across the whole level to re-walk ground they have already cleared. */
	void SetRespawnLocation(const FVector& InLocation) { RespawnLocation = InLocation; }

	/** Stamps a run event with elapsed time and travelled distance. The run report is the
	 *  measured record the plan calls for - nothing here is estimated. */
	void RecordMilestone(const FString& Label);
	void LogRunReport() const;

	float GetPlayerHealth() const { return PlayerHealth; }
	float GetElapsedTime() const;
	float GetTravelDistance() const { return TravelDistance; }
	bool IsRunComplete() const { return bRunComplete; }

private:
	void TryInitializePlayer();
	/** Hides or restores every UMG widget and widget component - the template life bars included. */
	void ApplyHudVisibility(bool bVisible);
	void TickDash(float DeltaSeconds);
	void TryBasicAttack();
	void DrawPlaytestHUD() const;
	void SpawnTransientTestCourse();
	void SpawnFinalBossCandidateTestFlow();
	void ResetPlayerAfterDefeat();
	AActor* SpawnTestBlock(const FName& ActorName, const FVector& Location, const FVector& Scale);

	TWeakObjectPtr<APlayerController> PlayerController;
	TWeakObjectPtr<APawn> PlayerPawn;
	TWeakObjectPtr<AP02QuarterViewCameraRig> CameraRig;
	TWeakObjectPtr<AP02EncounterTrigger> ActiveEncounter;

	TArray<FP02RunMilestone> Milestones;
	FVector StartLocation = FVector::ZeroVector;
	FVector RespawnLocation = FVector::ZeroVector;
	FVector LastPlayerLocation = FVector::ZeroVector;
	FVector DashDirection = FVector::ForwardVector;
	float RunStartTime = 0.0f;
	float RunEndTime = 0.0f;
	float TravelDistance = 0.0f;
	float HudReapplyTime = 0.0f;
	int32 LastHudState = -1;
	float PlayerHealth = 100.0f;
	float NextDashTime = 0.0f;
	float DashEndTime = 0.0f;
	float NextAttackTime = 0.0f;
	int32 EncounterRemaining = 0;
	bool bPlayerInitialized = false;
	bool bCourseSpawned = false;
	bool bCandidateFlowSpawned = false;
	bool bIsDashing = false;
	bool bEncounterActive = false;
	bool bEncounterCleared = false;
	bool bRunComplete = false;
};
