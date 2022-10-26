print("Main LUA")

for a = 0, 9 do
    local node = createNode("Node" .. a)

    if a % 2 == 0 then
        local transform = Transform.new()
        transform.translation = Vec3.new(a, a, a)
        transform.rotation = Vec3.new(a, a, a)
        transform.scale = Vec3.new(a, a, a)

        addComponent(node, transform)
    end

    print(node)
end
