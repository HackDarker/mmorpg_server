local dbOpCodes = dbOpCodes
local dbPacketHandler = dbPacketHandler
local msgpack  = msgpack

-----------------------------------------------------------------------------------------------------------------------------
--处理查询包
function c_procQuery(accountId, charId, pack)
	
end

-----------------------------------------------------------------------------------------------------------------------------
--处理执行包
function c_procExec(accountId, charId, pack)
    print("c_procExec===========",accountId,charId)
end

print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxaaaa",msgpack,msgpack.getSize)