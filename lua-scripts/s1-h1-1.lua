
function talk()
  if global.story >= 3 then
    player:say("My bed is going to burn up!")
    return
  end
  player:say("This is my bed.  So clean and neat!  Mom must have made it.")
end

function hit()
  if global.story >= 3 then
    player:say("It's hot!")
    return
  end
  player:say("Die, stupid bed!")
end

