#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "P02FieldTelemetry.generated.h"

/** Passive local recorder; never changes movement, collision, or level geometry. */
UCLASS()
class P02DUNGEON_API UP02FieldTelemetry : public UTickableWorldSubsystem
{
 GENERATED_BODY()
public:
 virtual void Initialize(FSubsystemCollectionBase& Collection) override;
 virtual void Deinitialize() override;
 virtual void Tick(float DeltaTime) override;
 virtual TStatId GetStatId() const override;
 virtual bool IsTickableWhenPaused() const override { return true; }
 void Event(const FString& Type,const FString& Detail=TEXT(""));
 void PromptDrawn();
 void Observation(bool Open);
 void BeforeTravel(const FString& Destination);
 void Finish(const FString& Status);
private:
 UFUNCTION() void OnPawnHit(AActor* Self,AActor* Other,FVector Impulse,const FHitResult& Hit);
 TWeakObjectPtr<APawn> BoundPawn;
 FVector ContactNormal=FVector::ZeroVector;
 double ContactClock=-100;
 bool ContactActive=false,AttemptActive=false,FellNearBlocker=false;
 bool SampleJump=false;
 bool Enabled=false,Started=false,Finished=false,Image=false,Prompt=false,HavePosition=false;
 double StartClock=0,LastClock=0,LastSample=0,LastFlush=0,PromptClock=-100,Distance=0,SampleDistance=0,Stall=0;
 double Interval=.2,FlushInterval=2,StallSeconds=1,StallSpeed=10,JumpLimit=1200;
 int64 Sequence=0;
 FVector Previous=FVector::ZeroVector;
 FString Folder,RunId,Route=TEXT("common"),Zone=TEXT("unmapped"),ExpectedDestination;
 FString Samples,Events;
 TSharedPtr<class FJsonObject> Config,Metadata;
 struct FZone { FString Id,Route,Enter; FVector Min,Max; bool Active=false; int Visits=0; double Candidate=-1; };
 TArray<FZone> Zones;
 bool LeftReachedGoal=false,LeftEntered=false,WasStall=false;
 void Start();
 void Flush();
 void SaveMetadata();
 void UpdateZones(const FVector& P,double Now);
};
