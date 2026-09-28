import unreal,json,traceback,shutil
from pathlib import Path
R=Path('C:/Users/jinsu/Documents/Codex/2026-09-26/new-chat')
field='/Game/Portfolio02/Levels/던전외부필드_v09-17_A'
dungeon='/Game/Portfolio02/Tests/FinalBossCandidates/던전내부최종_보수_v02'
unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).load_level(field)
actors=unreal.get_editor_subsystem(unreal.EditorActorSubsystem).get_all_level_actors()
controllers=[a for a in actors if isinstance(a,unreal.P02OutdoorReviewController)]
assert len(controllers)==1,len(controllers)
a=controllers[0];before=str(a.get_editor_property('destination'))
a.set_editor_property('destination',unreal.Name(dungeon))
assert unreal.EditorLoadingAndSavingUtils.save_map(unreal.EditorLevelLibrary.get_editor_world(),field)
rel='Content/Portfolio02/Levels/던전외부필드_v09-17_A.umap'
shutil.copy2(R/'work/P02Dungeon_export'/rel,R/'outputs/submission/P02Dungeon'/rel)
(R/'outputs/submission/verification/map-change.json').write_text(json.dumps({'actor':a.get_name(),'before':before,'after':str(a.get_editor_property('destination')),'field':field,'dungeon':dungeon},ensure_ascii=False,indent=2),encoding='utf-8')
