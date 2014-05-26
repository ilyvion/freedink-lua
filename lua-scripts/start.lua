--this is run when dink is loaded, directly after the dink.ini file
--is processed.

local load_sound = dink.load_sound
local create_sprite = dink.create_sprite

function main()
  --dink.playavi("anim\rtlogo.avi")
  
  dink.debug("Loading sounds..")
  load_sound("QUACK.WAV", 1)
  load_sound("PIG1.WAV", 2)
  load_sound("PIG2.WAV", 3)
  load_sound("PIG3.WAV", 4)
  load_sound("PIG4.WAV", 5)
  load_sound("BURN.WAV", 6)
  load_sound("OPEN.WAV", 7)
  load_sound("SWING.WAV", 8)
  load_sound("PUNCH.WAV", 9)
  load_sound("SWORD2.WAV", 10)
  load_sound("SELECT.WAV", 11)
  load_sound("WSCREAM.WAV", 12)
  load_sound("PICKER.WAV", 13)
  load_sound("GOLD.WAV", 14)
  load_sound("GRUNT1.WAV", 15)
  load_sound("GRUNT2.WAV", 16)
  load_sound("SEL1.WAV", 17)
  load_sound("ESCAPE.WAV", 18)
  load_sound("NONO.WAV", 19)
  load_sound("SEL2.WAV", 20)
  load_sound("SEL3.WAV", 21)
  load_sound("HIGH2.WAV", 22)
  load_sound("FIRE.WAV", 23)
  load_sound("SPELL1.WAV", 24)
  load_sound("CAVEENT.WAV", 25)
  load_sound("SNARL1.WAV", 26)
  load_sound("SNARL2.WAV", 27)
  load_sound("SNARL3.WAV", 28)
  load_sound("HURT1.WAV", 29)
  load_sound("HURT2.WAV", 30)
  load_sound("ATTACK1.WAV", 31)
  load_sound("CAVEENT.WAV", 32)
  load_sound("LEVEL.WAV", 33)
  load_sound("SAVE.WAV", 34)
  load_sound("SPLASH.WAV", 35)
  load_sound("SWORD1.WAV", 36)
  load_sound("BHIT.WAV", 37)
  load_sound("SQUISH.WAV", 38)
  load_sound("STAIRS.WAV", 39)
  load_sound("STEPS.WAV", 40)
  load_sound("ARROW.WAV", 41)
  load_sound("FLYBY.WAV", 42)
  load_sound("SECRET.WAV", 43)
  load_sound("BOW1.WAV", 44)
  load_sound("KNOCK.WAV", 45)
  load_sound("DRAG1.WAV", 46)
  load_sound("DRAG2.WAV", 47)
  load_sound("AXE.WAV", 48)
  load_sound("BIRD1.WAV", 49)
  
  dink.fill_screen(0)
  
  player.seq = 0
  player.brain = brain.MOUSE
  player.pseq = 10
  player.pframe = 8
  player.que = 20000
  player.noclip = true
  
  local dinklogo = create_sprite(320, 240, brain.NONE, 196, 1)

  local version = dink.get_version()
  if version < 103 then
    --can't play with old .exe, not all command are supported
    dink.say_xy("`4Error - Scripts require version V1.03+ of dink.exe.  Upgrade!", 0, 390)
    dink.wait(1)
    dink.wait(5000)
    dink.kill_game()
    return
  end
  
  local crap = create_sprite(76, 40, brain.BUTTON, 194, 1)
  crap.script = "start-1"
  crap.noclip = true
  crap.touch_damage = -1
  
  crap = create_sprite(524, 40, brain.BUTTON, 195, 1)
  crap.script = "start-2"
  crap.noclip = true
  crap.touch_damage = -1
  
  --crap = create_sprite(104, 440, brain.BUTTON, 192, 1)
  --crap.noclip = true
  --crap.script = "start-3"
  --crap.touch_damage = -1
  
  crap = create_sprite(560, 440, brain.BUTTON, 193, 1)
  crap.noclip = true
  crap.script = "start-4"
  crap.touch_damage = -1
  
  dink.playmidi("1003.mid")
  dink.kill_this_task()
end

