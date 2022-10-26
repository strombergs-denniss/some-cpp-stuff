

function generateCube()
    local node = createNode("Cube")
    addComponent(node, Transform.new())

    local positions = {}
    local normals = {}
    local uvs = {}

    

    -- local meshReference = MeshReference.new()
    -- addComponent(node, meshReference)

    return node
end
