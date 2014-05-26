function main()
  dink.preload_seq(452)
end

function touch()
  if global.life < 1 then return end
  
  player:freeze()
  player.x = 274
  player.y = 195
  player.seq = 452
  player.frame = 1
  player.nocontrol = true --dink can't move until anim is done!
  current_sprite.touch_damage = 0
  player.brain = brain.NONE
  dink.wait(2000)
  player.brain = brain.PLAYER
  global.player_map = 131
  player.x = 289
  player.y = 377
  dink.load_screen(131)
  dink.draw_screen()
end

