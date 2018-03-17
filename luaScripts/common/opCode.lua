opCodes = {
	CMSG_REGION_NOTICE       = 9,     --大区通知数据
	BASE_MSG_BEGIN		     = 500,
	MAP_MSG_BEGIN            = 3000,
	FRIEND_MSG_BEGIN    	 = 3200,  --好友
	ITEM_MSG_BEGIN           = 3300,  --物品

	MB_MSG_BEGIN             = 1500,
	PMB_MSG_BEGIN			 = 2500,
	GAME_MSG_COUNT           = 20000,
}

dbOpCodes = {
	DB_MSG_BEGIN   = 100,
	DM_MSG_BEGIN   = 2000,
	DP_MSG_BEGIN   = 5000,
	DB_MSG_COUNT   = 20000,
}

dbOpCodes.L2D_ROLE_LIST = 1
dbOpCodes.D2L_ROLE_LIST = 2
dbOpCodes.L2D_ROLE_CREATE = 3
dbOpCodes.D2L_ROLE_CREATE = 4
dbOpCodes.L2D_ROLE_DELETE = 5
dbOpCodes.D2L_ROLE_DELETE = 6
dbOpCodes.L2D_ROLE_SELECT = 7
dbOpCodes.D2L_ROLE_SELECT = 8
dbOpCodes.L2D_ROLE_VERIFY = 9

dbPacketHandler = {} 
