

-- Useful to position Particle FX such as rain , snow at the camera location.

function OnCreate(self)
	
	self.offset = 150;
	
end

function OnThink(self)

  -- update Particle Position according to Camera Position
  local cam = Game:GetCamera()
  
  local camPos = cam:GetPosition()
  
  -- height offset
  camPos = camPos + cam:GetDirection() * self.offset - Vision.hkvVec3(0, 0, 100)
  
  self:SetPosition(camPos)
    
end
