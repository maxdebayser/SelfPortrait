require "libluaselfportrait"
require "cxxtest"

function testVariant()

    local v1 = Variant.new(3)
    TS_ASSERT [[ v1:tonumber() == 3 ]]


    local v2 = Variant.new(5)
    TS_ASSERT [[ v2:tonumber() == 5 ]]

    local n11 = v1 +  1
    local n12 = 1  + v1

    local n21 = v1 + v2

    TS_ASSERT [[ type(n11) == 'number' ]]
    TS_ASSERT [[ n11 == 4 ]]

    TS_ASSERT [[ type(n12) == 'number' ]]
    TS_ASSERT [[ n12 == 4 ]]

    TS_ASSERT [[ type(n21) == 'number' ]]
    TS_ASSERT [[ n21 == 8 ]]

    return false
end
