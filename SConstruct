import os, sys

# create parent environment and load tools
globalEnv = Environment()
globalEnv['PARAMETER_SOURCE'] = ['custom.py']
globalEnv.Tool('config_report')
globalEnv.Tool('target_manager')
globalEnv.Tool('crosscompile')
globalEnv.Tool('build_verbose')
globalEnv.Tool('archive_builder')

# move to platform tool
params = Variables(globalEnv['PARAMETER_SOURCE'], ARGUMENTS)
params.Add(EnumVariable('target_platform', 'Target platform', 'auto',
	['auto', 'linux', 'windows', 'beos', 'macos', 'android']))
params.Update(globalEnv)

globalEnv.configReport.add('Target platform', 'target_platform')

targetPlatform = globalEnv['target_platform']
if targetPlatform == 'auto':
	if sys.platform == 'haiku1':
		targetPlatform = 'beos'
	elif os.name == 'win32' or sys.platform == 'win32':
		targetPlatform = 'windows'
	elif sys.platform == 'darwin':
		targetPlatform = 'macos'
	elif os.name == 'posix':
		targetPlatform = 'linux'
	else:
		raise 'Can not auto-detect platform'

globalEnv['TARGET_PLATFORM'] = targetPlatform

# parameters
params = Variables(globalEnv['PARAMETER_SOURCE'], ARGUMENTS)
params.Add(BoolVariable('with_debug', 'Build with debug symbols for GDB usage', False))
params.Add(BoolVariable('with_warnerrors', 'Treat warnings as errors (dev-builds)', False))
params.Add(BoolVariable('with_sanitize', 'Enable sanitizing (dev-builds)', False))
params.Add(BoolVariable('with_optimizations', 'Enable run-time optimizations', True))

params.Update(globalEnv)

# build
SConscript(dirs='cpp', variant_dir='cpp/build', duplicate=0, exports='globalEnv')

# build all target
targets = []
for t in globalEnv.targetManager.targets.values():
	try:
		targets.extend(t.build)
	except:
		pass
globalEnv.targetManager.add('build', globalEnv.targetManager.Target('Build All', globalEnv.Alias('build', targets)))

# install all target
targets = []
for t in globalEnv.targetManager.targets.values():
	try:
		targets.extend(t.install)
	except:
		pass
globalEnv.targetManager.add('install', globalEnv.targetManager.Target('Install All', globalEnv.Alias('install', targets)))

# archive all target
targets = []
for t in globalEnv.targetManager.targets.values():
	try:
		targets.extend(t.archive)
	except:
		pass
globalEnv.targetManager.add('archive', globalEnv.targetManager.Target('Archive All', globalEnv.Alias('archive', targets)))

# by default just build
Default('build')

# finish
globalEnv.targetManager.updateHelp()
print(globalEnv.configReport)
globalEnv.configReport.save()
