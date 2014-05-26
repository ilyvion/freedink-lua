
function talk()
  if global.story >= 3 then
    player:say("Mom's bed is on fire, noooo!")
    return
  end
  player:say("Mom's bed.")
end

function hit()
  if global.story >= 3 then
    player:say("No it's too hot.")
    return
  end
  player:say("Eat this, bed!")
end

