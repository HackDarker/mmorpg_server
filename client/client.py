import json
import socket
import struct
import select
import threading
import sys

class NetProtocol(object):
    def __str__(self):
        return 'msg[%s]seq:%d,msgid:%d,body_length:%d,msg(%s)' % (self.msg.DESCRIPTOR.full_name,
            self.seq,self.msgid,self.body_length,str(self.msg)
        )

def parse(filename):
    msg_name_map = {}
    msg_id_map = {}
    with open(filename,'rb') as f:
        for l in f.readlines():
            l = l.strip()
            if l == '':
                continue

            if l.startswith('#'):
                #comments
                continue

            params = l.split('=')
            if len(params) != 2:
                print 'invalid message definition',l
                return

            
            msg_name = params[0].strip()
            msg_id = params[1].strip()

            if not msg_id.isdigit():
                print 'invalid message id(%s)'%msg_id
                return

            msg_id = int(msg_id)
            name_component = msg_name.split('.')
            if len(name_component) != 2:
                print 'invalid message name',msg_name
                return

            module = name_component[0]
            msg = name_component[1]

            msg_name_map['%s.%s' % (module,msg)] = msg_id
            msg_id_map[msg_id] = (module,msg)

    return msg_name_map,msg_id_map
MSG_NAME_MAP,MSG_ID_MAP = parse('../proto/src/msgdef.ini')

class Client:
    def __init__(self):
        self.socket = socket.socket(family=socket.AF_INET,type=socket.SOCK_STREAM)
        self.inputs = []
        self.protocols = []
        self.buffer = bytes('')
        self.dest = 4 << 16 | 1

    def fileno(self):
        return self.socket.fileno()
        
    def connect(self,addr,port):
        self.socket.connect((addr,port))
        
    def _unpack_response(self):
        bytes_length = len(self.buffer)

        curr_cursor = 0
        while bytes_length - curr_cursor >= 10:
            cursor = curr_cursor
            length,seq,msgid = struct.unpack_from(">HII",self.buffer,cursor)
            body_length = length - 8
            cursor += 10

            if bytes_length - cursor < body_length:
                break

            body = ''
            if body_length > 0:
                body = struct.unpack_from("%ds" % body_length,self.buffer,cursor)
                body = body[0]
            cursor += body_length

            r = MSG_ID_MAP[msgid]
            module = __import__(r[0] + '_pb2')
            msg = getattr(module,r[1])()
            msg.ParseFromString(body)

            protocol = NetProtocol()
            protocol.seq = seq
            protocol.msgid = msgid
            protocol.body_length = body_length
            protocol.msg = msg
            protocol.handler_name = '_'.join(r).lower()

            self.protocols.append(protocol)

            curr_cursor = cursor
        
        self.buffer = self.buffer[curr_cursor:]

        return True
        
    def read(self):
        buffer = self.socket.recv(4096)
        if len(buffer) == 0:
            active_clients.remove(self)
        self.buffer += bytes(buffer)
        self._unpack_response()
        for r in self.protocols:
            print(r)
            if hasattr(self,r.handler_name):
                getattr(self,r.handler_name)(r.msg)
        self.protocols = []

    def pack(self,msgid,pb):
        s = pb.SerializeToString()
        real_length = len(s) + 4
        f = '>HH%ds' % len(s)
        print("real_length=========%d====%d=========",len(s),real_length)
        b = struct.pack(f,real_length,msgid,s)
        return b

    def send(self,msg):
        msgid = MSG_NAME_MAP[msg.DESCRIPTOR.full_name]
        print msgid
        b = self.pack(msgid,msg)
        self.socket.send(b)

    def login(self,accountId):
        import db_pb2
        pb = db_pb2.L2D_ROLE_LIST()
        pb.account_id = accountId
        self.send(pb)

active_clients = []
def thread_select():
    while True:
        result_list = select.select(active_clients,[],[],1)
        if len(result_list[0]) < 1:
            continue
        
        for c in result_list[0]:
            c.read()

def CT(host='localhost',port=8002,auto_add=True):
    c = Client()
    c.connect(host,port)
    if auto_add:
        active_clients.append(c)

    return c


thread= threading.Thread(target=thread_select)
thread.setDaemon(True)
thread.start()


if __name__ == '__main__':
    accountId = int(sys.argv[1])
    c = CT()
    c.login(accountId)
