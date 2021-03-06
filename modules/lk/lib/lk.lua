--[[------------------------------------------------------

  lk
  -----

  This file is loaded just after the load paths have been
  changed and loads Lubyk essentials.

--]]------------------------------------------------------
-- =========  SETUP LOAD PATHS ===========================
-- These paths already come loaded with lubyk lib path
-- through the lubyk loader "require 'lubyk'"
local lua_paths  = package.path
local lua_cpaths = package.cpath

-- 1. local lib (foo.lua or lib/foo.lua)
package.path  = './?.lua;./lib/?.lua;'
package.cpath = './?.so;./lib/?.so;'

-- 2. user libs
for _, path in ipairs(Lubyk.paths or {}) do
  package.path = package.path .. path .. '/?.lua;'
end

for _, cpath in ipairs(Lubyk.cpaths or {}) do
  package.cpath = package.cpath .. cpath .. '/?.so;'
end
-- 3. Installed lubyk libs and lua paths
package.path  = package.path  .. lua_paths
package.cpath = package.cpath .. lua_cpaths

-- =========  LOAD MINIMAL LIBS ==========================
require 'Autoload'
lk    = Autoload('lk')
-- Load defaults in 'Lubyk'
require 'lk.Lubyk'

lubyk = Autoload('lubyk')
-- autoload stuff in _G
Autoload.global()

-------------------------------- CONSTANTS
lubyk.service_type    = Lubyk.service_type

-- Get content at path (used with lk.Morph)
lubyk.get_url         = '/lk/get'
-- Get patch basic information
lubyk.info_url        = '/lk/info'
-- Get all patch information at once
lubyk.dump_url        = '/lk/dump'
-- (Partial) update of a patch
lubyk.update_url      = '/lk/update'
-- Execute method (actions depend on the specific service)
lubyk.execute_url     = '/lk/do' -- action, params
-- Quit process
lubyk.quit_url        = '/lk/quit'

-- Notification for new service (in-process)
lubyk.add_service_url = '/lk/add_service'
-- Notification for removed service (in-process)
lubyk.rem_service_url = '/lk/rem_service'

-------------------------------- lk.core
require 'lk.core'
local time_ref = lk.TimeRef()
function elapsed()
  return time_ref:elapsed()
end
now = elapsed

require 'lk.Scheduler'
sched = lk.Scheduler()
function sleep(ms)
  sched:sleep(ms)
end

function run(...)
  sched:run(...)
end

require 'lk.util'

