local function printFunctions(tbl, prefix)
  for k, v in pairs(tbl) do
    if type(v) == "function" then
      print(prefix .. k)
    elseif type(v) == "table" then
      printFunctions(v, prefix .. k .. ".")
    end
  end
  -- Check metatables
  local mt = getmetatable(tbl)
  if mt then
    printFunctions(mt, prefix .. "__index.")
  end
end

printFunctions(debug.getregistry(), "")
