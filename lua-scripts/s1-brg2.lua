local dumb

function main()
  dumb = dink.create_sprite(360, 300, brain.NONE, 64, 1)
  dumb.hard = false
  dumb:draw_hard()
  dumb.disabled = true
  
  current_sprite.base_walk = 370
  current_sprite.speed = 3
  current_sprite.timing = 0
end

function hit()
  current_sprite:say("`3Did something touch me?  A fly perhaps?")
end
  
local choice_menu = dink.create_choice_menu()
choice_menu.y = 240
choice_menu.title_color = 3
choice_menu.title = [["Hello good sir, the bridge toll is 100 gold."]]

local choice_pay = choice_menu:add_choice("Pay the toll")
local choice_argue = choice_menu:add_choice("Argue")
local choice_leave = choice_menu:add_choice("Leave")

function talk()
  player:freeze()
  
  local choice_result = choice_menu:show()
  
  if choice_result == choice_pay then
    dink.wait(500)
    if global.gold < 100 then
      current_sprite:say_stop("`3You don't have enough gold, fool!")
      player:unfreeze()
      return
    else
      current_sprite:say_stop("`3Thanks.  Have a nice day.")
      global.gold = global.gold - 100
      current_sprite:move_stop(direction.WEST, -50, true)
      global.story = 7
      player:unfreeze()
      dumb.hard = 1
      dink.force_vision(0)
      dink.kill_this_task()
      return
    end
  elseif choice_result == choice_argue then
    cutscene.quick_cutscene({d = player, c = current_sprite}, {
      {"w", 500},
      {"ac", "c", 3},
      {"ass", {"d", "c"}, {
        "This is ridiculous.  How can you justify charging this much?",
        "My kids need to eat.",
        "Does the king know about your little 'business'?",
        "Of course not.",
        "Ah, how nice."
      }},
      {"ufz", "d"}
    })
  end
end

