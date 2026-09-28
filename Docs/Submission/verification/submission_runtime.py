import unreal,json,time,traceback,math
from pathlib import Path
R=Path('C:/Users/jinsu/Documents/Codex/2026-09-26/new-chat')
OUT=R/'outputs/submission/verification'; CMD=OUT/'command.json'
unreal.EditorPythonScripting.set_keep_python_script_alive(True)
LES=unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
queue=[];active=None;history=[];samples=[];lastdump=0;started=time.monotonic();lastmap=None
def vec(v):return [v.x,v.y,v.z]
def world():
 ws=unreal.EditorLevelLibrary.get_pie_worlds(False)
 return ws[0] if ws else None
def actors(w):return unreal.GameplayStatics.get_all_actors_of_class(w,unreal.Actor)
def actor(w,n):return next(a for a in actors(w) if a.get_name()==n)
def state():
 w=world();d={'map':w.get_path_name() if w else None,'action':active,'queued':len(queue),'seconds':round(time.monotonic()-started,2)}
 if w:
  p=unreal.GameplayStatics.get_player_pawn(w,0);pc=unreal.GameplayStatics.get_player_controller(w,0)
  if p:d.update(position=vec(p.get_actor_location()),velocity=vec(p.get_velocity()),move_ignored=pc.is_move_input_ignored(),end=unreal.WebExportProbeLibrary.run_complete(p))
 return d
def record(k,**kw):
 d={'event':k,'clock':time.monotonic()-started,**kw};history.append(d)
 unreal.log('SUBMISSION_QA '+json.dumps(d,ensure_ascii=False))
def dump():
 (OUT/'runtime-status.json').write_text(json.dumps(state(),ensure_ascii=False,indent=2),encoding='utf-8')
 (OUT/'runtime-events.json').write_text(json.dumps(history,ensure_ascii=False,indent=2),encoding='utf-8')
 (OUT/'runtime-positions.json').write_text(json.dumps(samples,ensure_ascii=False),encoding='utf-8')
def tick(dt):
 global active,queue,lastdump,lastmap
 try:
  now=time.monotonic();w=world();p=unreal.GameplayStatics.get_player_pawn(w,0) if w else None
  if CMD.exists():
   req=json.loads(CMD.read_text(encoding='utf-8-sig'));CMD.unlink()
   if req.get('replace',False):queue=[];active=None
   queue+=req['actions'];record('command',actions=req['actions'])
  current=w.get_path_name() if w else None
  if current!=lastmap:record('world_change',old=lastmap,new=current,position=vec(p.get_actor_location()) if p else None);lastmap=current
  if active is None and queue:
   active=queue.pop(0);active['started']=now;active['origin']=vec(p.get_actor_location()) if p else None;record('action_begin',action=dict(active))
  if active:
   a=active;t=a['type'];elapsed=now-a['started'];done=False
   if t=='begin':LES.editor_request_begin_play();done=True
   elif t=='stop':LES.editor_request_end_play();done=True
   elif t=='load':LES.load_level(a['map']);done=True
   elif t=='wait':done=elapsed>=a['seconds']
   elif t=='key':unreal.WebExportProbeLibrary.test_key(unreal.GameplayStatics.get_player_controller(w,0),a['key'],a['down']);done=True
   elif t=='move' and p:
    loc=p.get_actor_location();dx=a['xy'][0]-loc.x;dy=a['xy'][1]-loc.y;dist=math.hypot(dx,dy)
    if dist<=a.get('radius',30):done=True
    else:p.add_movement_input(unreal.Vector(dx/dist,dy/dist,0),1,False)
    if elapsed>a.get('timeout',30):record('move_timeout',target=a['xy'],state=state());active=None;queue=[]
   elif t=='exec':exec(a['code'],globals());done=True
   elif t=='snapshot':
    rows=[]
    for ac in actors(w):
     if a.get('names') and ac.get_name() not in a['names']:continue
     rows.append({'name':ac.get_name(),'class':ac.get_class().get_name(),'location':vec(ac.get_actor_location()),'bounds':[vec(v) for v in ac.get_actor_bounds(False)],'collision':ac.get_actor_enable_collision(),'hidden':ac.get_editor_property('hidden')})
    (OUT/(a['name']+'.json')).write_text(json.dumps({'state':state(),'actors':rows},ensure_ascii=False,indent=2),encoding='utf-8');done=True
   elif t=='quit':dump();unreal.unregister_slate_post_tick_callback(handle);unreal.SystemLibrary.quit_editor();return
   if done:record('action_end',type=t,state=state());active=None
  if now-lastdump>.5:
   st=state();samples.append({k:v for k,v in st.items() if k!='action'});dump();lastdump=now
 except:
  record('exception',trace=traceback.format_exc());active=None;queue=[];dump()
record('editor_startup',map=unreal.EditorLevelLibrary.get_editor_world().get_path_name())
handle=unreal.register_slate_post_tick_callback(tick)
