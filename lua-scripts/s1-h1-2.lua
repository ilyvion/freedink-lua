
function talk()
  if global.story > 3 then
    player:say_stop("This may have been what started that accident.")
    player:say("But it really doesn't matter now.")
    return
  elseif story == 3 then
    player:say("Maybe this got out of control.")
    return
  end
  player:say("Ahh, fire.  Warm.")
end

function hit()
  if global.story > 3 then
    player:say("Stupid fireplace!")
    return
  end
  player:say("Ouch, that is hot!")
end

