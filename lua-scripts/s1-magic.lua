function main()
  if global.wizard_again == 0 then
    dink.preload_seq(561)
    dink.preload_seq(563)
    dink.preload_seq(567)
    dink.preload_seq(569)
    dink.preload_seq(167)
    
    local scrap = dink.create_sprite(78, 319, brain.NONE, 563, 1)
    scrap.base_walk = 560
    scrap.script = "s1-wiz"
    global.wizard_again = 1
  end
end

