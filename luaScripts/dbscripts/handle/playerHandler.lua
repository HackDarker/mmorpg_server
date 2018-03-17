local dbPacketHandler = dbPacketHandler

local function handleRoleCreate(accountId, playerId, pack)
	print("handleCharacterCreate===================",accountId,playerId)
end
dbPacketHandler[dbOpCodes.L2D_ROLE_CREATE] = handleRoleCreate