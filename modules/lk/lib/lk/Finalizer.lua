--[[------------------------------------------------------

  lk.Finalizer
  ------------

  This object executes the provided method when it is
  garbage collected. Be carefull how you use this tool or
  you will get burnt (= only use it if you have no other
  way of doing what you need to do). The main advice is
  "only call and use objects that would not die with the
  finalizer".

  Usage: lk.Finalizer(func)

--]]------------------------------------------------------
require 'lk.Finalizer_core'

