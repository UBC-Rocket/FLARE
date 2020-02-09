import json
import platform

try:
    with open('.vscode/launch.json', 'r') as f:
        uncommented = ''
        for line in f:
            if not line.strip().startswith('//'):
                uncommented += line
except FileNotFoundError:
    uncommented = """{
    "version": "0.2.0",
    "configurations": []
}
"""

exist = json.loads(uncommented)

if platform.system() == 'Darwin':
    additionalConfigs = [
        {
            'name': 'Native Tantalus Stage 1 (MacOS)',
            'type': 'lldb',
            'request': 'launch',
            'program': '${workspaceFolder}/.pio/build/native_tantalus_stage_1/program',
            'args': [],
        }
    ]
elif platform.system() == 'Windows':

    filePath = ""

    try:
        with open('custom_tools/UserSpecific/WindowsGDBPath.txt', 'r') as file:
            filePath = file.read()
    except IOError:
        print("Error: File WindowsGDBPath.txt Not Found")
    
    additionalConfigs = [
        {
            'name': 'Native Tantalus Stage 1 (Windows)',
            'type': 'cppdbg',
            'request': 'launch',
            'program': '${workspaceFolder}/.pio/build/native_tantalus_stage_1/program.exe',
            'args': [],
            'stopAtEntry': True,
            'cwd': '${workspaceFolder}',
            'environment': [],
            'externalConsole': True,
            'MIMode': 'gdb',
            'miDebuggerPath': filePath,
            'setupCommands': [
                {
                    'description': 'Enable pretty-printing for gdb',
                    'text': '-enable-pretty-printing',
                    'ignoreFailures': True
                }
            ]
        }
    ]
elif platform.system() == 'Linux':
    additionalConfigs = []

for config in additionalConfigs:
    if config not in exist['configurations']:
        exist['configurations'].append(config)

with open('.vscode/launch.json', 'w') as wr:
    json.dump(exist, wr)
