# Building: wordclock70.d1r2.lic.D4
Import("env")

env.Replace(PROGNAME="wordclock%s.%s.%s.%s" % (env.GetProjectOption("custom_version"), env.GetProjectOption("custom_hardware"), env.GetProjectOption("custom_license"), env.GetProjectOption("custom_pin")))

