function talk()
  player:say_stop("Hey, that's an AlkTree.")
  dink.wait(250)
end

function hit()
  if dink.scripts_used() > 170 then
    -- don't make any more nuts, there are 165 already on the screen..)
    return
  end
  
  -- lets make the nut fall down and give it a nut script
  local randx, randy = dink.random(200, 320), dink.random(80, 0)
  local crap = dink.create_sprite(randx, randy, brain.NONE, 421, 23)
  crap.speed = 1
  crap:move_stop(direction.NORTH, 86, true)
  crap.script = "s1-nut"
  crap:move_stop(direction.SOUTH, dink.random(80, 110), true)
end

