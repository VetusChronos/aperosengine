
local message = function(msg)
	aperosengine.log("action", "[callbacks] "..msg)
	aperosengine.chat_send_all(msg)
end

core.register_on_punchplayer(function(player, hitter, time_from_last_punch, tool_capabilities, dir, damage)
	if not hitter then
		message("Player "..player:get_player_name().." punched without hitter.")
	end
end)
