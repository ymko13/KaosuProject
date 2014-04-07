function OnThink(self)
 
  -- rotate object over time
  local t = Timer:GetTimeDiff() * 50
  local newPos = Vision.hkvVec3(t, 0, 0)

  self:IncOrientation(newPos)
end
 