/* 	Animation and Static Model Export Script 
	Written January 4, 2013
	Ange Yaghi
*/

global floater
global YSGE_Active = 0

-- File Data
	
	MODEL_FILE_MAGIC_NUMBER = 0x50F1A
	MODEL_FILE_VERSION = 0x5
	
	ANIMATION_FILE_MAGIC_NUMBER = 0x50F1AA
	ANIMATION_FILE_VERSION = 0x0
	
-- Software Codes
	SW_3DS_MAX = 0x1
	
-- Compiler Codes
	CC_RAW = 0x1
	CC_COMPILED = 0x2

-- OBJECT_TYPES

	TYPE_GEOMETRY = 0
	TYPE_BONE = 1
	TYPE_GROUP = 2
	TYPE_PLANE = 3
	TYPE_INSTANCE = 4
	TYPE_UNDEFINED = -1

-- FLAGS

	IncludeAnimationData = false
	IncludeBoneData = false
	IncludeTextureCoordinates = false
	IncludeRotation = false
	IncludeMaterialData = false
	IncludeSmoothingData = false

-- ANIMATION KEYS

	KEY_POSITION = 0
	--KEY_POSITION_Y = 1
	--KEY_POSITION_Z = 2
	
	KEY_ROTATION = 1
	--KEY_ROTATION_Y = 4
	--KEY_ROTATION_Z = 5
	
	KEY_SCALE = 2
	--KEY_SCALE_Y = 7
	--KEY_SCALE_Z = 8

-- GLOBAL SCRIPT VALUES

	object_list = objects
	SceneRotation = rotateXMatrix -90
	ERROR_STATE = false

function GetObjectIndex obj =
(
	
	return getUserProp obj "YDS_OBJECT_INDEX"
	
)

function GenerateObjectIndices list =
(
	
	for i=1 to list.count do
	(
		
		setUserProp list[i] "YDS_OBJECT_INDEX" i
		
	)	
	
)

function GetObjectParentInstance index list = 
(
	
	local parent_instance = 0
	
	obj = list[index]
	
	for i=1 to (index-1) do
	(
		
		if (areNodesInstances obj list[i]) then
			parent_instance = i
		
	)
	
	-- RETURN:
		parent_instance
	
)

function GetRoot obj =
(
	
	parent = obj.parent
	
	if (parent == undefined) then 
	(
		print obj.name
		return obj
	)
	
	return (GetRoot parent)
	
)

function GetObjectType index list =
(
	
	obj = list[index]
	
	parent_instance = GetObjectParentInstance index list
	
	local type
	
	if ( classOf obj == PolyMeshObject ) then type = TYPE_GEOMETRY
	else if ( classOf obj == Editable_Poly) then type = TYPE_GEOMETRY
	else if ( classOf obj == BoneGeometry) then type = TYPE_BONE
	else if (classOf obj == Editable_Mesh) then type = TYPE_GEOMETRY
	else if (classOf obj == Plane) then type = TYPE_PLANE
	else if ( isGroupHead obj) then type = TYPE_GROUP
	else type = TYPE_UNDEFINED
	
	if (parent_instance != 0) then
	(
		
		if (type != TYPE_UNDEFINED) then
		(
			
			type = TYPE_INSTANCE
			
		)
		
	)
	
	-- RETURN:
		type
	
)

function GetSkeletonIndex obj = 
(
	
	return getUserProp obj "YDS_SKELETON_INDEX"
	
)

function GenerateSkeletonIndices list root = 
(
	
	currentIndex = 0
	setUserProp root "YDS_SKELETON_INDEX" currentIndex
	
	for i=1 to list.count do
	(
		
		if ((GetObjectType i list) == TYPE_BONE) then
		(
			
			if ((GetRoot list[i]) == root) then
			(
				
				setUserProp list[i] "YDS_SKELETON_INDEX" currentIndex
				currentIndex = currentIndex + 1
				
			)
			
		)
		
	)
	
)

