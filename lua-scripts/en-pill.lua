-- pillbug brain

--local misc = include("_misc")
local enemy = include("_enemy")

function main()
  -- TODO: Possibly move into _enemy.lua.
  current_sprite.brain = brain.MONSTER_DIAGONAL
  current_sprite.speed = 1
  current_sprite.exp = 5
  current_sprite.base_walk = 130
  current_sprite.base_death = 140
  current_sprite.touch_damage = 1
  current_sprite.hitpoints = 8
  dink.preload_seq(131)
  dink.preload_seq(133)
  dink.preload_seq(141)
  dink.preload_seq(143)
  
  if dink.random(2, 1) == 1 then
    current_sprite.target = player
  end
end

function hit(enemy_sprite)
  current_sprite.target = enemy_sprite
  dink.playsound(30, 21050, 4000, current_sprite, false)
end

function die()
  enemy.kill_enemy(current_sprite, editor_type.KILL_RETURN_AFTER_FIVE_MINUTES, "small")
end

