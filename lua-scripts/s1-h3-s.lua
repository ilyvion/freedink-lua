function main()
  --playmidi("neighbor.mid")
  
  if global.farmer_quest > 1 then
    global.vision = 1
    --so the real warping ladder will appear
  end
  
  if dink.random(3, 1) == 1 then
    player:say("Ah, hello Mr. SmileStein.")
  end
end