function GenerateSkeletonsIndices list = 
(
	
	for i=1 to list.count do
	(
		
		if ((GetObjectType i list) == TYPE_BONE) then
		(
			
			if ((GetRoot list[i]) == list[i]) then
			(
				
				GenerateSkeletonIndices list list[i] 
				
			)
			
		)
		
	)
	
)

function WriteFloat4 file float4 =
(
	WriteFloat file float4.w
	WriteFloat file float4.x
	WriteFloat file float4.y
	WriteFloat file float4.z
)

function WriteFloat3 file float3 =
(
	
	WriteFloat file float3.x
	WriteFloat file float3.y
	WriteFloat file float3.z
	
)

function WriteFloat2 file float2 =
(
	
	WriteFloat file float2.x
	WriteFloat file float2.y
	
)

function GetTransform obj =
(
	
	parent  = obj.parent
	local ret
	
	if (parent == undefined) then ret = obj.transform
	else
	(
		
		ptrans = obj.parent.transform
		inv_ptrans = inverse ptrans
		
		ret = obj.transform * inv_ptrans
		
	)	
	
	return ret
	
)

function GetFullTransform obj =
(
	
	parent = obj.parent
	
	if (parent == undefined) then return obj.transform
	
	return ((GetTransform obj) * (GetFullTransform parent))
	
	
)

function GetSign vec =
(
	
	ret = [1, 1, 1]
	
	if (vec.x < 0) then ret.x = -1
	if (vec.y < 0) then ret.y = -1
	if (vec.z < 0) then ret.z = -1
	
	return ret
	
)

function GetObjectScale obj =
(
	
	parent = obj.parent
	if (parent == undefined) then return obj.scale
	
	return obj.scale * (GetSign parent.scale)
	
)

function GetFlipped obj = 
(
	
	sign = GetSign (obj.scale)
	
	if (sign.x < 0 or sign.y < 0 or sign.z < 0) then return true
	--else if (obj.parent != undefined) then return (GetFlipped obj.parent)
	else return false
	
)

function GetObjLocalTransform obj =
(
	
	obj_trans = (GetTransform obj)
		
	S_inv = inverse SceneRotation
	S = SceneRotation
	
	T = obj_trans * S
	T = S_inv * T	
	
	return T
	
)


