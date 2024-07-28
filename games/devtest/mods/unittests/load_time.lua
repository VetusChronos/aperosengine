-- Test item (un)registration and overriding
do
	local itemname = "unittests:test_override_item"
	aperosengine.register_craftitem(":" .. itemname, {description = "foo"})
	assert(assert(aperosengine.registered_items[itemname]).description == "foo")
	aperosengine.override_item(itemname, {description = "bar"})
	assert(assert(aperosengine.registered_items[itemname]).description == "bar")
	aperosengine.override_item(itemname, {}, {"description"})
	-- description has the empty string as a default
	assert(assert(aperosengine.registered_items[itemname]).description == "")
	aperosengine.unregister_item("unittests:test_override_item")
	assert(aperosengine.registered_items["unittests:test_override_item"] == nil)
end
