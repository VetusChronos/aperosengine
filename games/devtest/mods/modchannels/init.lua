--
-- Mod channels experimental handlers
--
local mod_channel = aperosengine.mod_channel_join("experimental_preview")

aperosengine.register_on_modchannel_message(function(channel, sender, message)
	aperosengine.log("action", "[modchannels] Server received message `" .. message
			.. "` on channel `" .. channel .. "` from sender `" .. sender .. "`")

	if mod_channel:is_writeable() then
		mod_channel:send_all("experimental answers to preview")
		mod_channel:leave()
	end
end)
