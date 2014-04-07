-- new script file
currentTask = "";
function OnCreate(self)
  Debug:PrintLine("Object Created "..self:GetProperty("currentTask"))
end

function OnBeforeSceneLoaded(self)
end

function OnAfterSceneLoaded(self)
  Debug:PrintLine("Scene Loaded "..self:GetProperty("currentTask"))
  local ob = Game:GetEntity("Player")
  local pos = ob:GetPosition()
  Debug:PrintLine("Pos: " .. pos.x .. " " .. pos.y)
  Console:SetVisible(true)
end

function OnUpdateSceneBegin(self)
end

function OnInitProperties(self)
  local seed = 1001;
  self:SetProperty("currentTask"," Seed->"..seed)
  self:SetProperty("seed",seed)
  self:SetProperty("normal_textures",4)
  self:SetProperty("object_meshes",3)
  self:SetProperty("total_rooms",3)
  self:SetProperty("room_min",8)
  self:SetProperty("room_max",15)
  self:SetProperty("outer_ring",2)
  
  self:SetProperty("vMeshSize",100.8)
  self:SetProperty("xoff",11)
  self:SetProperty("yoff",11)
end