function WriteObjectHeader file index list =
(
	
	obj = list[index]
	
	-- INSTANCE DATA
	parent_instance = GetObjectParentInstance index list
	
	-- PARENT DATA
	parent = obj.parent
	parent_index = 0
	
	skeleton_index = -1
	
	if ((GetObjectType index list) == TYPE_BONE) then
	(
		
		skeleton_index = GetSkeletonIndex obj
		format "SKELETON: %\n" skeleton_index
		
	)
	
	if (parent != undefined) then 
	(
		
		parent_index = GetObjectIndex parent
		
	)
	
	ModelUsesBones = ( obj.modifiers[#Skin] != undefined ) 
	if (ModelUsesBones) then modpanel.setCurrentObject obj.skin
	
	ModelUsesBones = ModelUsesBones and ( ( skinOps.GetNumberBones obj.skin ) > 0 )
	
	if ( ModelUsesBones and ( obj.modifiers[#Skin] != obj.modifiers[1] ) ) then
	(
		
		msg = "Error: The Skin modifier of " + obj.name + " is not at the top of the stack. Skinning data will be ignored."
		MessageBox msg
		
		ModelUsesBones = false
		
		ERROR_STATE = true
		
	)
	
	if (ModelUsesBones) then 
	(
		
		select obj
		setCommandPanelTaskMode #modify
		modpanel.setCurrentObject obj
		
	)
	
	object_type = GetObjectType index list
	
	if (object_type == TYPE_UNDEFINED) then
	(
		
		msg = "Error: Undefined/unsupported type found in scene: " + obj.name
		MessageBox msg
		
		ERROR_STATE = true
		
	)
	
	else
	(
	
		-- HIGH LEVEL DATA 
		
		WriteLong file (index - 1)						-- Index 
		WriteLong file (parent_index - 1)			-- Parent
		WriteLong file (parent_instance - 1)		-- Parent instance
		WriteLong file object_type						-- Type
		if (ModelUsesBones) then WriteLong file 1			-- Uses bones
		else WriteLong file 0
		WriteLong file skeleton_index
		WriteString file obj.name		
		-- Name
		if (obj.material != undefined) then
			WriteString file obj.material.name		   	-- Material Name
		
		else
			WriteString file "" -- Empty string
		
		print parent_index
		print parent_instance
		
		-- TRANSFORMATION DATA
		
		input_trans =  matrix3 1
		obj_trans = (GetTransform obj)
		
		S_inv = inverse SceneRotation
		S = SceneRotation
		
		scaleT = scaleMatrix obj.scale
		invScaleT = inverse scaleT
		
		T = obj_trans * S
		T = S_inv * T
		--T = invScaleT * T
		
		t0 = transMatrix obj_trans.translationPart
		r0 = (obj.rotation as matrix3)
		
		print r0
		
		print r0.scalePart
		
 		angles = ( (T.rotationPart) as eulerangles )
		
		--trans = matrix3 1
		objScale = GetObjectScale obj
		scaleTest = scaleMatrix objScale
		scaleTest = SceneRotation * scaleTest 
		print scaleTest
		
		if (IncludeRotation) then
		(
			
			sign = GetSign obj.scale
			scaleVec = sign
			scaleVec.x = sign.x
			scaleVec.y = sign.z
			scaleVec.z = sign.y
			
			scaleVec = scaleVec * T.scalePart
			
		)
		
		else
		(
			
			sign = GetSign obj.scale
			scaleVec = sign
			scaleVec.x = sign.x
			scaleVec.y = sign.z
			scaleVec.z = sign.y
			
			scaleVec = scaleVec * T.scalePart
			
		)

		WriteFloat3 file ( T.translationPart )
		WriteFloat3 file ( ((T.rotationPart) as eulerangles) )
		WriteFloat4 file T.rotationPart
		WriteFloat3 file ( scaleVec )
		
		pos = t0.translationPart
		poss = obj_trans.translationPart
		angless = (obj_trans.rotationPart as eulerangles)
		
		print T
		format "SCENE ROT | POSITION = [%, %, %], ROTATION = [%, %, %], SCALE = [% % %]\n" 	pos.x pos.y pos.z angles.x angles.y angles.z scaleVec.x scaleVec.y scaleVec.z
		format "STANDARD | POSITION = [%, %, %], ROTATION = [%, %, %], SCALE = [% % %]\n" 		poss.x poss.y poss.z angless.x angless.y angless.z obj.scale.x obj.scale.y obj.scale.z
		
		-- VERTEX FORMAT DATA
		
		if (object_type == TYPE_GEOMETRY) then
		(
			
			tmesh = snapshotAsMesh obj  									-- Trimesh Snapshot
			
			if (IncludeTextureCoordinates) then 
				WriteLong file ( (	meshop.getNumMaps tmesh ) - 1) 	-- Number of Texture Coordinate Channels
			
			else
				WriteLong 		file 0												-- Ignore if texture coordinate channels are not included
			
			WriteLong 		file tmesh.numverts								-- Number of Vertices
			WriteLong 		file tmesh.numfaces								-- Number of Faces
			
		)
		
		else
		(
			
			WriteLong 	file 0
			WriteLong 	file 0
			WriteLong 	file 0
			
		)
		
		if (GetFlipped obj) then 
		(
			
			WriteLong file 1 -- Flip Normals
			print "Normals are flipped\n"
			
		)
		
		else WriteLong file 0
		
	)
	
)

function WriteGeometryData file index list =
(
	
	obj = list[index]
	
	-- Check to make sure this object has a valid bone system
	ModelUsesBones = ( obj.modifiers[#Skin] != undefined ) 
	if (ModelUsesBones) then modpanel.setCurrentObject obj.skin
	
	ModelUsesBones = ModelUsesBones and ( ( skinOps.GetNumberBones obj.skin ) > 0 )
	
	object_type = GetObjectType index list
	
	if (object_type == TYPE_PLANE) then
	(
		
		print obj.length
		print obj.width
		WriteFloat file obj.length
		WriteFloat file obj.width
		
	)
		
	
	if (object_type == TYPE_GEOMETRY) then
	(
		
		if (ModelUsesBones and obj.modifiers.count > 1) then modpanel.setCurrentObject obj.modifiers[obj.modifiers.count - 1]
		else if (obj.modifiers.count > 1) then modpanel.setCurrentObject obj.modifiers[obj.modifiers.count - 1]
		
		format "COUNT %\n" obj.modifiers.count 
		
		flip = ((SceneRotation.determinantSign * obj.transform.determinantSign) < 0)
		if (flip) then format "FLIP % %\n" obj.transform.determinantSign SceneRotation.determinantSign

		tmesh = snapshotAsMesh obj
		
		O = (obj.transform)
		O_inv = inverse O
		
		S = SceneRotation
		S_inv = inverse S
		S_O_inv = O_inv * S 
		T = O * S
		T = S_inv * T
		
		--print S_inv
		--print O_inv
		--print S_O_inv
		--print T
		
		-- Output all Vertex Data
		for vert = 1 to obj.numverts do
		(
			
			V_o = (getVert tmesh vert) 
			V = V_o * S_O_inv
			
			--print old_vertex
			--print SceneRotation
			--print obj.transform
				
			--format "% -> %\n" V_o V
			--format "% -> %\n" (V_o * S) (V * T)
			--print "-\n"
			WriteFloat3 file V
			
			print V
			
			if (IncludeMaterialData) then
			(
				
				WriteLong file 0 -- MATERIAL DATA NOT YET SUPPORTED
				
			)
		
		)
		
		if (IncludeBoneData) then
		(
			
			if (ModelUsesBones) then 
			(
				
				modpanel.setCurrentObject obj.skin
			
				for vert = 1 to obj.numverts do
				(
					
					if (ModelUsesBones) then
					(
						
						n_bones = ( skinOps.GetVertexWeightCount obj.skin vert )
						WriteLong file n_bones
						
						print "--"
						
						for bone_index = 1 to n_bones do
						(
							
							bone_ID = skinOps.GetVertexWeightBoneID obj.skin vert bone_index
							
							bone_name = skinOps.GetBoneName obj.skin bone_ID 1
							bone_obj = getNodeByName bone_name
							bone_skeleton_index = GetSkeletonIndex bone_obj
							
							print bone_ID
							WriteLong file bone_skeleton_index
							
						)
						
						for bone_index = 1 to n_bones do
						(
							
							WriteFloat file ( skinOps.GetVertexWeight obj.skin vert bone_index )
							
						)
						
					)	
					
				)
				
			)
			
		)
		
		-- UV Coordinate Channel Data
		
		num_uv_channels = ( (	meshop.getNumMaps tmesh ) - 1)
		
		if (IncludeTextureCoordinates and num_uv_channels > 0) then
		(
			
			for texture_channel = 1 to  num_uv_channels do
			(
				
				num_coordinates = meshop.getNumMapVerts tmesh texture_channel
				
				WriteLong file num_coordinates
				
				for coordinate = 1 to num_coordinates do
				(
					
					WriteFloat2 file (meshop.getMapVert tmesh texture_channel coordinate)
					
				)
				
			)
			
		)
		
		-- Output All Face Data
		for face_index = 1 to tmesh.numfaces do
		(
			
			face = getFace tmesh face_index
			
			if (flip) then
			(
				
				WriteLong file (face.x - 1)
				WriteLong file (face.z - 1)
				WriteLong file (face.y - 1)
				
			)
			
			else
			(
				
				WriteLong file (face.x - 1)
				WriteLong file (face.y - 1)
				WriteLong file (face.z - 1)
				
			)
			
			if (IncludeSmoothingData) then
			(
				
				WriteLong file (getFaceSmoothGroup tmesh face_index)	
				
			)
			
			if (IncludeTextureCoordinates) then
			(
			
				for texture_channel = 1 to num_uv_channels do
				(
					
					map_face = meshop.getMapFace tmesh texture_channel face_index
					
					if (flip) then
					(
						
						WriteLong file (map_face.x - 1)
						WriteLong file (map_face.z - 1)
						WriteLong file (map_face.y - 1)
						
					)
					
					else
					(
						
						WriteLong file (map_face.x - 1)
						WriteLong file (map_face.y - 1)
						WriteLong file (map_face.z - 1)
						
					)
					
				)
				
			)
			
		)
		
		-- Write the list of bones that makes up the bone system of the object
		if (IncludeBoneData and ModelUsesBones) then
		(
			
			n_bones = ( skinOps.GetNumberBones obj.skin )
			
			if (n_bones > 0) then
			(
				
				WriteLong file 1
				
				main_root_bone = undefined
				for bone_index = 1 to n_bones do
				(
					
					bone_name = skinOps.GetBoneName obj.skin bone_index 1
					bone_obj = getNodeByName bone_name
					bone_ID = getObjectIndex bone_obj
					root_bone = GetRoot bone_obj
					
					if (main_root_bone == undefined) then
					(
						
						main_root_bone = root_bone
						
					)
					
					if (root_bone != main_root_bone) then
					(
						
						msg = "Error: Object " + obj.name + " has multiple skeletons."
						MessageBox msg
						
					)
					
				)	
				
				WriteLong file (( getObjectIndex main_root_bone ) - 1)
				
			)
			
			else
			(
				
				WriteLong file 0
				
			)
			
		)
		
	)
	
)

function IsObjectAnimated obj = 
(
	
	local ret
	
	if obj.position.isAnimated or obj.rotation.isAnimated then
	(
		
		ret = True
		
	)
	
	else ret = False
	
	return ret
	
)

function WriteRotationKeys file obj =
(
	
	WriteLong file KEY_ROTATION
	
	countOffset = ftell file 
	WriteLong file 0 -- Temporary value
	
	frames_counted = #()
	key_count = 0
	
	for key_index=1 to 3 do
	(
		
		if (key_index == 1) then keys = obj.rotation.controller.x_rotation.keys
		else if (key_index == 2) then keys = obj.rotation.controller.y_rotation.keys
		else keys = obj.rotation.controller.z_rotation.keys
	
		for key=1 to keys.count do
		(
			
			key_time = keys[key].time
			
			if (findItem frames_counted key_time == 0) then
			(
			
				at time(key_time) T = GetObjLocalTransform obj
				
				WriteLong file keys[key].time
				WriteFloat4 file (T.rotationPart)
				
				format "KEY | TYPE: %, TIME: %, VALUE: %\n" "Rotation" keys[key].time T.rotationPart
				
				append frames_counted key_time
				
				key_count += 1
				
			)
			
		)
		
	)
	
	currentOffset = ftell file
	fseek file countOffset #seek_set
	WriteLong file key_count
	fseek file currentOffset #seek_set
	
)

function WritePositionKeys file obj =
(
	
	WriteLong file KEY_POSITION
	
	countOffset = ftell file 
	WriteLong file 0 -- Temporary value
	
	frames_counted = #()
	key_count = 0
	
	for key_index=1 to 3 do
	(
		
		if (key_index == 1) then keys = obj.position.controller.x_position.keys
		else if (key_index == 2) then keys = obj.position.controller.y_position.keys
		else keys = obj.position.controller.z_position.keys
	
		for key=1 to keys.count do
		(
			
			key_time = keys[key].time
			
			if (findItem frames_counted key_time == 0) then
			(
				
				at time(key_time) T = GetObjLocalTransform obj
				
				WriteLong file keys[key].time
				WriteFloat3 file (T.translationPart)
				
				format "KEY | TYPE: %, TIME: %, VALUE: %\n" "Position" keys[key].time T.translationPart
				
				append frames_counted key_time
				
				key_count += 1
				
			)
			
		)
		
	)
	
	currentOffset = ftell file
	fseek file countOffset #seek_set
	WriteLong file key_count
	fseek file currentOffset #seek_set
	
)

function WriteAnimationKeyData file index list =
(
	
	obj = list[index]
	
	WritePositionKeys file obj
	WriteRotationKeys file obj
	
)

function WriteTimeTags file = 
(
	
	count = FrameTagManager.GetTagCount()
	WriteLong file count
	
	for i =1 to count do
	(
		
		tag_ID = FrameTagManager.GetTagID i
		tag_time = FrameTagManager.GetTimeByID tag_ID
		tag_name = FrameTagManager.GetNameByID tag_ID
		
		WriteString file tag_name
		WriteLong file tag_time
		
	)
	
)

function WritePadding file n =
(
	
	for i = 1 to n do
	(
		
		WriteByte file 0
		
	)
	
)

function WriteObjectAnimationHeader file index list =
(
	
	obj = list[index]
	WriteString file obj.name		
	
)


Rollout DeltaAnimation "Delta Animation" width:230 height:300
(
	
	button ExportButton "Export Animation" pos:[5,5] width:190 height:33
	checkbox SelectedOnlyCheck "Selection" pos: [5, 50] width:90 height:25 checked:false 
	checkbox RotateCheck "Rotate X" pos:[105,50] width:90 height:25 checked:true
	
	on ExportButton pressed do
	(
		
		if (SelectedOnlyCheck.tristate == 1) then export_list = selection as array
		else export_list =objects as array
		
		if export_list.count > 0 then
		(
		
			file_name = getSaveFileName caption:"Delta Animation File" types: "Delta Animation (*.daf) |*.daf| All Files (*.*) |*.*|"
		
			if (not IncludeRotation) then 
			(
				SceneRotation = (matrix3 [1, 0, 0] [0, 1, 0] [0, 0, 1] [0, 0, 0])
			)
			else
			(
				SceneRotation = (matrix3 [-1, 0, 0] [0, 0, -1] [0, 1, 0] [0, 0, 0])				
			)
			
			if (file_name != undefined) then
			(
				
				file = fopen file_name "wb"
				adjust_scene_rotation = RotateCheck.triState
				
				-- Filter List
				for obj = 1 to export_list.count do
				(
					
					if (export_list[obj] != undefined) then
					(
						
						type = GetObjectType obj export_list
						if (type == TYPE_UNDEFINED) then
						(
							
							msg = "Error: Undefined/unsupported type found in scene: " + export_list[obj].name
							MessageBox msg
							
							export_list = deleteItem export_list obj
							
						)
						
						if (not IsObjectAnimated export_list[obj]) then
						(
							
							export_list = deleteItem export_list obj
							
						)
						
					)
					
				)
				
				-- HEADER =====================================
				
				WriteLong file ANIMATION_FILE_MAGIC_NUMBER			-- Magic Number
				WriteLong file ANIMATION_FILE_VERSION 					-- File Version
				WriteLong file SW_3DS_MAX 								-- Last program to modify this file
				WriteLong file CC_RAW										-- Write raw code
				
				WriteLong file export_list.count
				
				-- END HEADER ================================= 	

				for obj = 1 to export_list.count do
				(
					
					if (export_list[obj] != undefined) then
					(
						
						WriteObjectAnimationHeader file obj export_list
						WriteAnimationKeyData file obj export_list 
						
					)
					
					else
					(
						
						format "ERROR UNDEFINED OBJECT: %\n" obj
						
					)
					
				)
				
				WriteTimeTags file
				
				fclose file				
				
			)
			
		)
			
	)
	
)

Rollout YSGE "YSGE Rollout" width:230 height:300
(
	
	button ExportButton "Export Model" pos:[5,5] width:190 height:33
	
	checkbox AnimationDataCheck "Animation Data" pos:[5,50] width:90 height:25 checked:false
	checkbox BoneDataCheck "Bone Data" pos:[105,50] width:90 height:25 checked:false
	checkbox TextureCoordinateData "Texture Coordinates" pos:[5,80] width:90 height:25 checked:true
	checkbox RotateCheck "Rotate X" pos:[105,80] width:90 height:25 checked:true
	checkbox MaterialDataCheck "Material Data" pos:[5,110] width:90 height:25 checked:false
	checkbox SmoothingDataCheck "Smoothing Data" pos:[105, 110] width:90 height:25 checked:true
	checkbox SelectedOnlyCheck "Selection" pos: [5, 140] width:90 height:25 checked:false 
	
	on ExportButton pressed  do
	(
	
		if (SelectedOnlyCheck.tristate == 1) then export_list = selection as array
		else export_list =objects as array
		
		if export_list.count > 0 then
		(
			
			file_name = getSaveFileName caption:"Yaghi Static Model File" types: "Yaghi Static (*.ysc) |*.ysc| All Files (*.*) |*.*|"
			
			IncludeAnimationData = 		(AnimationDataCheck.triState == 1)
			IncludeBoneData =	 			(BoneDataCheck.triState == 1)
			IncludeTextureCoordinates = (TextureCoordinateData.triState == 1)
			IncludeRotation = 				(RotateCheck.triState == 1)
			IncludeMaterialData = 			(MaterialDataCheck.triState == 1)
			IncludeSmoothingData = 		(SmoothingDataCheck.triState == 1)
			
			if (not IncludeRotation) then 
			(
				SceneRotation = (matrix3 [1, 0, 0] [0, 1, 0] [0, 0, 1] [0, 0, 0])
			)
			else
			(
				SceneRotation = (matrix3 [-1, 0, 0] [0, 0, -1] [0, 1, 0] [0, 0, 0])				
			)
			
			if (file_name != undefined) then
			(
				
				file = fopen file_name "wb"
				adjust_scene_rotation = RotateCheck.triState
				
				-- Filter List
				for obj = 1 to export_list.count do
				(
					
					if (export_list[obj] != undefined) then
					(
						
						type = GetObjectType obj export_list
						if (type == TYPE_UNDEFINED) then
						(
							
							msg = "Error: Undefined/unsupported type found in scene: " + export_list[obj].name
							MessageBox msg
							
							export_list = deleteItem export_list obj
							
						)
						
					)
					
				)
				
				-- HEADER =====================================
				
				WriteLong file MODEL_FILE_MAGIC_NUMBER			-- Magic Number
				WriteLong file MODEL_FILE_VERSION 					-- File Version
				WriteLong file SW_3DS_MAX 					-- Last program to modify this file
				WriteLong file CC_RAW							-- Write raw code
				
				WriteByte file (AnimationDataCheck.triState)
				WriteByte file (BoneDataCheck.triState)
				
				WriteByte file (TextureCoordinateData.triState)
				WriteByte file (MaterialDataCheck.triState )
				
				WriteByte file (SmoothingDataCheck.triState)
				WritePadding file 3
				
				WriteLong file export_list.count
				
				-- END HEADER ================================= 
				
				GenerateObjectIndices export_list
				GenerateSkeletonsIndices export_list
				
				for obj = 1 to export_list.count do
				(
					
					if (export_list[obj] != undefined) then
					(
						
						WriteObjectHeader file obj export_list
						WriteGeometryData file obj export_list
						--WriteAnimationKeyData file obj export_list
						
					)
					
					else
					(
						
						format "ERROR UNDEFINED OBJECT: %\n" obj
						
					)
					
				)
				
				fclose file
				
			)
			
		)
	
	)
	
)

MacroScript RunDeltaStudiosUtility 
category: "Export"
icon: #("Delta", 1)
tooltip: "Start Delta Studios export utility."
(
	
	if YSGE_active < 1 then
	(
		floater = newRolloutFloater "Delta Toolbar" 218 400

		addRollout YSGE floater
		addRollout DeltaAnimation floater

		cui.RegisterDialogBar floater
		cui.DockDialogBar floater #cui_dock_left
		
		YSGE_active += 1
	)
	
)

MacroScript CloseDeltaStudiosUtility 
category: "Export"
icon: #("Delta", 2)
tooltip: "Close Delta Studios export utility."
(
	
	if YSGE_active > 0 then
	(
		
		removeRollout YSGE
		removeRollout DeltaAnimation
		cui.UnRegisterDialogBar floater
		
		closeRolloutFloater floater
		
		YSGE_active -= 1
		
	)
	
)