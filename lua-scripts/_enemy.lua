--[[
Common enemy code. In here goes code that lots of enemies share.
--]]

-- We'll make use of the emake functions, so let's import them.
local emake = include("_emake")

function kill_enemy(enemy_sprite, editor_type_value, drop_item)
  local hold = enemy_sprite.editor_sprite
  if hold ~= nil then
    hold.type = editor_type_value
  end
  
  if drop_item ~= nil then
    --[[
      At this point, remember (or learn) that table.entry is really just
      syntactic sugar for table["entry"], which makes this equivalent to
      calling the function named whatever make_item is set to in the
      _emake.lua file, e.g. emake.small(x, y).
    --]]
    emake[drop_item](enemy_sprite.x, enemy_sprite.y)
  end
end
