global BMT_Active = 0
global floater

Rollout BMT "Delta Bone Tools" width:230 height:300
(
	
	button CaptureTransform "Capture Transform" pos:[5,5] width:190 height:33
	button MirrorApply "Apply Transform" pos:[5,38] width:190 height:33
	
	local rotationTransform = quat 0 0 0 0
	
	on CaptureTransform pressed  do
	(
		rotationTransform = selection[1].transform
	)
	
	on MirrorApply pressed do
	(
		
		trans = copy rotationTransform

		pos = selection[1].pos
		
		transformMatrix = matrix3 	[-1, 0, 0] [0, 1, 0] [0, 0, 1] [0, 0, 0]
		flipMatrix = matrix3 			[1, 0, 0] [0, -1, 0] [0, 0, 1] [0, 0, 0]
		
		
		
		selection[1].transform = flipMatrix * rotationTransform * transformMatrix
		
		selection[1].pos = pos
		
		format "Angles: %\n" angles
		
	)
	
)

MacroScript RunBoneToolsUtility
category: "Export"
tooltip: "Start Delta Studios bone tools utility."
(
	
	if BMT_Active < 1 then
	(
		floater = newRolloutFloater "Delta Bone Tools" 218 400

		addRollout BMT floater

		cui.RegisterDialogBar floater
		cui.DockDialogBar floater #cui_dock_left
		
		BMT_Active += 1
	)
	
)

MacroScript CloseBoneToolsUtility 
category: "Export"
tooltip: "Close Delta Studios bone tools utility."
(
	
	if BMT_Active > 0 then
	(
		
		removeRollout BMT
		cui.UnRegisterDialogBar floater
		
		closeRolloutFloater floater
		
		BMT_Active -= 1
		
	)
	
)