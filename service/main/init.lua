print("run lua init.lua")

function OnInit(id)
    Sunnet.NewService("chat")
end

function OnExit()
    print("[lua] main OnExit")
end