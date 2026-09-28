import unreal,json
from pathlib import Path
R=Path('C:/Users/jinsu/Documents/Codex/2026-09-26/new-chat')
OUT=R/'outputs/submission/verification'
def v(x):return [x.x,x.y,x.z]
result={}
for tag,path in [('field','/Game/Portfolio02/Levels/던전외부필드_v09-17_A'),('dungeon','/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종_보수_v02')]:
 unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(path)
 actors=unreal.GameplayStatics.get_all_actors_of_class(unreal.EditorLevelLibrary.get_editor_world(),unreal.Actor)
 rows={a.get_name():{'class':a.get_class().get_path_name(),'label':a.get_actor_label(),'location':v(a.get_actor_location()),'scale':v(a.get_actor_scale3d()),'collision':a.get_actor_enable_collision()} for a in actors}
 original=json.loads((R/('outputs/web-export/evidence/'+tag+'-editor-native.json')).read_text(encoding='utf-8'))['actors']
 before={a['name']:a for a in original}
 diffs=[]
 for n,a in rows.items():
  if n not in before:diffs.append({'new':n});continue
  b=before[n]
  for k,bv in [('class',b['class']),('label',b['label']),('location',b['transform_ue']['translation']),('scale',b['transform_ue']['scale']),('collision',b['collision_enabled'])]:
   av=a[k]
   same=all(abs(x-y)<.001 for x,y in zip(av,bv)) if isinstance(av,list) else av==bv
   if not same:diffs.append({'actor':n,'property':k,'before':bv,'after':av})
 for n in before:
  if n not in rows and n!='ChaosDebugDrawActor':diffs.append({'removed':n})
 component_diffs=[]
 for a in actors:
  b=before.get(a.get_name(),{});bc={c['name']:c for c in b.get('components',[])}
  for c in a.get_components_by_class(unreal.StaticMeshComponent):
   if c.get_name() not in bc:continue
   actual={'mesh':c.static_mesh.get_path_name() if c.static_mesh else None,'materials':[m.get_path_name() if m else None for m in c.get_materials()],'collision_enabled':str(c.get_collision_enabled()),'collision_profile':str(c.get_collision_profile_name()),'pawn_response':str(c.get_collision_response_to_channel(unreal.CollisionChannel.ECC_PAWN))}
   for k,av in actual.items():
    bv=bc[c.get_name()].get(k)
    if av!=bv:component_diffs.append({'actor':a.get_name(),'component':c.get_name(),'property':k,'before':bv,'after':av})
 d={'map':path,'actor_count':len(rows),'geometry_and_name_differences':diffs,'mesh_material_collision_differences':component_diffs,'ignored_session_generated_actor':'ChaosDebugDrawActor (rendering-session debug actor; absent in NullRHI commandlet)','actors':rows}
 if tag=='field':
  c=next(a for a in actors if isinstance(a,unreal.P02OutdoorReviewController))
  d['destination']=str(c.get_editor_property('destination'));d['entrance']=v(c.get_editor_property('entrance'));d['entrance_half_width']=c.get_editor_property('entrance_half_width')
 result[tag]=d
(OUT/'submission-asset-readback.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
