--[[------------------------------------------------------

  Lubyk
  -----

  The role of this file is to alter the load paths and
  prepare for lubyk lib loading. This file also loads user
  settings from ~/.lubyk

--]]------------------------------------------------------
local code = loadfile(string.format('%s/.lubyk', os.getenv('HOME')))
local user_settings = {paths = {}, cpaths = {}}
if code then
  user_settings = code()
end
-- search order:
local lua_paths  = package.path
local lua_cpaths = package.cpath
-- 1. local lib (foo.lua or lib/foo.lua)
package.path  = './?.lua;./lib/?.lua;'
package.cpath = './?.so;./lib/?.so;'
-- 2. user libs
for _, path in ipairs(user_settings.paths) do
  package.path = package.path .. path .. ';'
end
for _, cpath in ipairs(user_settings.cpaths) do
  package.cpath = package.cpath .. cpath .. ';'
end
-- 3. Installed lubyk libs
package.path  = package.path  .. '/usr/local/lib/lubyk/?.lua;'
package.cpath = package.cpath .. '/usr/local/lib/lubyk/?.so;'
-- 4. Installed lua paths
package.path  = package.path  .. lua_paths
package.cpath = package.cpath .. lua_cpaths

require 'lk'
