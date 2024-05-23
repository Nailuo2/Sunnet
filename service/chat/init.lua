local serviceId
local conns = {}

--服务启动
function OnInit(id)
    serviceId = id
    print("[lua] chat OnInit id:"..id)
    Sunnet.Listen(8002, id)
end

--有新连接
function OnAcceptMsg(listenfd, clientfd)
    print("[lua] chat OnAcceptMsg "..clientfd)
    conns[clientfd] = true
end

--收到网络数据  向各个客户端转发消息
function OnSocketData(fd, buff)
    print("[lua] chat OnSocketData "..fd)
    for fd, _ in pairs(conns) do
        Sunnet.Write(fd, buff)
    end
end

--连接断开
function OnSocketClose(fd)
    print("[lua] chat OnSocketClose "..fd)
    conns[fd] = nil
end