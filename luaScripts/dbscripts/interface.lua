local pb = require 'protobuf'

local dbOpCodes = dbOpCodes
local dbPacketHandler = dbPacketHandler
local msgpack  = msgpack

 pb.register_file('/home/source2222/proto/built/db.pb')
 print('registered file','/home/source2222/proto/built/db.pb')


-----------------------------------------------------------------------------------------------------------------------------
--处理查询包
function c_procQuery(accountId, charId, pack)
	print("c_procQuery===========enter============11111",accountId,charId,pack)
	print("c_procQuery===========22222222",msgpack.getOpcode(pack))

	local pbMsg,pbSize = msgpack.getContents(pack)

	print("c_procQuery333333333333333333",pbMsg,pbSize,pb.decode)

	local pbname = 'db.L2D_ROLE_LIST'
    local req_msg = pb.decode(pbname,pbMsg,pbSize)
    if not req_msg then
        print("c_procQuery pb decode error",pbname)
        return
    end

    for k,v in pairs(req_msg) do
    	print("ttttttttttttttttttt===",k,v,type(v))
    end

    print("ccccccccccccccccccccccccccc_procQuery",req_msg.account_id)
end

-----------------------------------------------------------------------------------------------------------------------------
--处理执行包
function c_procExec(accountId, charId, pack)
    print("c_procExec===========",accountId,charId)
end

print("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",msgpack,pb)