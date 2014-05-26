function main()
  dink.playmidi("insper.mid")
  local mbot = dink.random(7, 1)
  if mbot == 1 then
    player:say_stop("Smells damp in here.")
  elseif mbot == 2 then
    player:say_stop("I don't like this one bit...")
  end
  noise()
  
  dink.set_callback_random("noise", 5000, 10000)
end

function noise()
  if global.wizard_see > 2 then
    -- killed monster already, why would it roar?
    return
  end
  
  dink.playsound(32, 11000, 4000, nil, false)
  dink.wait(4000)
  local bot = dink.random(4, 1)
  if bot == 1 then
    player:say_stop("Can we leave now? <shiver>")
  elseif bot == 2 then
    player:say_stop("I'm scared.")
  elseif bot == 3 then
    player:say_stop("Why, that's a funny noise.")
  elseif bot == 4 then
    player:say_stop("This cave sucks, let's leave...")
  end
end

