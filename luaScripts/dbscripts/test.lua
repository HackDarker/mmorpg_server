local pb = require "protobuf"

addr = io.open("/home/source2222/proto/built/db.pb","rb")
buffer = addr:read "*a"
addr:close()
pb.register(buffer)

local data = {account_id = 123}

print("test11111111111111111")
local buffer = pb.encode("db.L2D_ROLE_LIST", data)
print("test222222222222222222222222")

local t = pb.decode("db.L2D_ROLE_LIST", buffer)

for k,v in pairs(t) do
	print("tttttttttttttttt",k,v)
end

