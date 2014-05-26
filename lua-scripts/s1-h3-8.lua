function talk()
  local maybe = dink.random(3, 1)
  if maybe == 1 then
    player:say("Libby's bed, ahh yeah.")
  elseif maybe == 2 then
    player:say("Libby's bed, to dream the impossible dream...")
  elseif maybe == 3 then
    player:say("Libby's bed, looks lonely.")
  end
end

function hit()
  player:say("Get ready for me!")
end

