--[[------------------------------------------------------

  Value
  -----

  Stores value in 'value'.

--]]------------------------------------------------------
inlet('print', 'Print/dump incoming value [any].')

function inlet.print(v)
  if type(v) == 'table' then
    print(yaml.dump(v))
  else
    print(v)
  end
end
