local modname = aperosengine.get_current_modname()
local prefix = "["..modname.."] "

-- Startup info
aperosengine.log("action", prefix.."modname="..dump(modname))
aperosengine.log("action", prefix.."modpath="..dump(aperosengine.get_modpath(modname)))
aperosengine.log("action", prefix.."worldpath="..dump(aperosengine.get_worldpath()))

-- Callback info
aperosengine.register_on_mods_loaded(function()
	aperosengine.log("action", prefix.."Callback: on_mods_loaded()")
end)

aperosengine.register_on_chatcommand(function(name, command, params)
	aperosengine.log("action", prefix.."Caught command '"..command.."', issued by '"..name.."'. Parameters: '"..params.."'")
end)
