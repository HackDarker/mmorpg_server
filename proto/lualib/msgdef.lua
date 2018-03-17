local NAME_TO_ID = {
	['db.L2D_ROLE_LIST'] = 10001,
	['db.D2L_ROLE_LIST'] = 10002,
}
local ID_TO_NAME = {
	[10001] = {'db','L2D_ROLE_LIST'},
	[10002] = {'db','D2L_ROLE_LIST'},
}
return {name_to_id = NAME_TO_ID,id_to_name = ID_TO_NAME}