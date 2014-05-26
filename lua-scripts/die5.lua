-- generic script for killing things so they don't come back

function main()
  current_sprite.hitpoints = 1
end

function die()
  local hold = current_sprite.editor_sprite
  if hold ~= nil then
    hold.type = editor_type.KILL_RETURN_AFTER_FIVE_MINUTES
  end
end

