#include "P02FieldTelemetry.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

namespace {
FString Csv(FString S){return TEXT("\"")+S.Replace(TEXT("\""),TEXT("\"\""))+TEXT("\"");}
FString Json(const TSharedPtr<FJsonObject>& O){FString S;auto W=TJsonWriterFactory<>::Create(&S);FJsonSerializer::Serialize(O.ToSharedRef(),W);return S;}
bool Read(const FString& P,TSharedPtr<FJsonObject>& O){FString S;return FFileHelper::LoadFileToString(S,*P)&&FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(S),O)&&O.IsValid();}
FVector Vec(const TArray<TSharedPtr<FJsonValue>>& A){return FVector(A[0]->AsNumber(),A[1]->AsNumber(),A[2]->AsNumber());}
bool In(const FVector&P,const FVector&A,const FVector&B,double Margin=0){return P.X>=A.X-Margin&&P.X<=B.X+Margin&&P.Y>=A.Y-Margin&&P.Y<=B.Y+Margin&&P.Z>=A.Z-Margin&&P.Z<=B.Z+Margin;}
}
void UP02FieldTelemetry::Initialize(FSubsystemCollectionBase& C){Super::Initialize(C);
 if(!GetWorld()->IsGameWorld())return;
 // Resolve an explicitly flushed travel handoff only in the destination world.
 const FString Pending=FPaths::ProjectSavedDir()/TEXT("FieldTelemetry/pending_travel.json");
 TSharedPtr<FJsonObject> H;
 if(Read(Pending,H)){
  const FString Expected=FPaths::GetBaseFilename(H->GetStringField(TEXT("destination")));
  if(H->HasField(TEXT("process_id")) && H->GetNumberField(TEXT("process_id"))==FPlatformProcess::GetCurrentProcessId() && GetWorld()->GetMapName().Contains(Expected)){
   TSharedPtr<FJsonObject> R;const FString Dir=H->GetStringField(TEXT("folder"));
   if(Read(Dir/TEXT("run.json"),R)){
    R->SetStringField(TEXT("status"),TEXT("completed"));R->SetStringField(TEXT("actual_destination"),GetWorld()->GetMapName());
    FFileHelper::SaveStringToFile(Json(R),*(Dir/TEXT("run.json")),FFileHelper::EEncodingOptions::ForceUTF8);
    const double Entered=FPlatformTime::Seconds()-H->GetNumberField(TEXT("start_clock"));
    R->SetNumberField(TEXT("dungeon_entered_s"),Entered);
    FFileHelper::SaveStringToFile(Json(R),*(Dir/TEXT("run.json")),FFileHelper::EEncodingOptions::ForceUTF8);
    const FString Row=FString::Printf(TEXT("%s,%.6f,%.3f,%.3f,%.3f,destination,%s,dungeon_entered,%s\n"),*R->GetStringField(TEXT("run_id")),Entered,H->GetNumberField(TEXT("x")),H->GetNumberField(TEXT("y")),H->GetNumberField(TEXT("z")),*H->GetStringField(TEXT("route")),*Csv(TEXT("source_entrance_position;loaded=")+GetWorld()->GetMapName()));FFileHelper::SaveStringToFile(Row,*(Dir/TEXT("events.csv")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,&IFileManager::Get(),FILEWRITE_Append);
   }IFileManager::Get().Delete(*Pending);
  }
 }
 if(!Read(FPaths::ProjectSavedDir()/TEXT("FieldTelemetry/session.json"),Config))return;
 Config->TryGetBoolField(TEXT("enabled"),Enabled);
 if(!Enabled||!GetWorld()->GetMapName().Contains(Config->GetStringField(TEXT("map_filter")))){Enabled=false;return;}
 Config->TryGetNumberField(TEXT("sample_interval"),Interval);Interval=FMath::Max(.05,Interval);
 Config->TryGetNumberField(TEXT("flush_interval"),FlushInterval);FlushInterval=FMath::Max(.2,FlushInterval);
 Config->TryGetNumberField(TEXT("stall_seconds"),StallSeconds);Config->TryGetNumberField(TEXT("stall_speed_uu_s"),StallSpeed);
 Config->TryGetNumberField(TEXT("discontinuity_uu"),JumpLimit);
 for(auto V:Config->GetArrayField(TEXT("zones"))){auto O=V->AsObject();FZone Z;Z.Id=O->GetStringField(TEXT("id"));Z.Route=O->GetStringField(TEXT("route"));Z.Enter=O->GetStringField(TEXT("event"));Z.Min=Vec(O->GetArrayField(TEXT("min")));Z.Max=Vec(O->GetArrayField(TEXT("max")));Zones.Add(Z);}
}
TStatId UP02FieldTelemetry::GetStatId()const{RETURN_QUICK_DECLARE_CYCLE_STAT(UP02FieldTelemetry,STATGROUP_Tickables);}
void UP02FieldTelemetry::Start(){
 const FString Participant=Config->GetStringField(TEXT("participant"));
 // Prevent accidental collection of names or paths: facilitator uses anonymous Tnn IDs.
 if(!Participant.StartsWith(TEXT("T"))||Participant.Len()<2){Enabled=false;return;}
 for(int i=1;i<Participant.Len();++i)if(!FChar::IsDigit(Participant[i])){Enabled=false;return;}
 FString Root=FPaths::ProjectSavedDir()/TEXT("FieldTelemetry/runs");IFileManager::Get().MakeDirectory(*Root,true);
 int Total=0,VersionCount=0,Completed=0;TArray<FString> Dirs;IFileManager::Get().FindFiles(Dirs,*(Root/TEXT("*")),false,true);
 for(const auto&D:Dirs){TSharedPtr<FJsonObject>M;if(!Read(Root/D/TEXT("run.json"),M))continue;
 if(M->GetStringField(TEXT("participant"))!=Participant)continue;
 if(M->GetStringField(TEXT("status"))==TEXT("running")||M->GetStringField(TEXT("status"))==TEXT("travel_pending")){
 M->SetStringField(TEXT("status"),TEXT("technical_error"));M->SetStringField(TEXT("recovery"),TEXT("previous_run_not_closed;recovered_at_next_start"));FFileHelper::SaveStringToFile(Json(M),*(Root/D/TEXT("run.json")),FFileHelper::EEncodingOptions::ForceUTF8);
 }
 ++Total;if(M->GetStringField(TEXT("version"))==Config->GetStringField(TEXT("version")))++VersionCount;
 if(M->GetStringField(TEXT("status"))==TEXT("completed"))++Completed;}
 RunId=FGuid::NewGuid().ToString(EGuidFormats::Digits);Folder=Root/RunId;IFileManager::Get().MakeDirectory(*Folder,true);
 Metadata=MakeShared<FJsonObject>();Metadata->SetStringField(TEXT("run_id"),RunId);
 for(const auto&K:{TEXT("participant"),TEXT("version"),TEXT("exposure"),TEXT("audience")})Metadata->SetStringField(K,Config->GetStringField(K));
 Metadata->SetNumberField(TEXT("overall_attempt"),Total+1);Metadata->SetNumberField(TEXT("version_attempt"),VersionCount+1);Metadata->SetNumberField(TEXT("previous_completions"),Completed);
 Metadata->SetStringField(TEXT("start_utc"),FDateTime::UtcNow().ToIso8601());Metadata->SetStringField(TEXT("map"),GetWorld()->GetMapName());Metadata->SetStringField(TEXT("status"),TEXT("running"));
 Metadata->SetObjectField(TEXT("configuration"),Config);SaveMetadata();
 FFileHelper::SaveStringToFile(TEXT("run_id,sequence,elapsed_s,dt_s,x,y,z,zone,route,movement_input,distance_uu,movement_allowed,observation_open,paused,discontinuity\n"),*(Folder/TEXT("positions.csv")),FFileHelper::EEncodingOptions::ForceUTF8);
 FFileHelper::SaveStringToFile(TEXT("run_id,elapsed_s,x,y,z,zone,route,event,detail\n"),*(Folder/TEXT("events.csv")),FFileHelper::EEncodingOptions::ForceUTF8);
 Started=true;StartClock=LastClock=LastSample=LastFlush=FPlatformTime::Seconds();Event(TEXT("test_start"));Flush();
}
void UP02FieldTelemetry::SaveMetadata(){if(!Metadata.IsValid())return;FString Tmp=Folder/TEXT("run.json.tmp");FFileHelper::SaveStringToFile(Json(Metadata),*Tmp,FFileHelper::EEncodingOptions::ForceUTF8);IFileManager::Get().Move(*(Folder/TEXT("run.json")),*Tmp,true);}
void UP02FieldTelemetry::Flush(){if(!Started)return;
 if(!Samples.IsEmpty()){FFileHelper::SaveStringToFile(Samples,*(Folder/TEXT("positions.csv")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,&IFileManager::Get(),FILEWRITE_Append);Samples.Reset();}
 if(!Events.IsEmpty()){FFileHelper::SaveStringToFile(Events,*(Folder/TEXT("events.csv")),FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM,&IFileManager::Get(),FILEWRITE_Append);Events.Reset();}
 Metadata->SetNumberField(TEXT("last_flush_elapsed_s"),FPlatformTime::Seconds()-StartClock);Metadata->SetNumberField(TEXT("distance_uu"),Distance);SaveMetadata();LastFlush=FPlatformTime::Seconds();
}
void UP02FieldTelemetry::Event(const FString&T,const FString&D){if(!Started||Finished)return;if(T==TEXT("fall_below_kill_z")&&(Zone==TEXT("blocked_old_road")||ContactActive))FellNearBlocker=true;auto P=UGameplayStatics::GetPlayerPawn(this,0);FVector V=P?P->GetActorLocation():Previous;
 Events+=FString::Printf(TEXT("%s,%.6f,%.3f,%.3f,%.3f,%s,%s,%s,%s\n"),*RunId,FPlatformTime::Seconds()-StartClock,V.X,V.Y,V.Z,*Csv(Zone),*Csv(Route),*Csv(T),*Csv(D));}
void UP02FieldTelemetry::PromptDrawn(){if(!Started||Finished)return;PromptClock=FPlatformTime::Seconds();if(!Prompt){Prompt=true;Event(TEXT("observation_prompt_shown"));}}
void UP02FieldTelemetry::Observation(bool Open){if(!Started||Finished)return;if(Open){Event(TEXT("observation_interaction"));Event(TEXT("observation_image_open"));}else Event(TEXT("observation_image_close"));Image=Open;Flush();}
void UP02FieldTelemetry::UpdateZones(const FVector&P,double Now){
 Zone=TEXT("unmapped");
 for(auto&Z:Zones){bool Inside=In(P,Z.Min,Z.Max,Z.Active?60:0);
 if(Inside!=Z.Active){if(Z.Candidate<0)Z.Candidate=Now;if(Z.Id==TEXT("entrance")||Now-Z.Candidate>=.2){Z.Active=Inside;Z.Candidate=-1;
 if(Inside){Zone=Z.Id;++Z.Visits;if(Z.Id==TEXT("blocked_old_road")&&FellNearBlocker){Event(TEXT("blocker_reapproach_after_fall"));FellNearBlocker=false;}Event(Z.Enter,FString::Printf(TEXT("zone=%s;visit=%d"),*Z.Id,Z.Visits));
 if(Z.Id==TEXT("observation")||Z.Id==TEXT("front_merge"))LeftReachedGoal=true;
 if(Z.Id==TEXT("branch")&&Route!=TEXT("common")){Event(TEXT("branch_return"));if(LeftEntered&&!LeftReachedGoal)Event(TEXT("pilgrim_return_to_start"));}
 if(!Z.Route.IsEmpty()&&Z.Route!=TEXT("common")&&Z.Route!=Route){FString Old=Route;Route=Z.Route;Event(TEXT("route_change"),Old+TEXT("->")+Route);if(Route==TEXT("left"))LeftEntered=true;}
 if(Z.Id==TEXT("entrance")&&!Metadata->HasField(TEXT("first_arrival_s"))){Metadata->SetNumberField(TEXT("first_arrival_s"),Now-StartClock);Event(TEXT("front_entrance_first_arrival"));Flush();}
 }else {Zone=Z.Id;Event(TEXT("zone_exit"),Z.Id);}
 }}else Z.Candidate=-1;
 if(Z.Active)Zone=Z.Id;
 }
}
void UP02FieldTelemetry::Tick(float){if(!Enabled||Finished)return;auto P=UGameplayStatics::GetPlayerPawn(this,0);if(!P)return;if(!Started)Start();if(!Started)return;
 if(BoundPawn.Get()!=P){if(BoundPawn.IsValid())BoundPawn->OnActorHit.RemoveDynamic(this,&UP02FieldTelemetry::OnPawnHit);P->OnActorHit.AddDynamic(this,&UP02FieldTelemetry::OnPawnHit);BoundPawn=P;}
 double Now=FPlatformTime::Seconds(),Dt=Now-LastClock;LastClock=Now;
 auto PC=UGameplayStatics::GetPlayerController(this,0);auto C=Cast<ACharacter>(P);auto M=C?C->GetCharacterMovement():nullptr;
 bool Paused=UGameplayStatics::IsGamePaused(this),Allowed=PC&&!PC->IsMoveInputIgnored()&&M&&M->MovementMode!=MOVE_None&&!Paused&&!Image;
 FVector Pos=P->GetActorLocation();double Step=HavePosition?FVector::Dist(Pos,Previous):0;
 bool Jump=HavePosition&&(Step>FMath::Min(JumpLimit,FMath::Max(50.0,FMath::Max(M?M->GetMaxSpeed():0.0f,P->GetVelocity().Size())*Dt*4))||M&&M->bJustTeleported);
 if(Jump){Event(TEXT("position_discontinuity"));Step=0;SampleJump=true;}HavePosition=true;Previous=Pos;Distance+=Step;SampleDistance+=Step;
 bool Input=!P->GetLastMovementInputVector().IsNearlyZero()||!P->GetPendingMovementInputVector().IsNearlyZero();
 UpdateZones(Pos,Now);
 if(Prompt&&(Image||Now-PromptClock>.25)){Prompt=false;Event(TEXT("observation_prompt_hidden"));}
 if(Input&&Allowed&&Dt>0&&Step/Dt<StallSpeed&&!Jump)Stall+=Dt;else{if(WasStall)Event(TEXT("stagnation_candidate_end"));Stall=0;WasStall=false;}
 if(ContactActive&&Now-ContactClock>.3){ContactActive=false;Event(TEXT("blocker_contact_end"));}
 const bool Attempt=ContactActive&&Stall>=StallSeconds&&FVector::DotProduct(P->GetLastMovementInputVector().GetSafeNormal(),-ContactNormal)>.2;
 if(Attempt!=AttemptActive){AttemptActive=Attempt;Event(Attempt?TEXT("blocked_movement_attempt_begin"):TEXT("blocked_movement_attempt_end"));}
 if(Stall>=StallSeconds&&!WasStall){Event(TEXT("stagnation_candidate_begin"),TEXT("input_present;low_displacement;not_proven_collision"));WasStall=true;}
 if(Now-LastSample>=Interval){Samples+=FString::Printf(TEXT("%s,%lld,%.6f,%.6f,%.3f,%.3f,%.3f,%s,%s,%d,%.5f,%d,%d,%d,%d\n"),*RunId,Sequence++,Now-StartClock,Now-LastSample,Pos.X,Pos.Y,Pos.Z,*Csv(Zone),*Csv(Route),Input,SampleDistance,Allowed,Image,Paused,SampleJump);LastSample=Now;SampleDistance=0;SampleJump=false;}
 if(!Metadata->HasField(TEXT("max_walk_speed"))&&M){Metadata->SetNumberField(TEXT("max_walk_speed"),M->MaxWalkSpeed);}
 if(Now-LastFlush>=FlushInterval){
  TSharedPtr<FJsonObject> Note;FString NP=FPaths::ProjectSavedDir()/TEXT("FieldTelemetry/note.json");
  if(Read(NP,Note)){Event(TEXT("facilitator_note"),Note->GetStringField(TEXT("text")));IFileManager::Get().Delete(*NP);}
  Flush();
 }
}
void UP02FieldTelemetry::BeforeTravel(const FString&D){if(!Started||Finished)return;ExpectedDestination=D;
 if(!Metadata->HasField(TEXT("first_arrival_s"))){Metadata->SetNumberField(TEXT("first_arrival_s"),FPlatformTime::Seconds()-StartClock);Zone=TEXT("entrance");Event(TEXT("front_entrance_first_arrival"),TEXT("existing_entrance_trigger"));}
 auto H=MakeShared<FJsonObject>();H->SetNumberField(TEXT("process_id"),FPlatformProcess::GetCurrentProcessId());H->SetNumberField(TEXT("x"),Previous.X);H->SetNumberField(TEXT("y"),Previous.Y);H->SetNumberField(TEXT("z"),Previous.Z);H->SetNumberField(TEXT("start_clock"),StartClock);H->SetStringField(TEXT("route"),Route);H->SetStringField(TEXT("destination"),D);H->SetStringField(TEXT("folder"),Folder);
 H->SetStringField(TEXT("event_row"),FString::Printf(TEXT("%s,%.6f,%.3f,%.3f,%.3f,%s,%s,dungeon_entered,confirmed_destination_load\n"),*RunId,FPlatformTime::Seconds()-StartClock,Previous.X,Previous.Y,Previous.Z,*Csv(Zone),*Csv(Route)));
 FFileHelper::SaveStringToFile(Json(H),*(FPaths::ProjectSavedDir()/TEXT("FieldTelemetry/pending_travel.json")),FFileHelper::EEncodingOptions::ForceUTF8);
 Event(TEXT("dungeon_travel_requested"),D);Metadata->SetStringField(TEXT("expected_destination"),D);Flush();}
void UP02FieldTelemetry::Finish(const FString&S){if(!Started||Finished)return;Event(TEXT("test_end"),S);Metadata->SetStringField(TEXT("status"),S);Metadata->SetNumberField(TEXT("end_elapsed_s"),FPlatformTime::Seconds()-StartClock);Flush();Finished=true;}
void UP02FieldTelemetry::Deinitialize(){if(Started&&!Finished)Finish(ExpectedDestination.IsEmpty()?TEXT("interrupted"):TEXT("travel_pending"));Super::Deinitialize();}

void UP02FieldTelemetry::OnPawnHit(AActor*,AActor* Other,FVector,const FHitResult& Hit){
 if(!Started||Finished||!Other||!Config->HasField(TEXT("blocker_actor_names")))return;
 bool Match=false;for(auto V:Config->GetArrayField(TEXT("blocker_actor_names")))if(Other->GetName()==V->AsString())Match=true;
 if(!Match)return;
 ContactClock=FPlatformTime::Seconds();ContactNormal=Hit.ImpactNormal;
 if(!ContactActive){ContactActive=true;Event(TEXT("blocker_contact_begin"),Other->GetName()+TEXT(";normal=")+ContactNormal.ToCompactString());}
}
