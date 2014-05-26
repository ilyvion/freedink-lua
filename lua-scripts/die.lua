-- generic script for killing things so they don't come back

function die()
  local hold = current_sprite.editor_sprite
  if hold ~= nil then
    hold.type = editor_type.KILL_COMPLETELY
  end
end

