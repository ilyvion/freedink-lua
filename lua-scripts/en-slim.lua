--slime brain

local enemy = include("_enemy")

function main()
  -- TODO: Possibly move into _enemy.lua.
  current_sprite.brain = brain.MONSTER_DIAGONAL
  current_sprite.speed = 1
  current_sprite.exp = 8
  current_sprite.timing = 20
  current_sprite.hitpoints = 15
  current_sprite.touch_damage = 6
  dink.preload_seq(651)
  dink.preload_seq(653)
  dink.preload_seq(661)
  dink.preload_seq(663)
  current_sprite.base_attack = -1
  current_sprite.base_walk = 690
  current_sprite.base_death = 700
  
  if dink.random(2, 1) == 1 then
    current_sprite.target = player
  end
end

function touch()
  -- slurp noise
  dink.playsound(38, 18050, 6000, nil, false)
end

function hit(enemy_sprite)
  current_sprite.target = enemy_sprite
  --lock on to the guy who just hit us
  dink.playsound(38, 38050, 6000, player, false)
end

function die()
  enemy.kill_enemy(current_sprite, editor_type.KILL_RETURN_AFTER_FIVE_MINUTES, "small")
end

