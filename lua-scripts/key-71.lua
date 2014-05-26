local godmodectrl
local oldscreen
local weakening
local overheadring

function main()
  dink.script_attach(1000)
  godmodectrl = global.player_map
  oldscreen = 0
  weakening = 0
  
  if global.defense > 899 then
    global.defense = global.defense - 900
    global.magic = global.magic - 900
    global.life = global.life - 1000
    global.lifemax = global.lifemax - 1000
    player.nohit = false
    dink.wait(10)
    dink.kill_this_task()
  end
  
  dink.init([[load_sequence_now graphics\godmd- 456 66 11 90 0 0 0 0]])
  player:say("I'm God now!")
  global.defense = global.defense + 900
  global.magic = global.magic + 900
  global.life = global.life + 1000
  global.lifemax = global.lifemax + 1000
  
  while true do
    player.nohit = true
    global.life = global.lifemax
    godmodectrl = global.player_map
    if godmodectrl ~= oldscreen then
      overheadring = dink.create_sprite(player.x, player.y, brain.SHADOW, 456, 1)
      overheadring.que = 1000
      overheadring.brain = brain.SHADOW
      overheadring.seq = 456
      overheadring.brain_parm = 1
      overheadring.size = 100
      overheadring.nohit = true
      oldscreen = global.player_map
    end
    dink.wait(90)
    weakening = dink.get_sprite_with_this_brain(brain.MONSTER_DIAGONAL, player)
    if weakening ~= nil then
      weakening = dink.get_rand_sprite_with_this_brain(brain.MONSTER_DIAGONAL, player)
      weakening.touch_damage = 0
      weakening.strength = 0
    end
    weakening = dink.get_sprite_with_this_brain(brain.MONSTER_NONDIAGONAL, player)
    if weakening ~= nil then
      weakening = dink.get_rand_sprite_with_this_brain(brain.MONSTER_NONDIAGONAL, player)
      weakening.touch_damage = 0
      weakening.strength = 0
    end
    if global.defense < 900 then
      player:say("I'm vulnerable again!")
      overheadring.brain = brain.BRAIN_PARM_SIZE_MATCH
      overheadring.brain_parm = 10
      dink.wait(100)
      dink.init([[load_sequence_now graphics\inter\arrow\arowl- 456 100]])
      dink.wait(10)
      dink.kill_this_task()
    end
  end
end

