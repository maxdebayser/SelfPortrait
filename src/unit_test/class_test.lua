package.cpath = package.cpath..";../lua_module/?.so"
require "libluacppreflect"

function testClass()
	Test1Class = Class.lookup("Test::Test1")

	if (Test1Class == nil) then return false end

	if (Test1Class:simpleName() ~= "Test1") then return false end
	if (Test1Class:fullyQualifiedName() ~= "Test::Test1") then return false end

	Superclasses = Test1Class:superclasses()
	if (#Superclasses ~= 2) then return false end

	sups = {}
	for _, v in ipairs(Superclasses) do sups[v:simpleName()] = v end

	if (sups["TestBase1"] == nil) then return false end;
	if (sups["TestBase2"] == nil) then return false end;

	Base1 = sups["TestBase1"]
	Base1Cons = Base1:constructors()
	if (Base1Cons == nil or #Base1Cons ~= 1) then return false end

	Base2Cons = sups["TestBase2"]:constructors()
	if (Base2Cons == nil or #Base2Cons ~= 1) then return false end

	Base2Constructor = Base2Cons[1]

	return true
end
