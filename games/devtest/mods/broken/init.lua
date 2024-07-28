-- Register stuff with empty definitions to test if AperosEngine fallback options
-- for these things work properly.

-- The itemstrings are deliberately kept descriptive to keep them easy to
-- recognize.

aperosengine.register_node("broken:node_with_empty_definition", {})
aperosengine.register_tool("broken:tool_with_empty_definition", {})
aperosengine.register_craftitem("broken:craftitem_with_empty_definition", {})

aperosengine.register_entity("broken:entity_with_empty_definition", {})
