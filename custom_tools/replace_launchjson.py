import json
from jsmin import jsmin

with open('.vscode/launch.json', 'r') as f:
    minified = jsmin(f.read())

    exist = json.loads(minified)

    additionalConfigs = [{
            'name': 'Python: Current File',
            'type': 'python',
            'request': 'launch',
            'program': '${{file}}',
            'console': 'integratedTerminal'
        },
        {
            'name': 'Native Tantalus Stage 1',
            'type': 'cppdbg',
            'request': 'launch',
            'program': '${{workspaceFolder}}/.pio/build/native_tantalus_stage_1/program.exe',
            'args': [],
            'stopAtEntry': True,
            'cwd': '${{workspaceFolder}}',
            'environment': [],
            'externalConsole': True,
            'MIMode': 'gdb',
            'miDebuggerPath': 'C:/Program Files (x86)/mingw-w64/i686-8.1.0-posix-dwarf-rt_v6-rev0/mingw32/bin/gdb.exe',
            'setupCommands': [
                {
                    'description': 'Enable pretty-printing for gdb',
                    'text': '-enable-pretty-printing',
                    'ignoreFailures': True
                }
            ]
        }
    ]

    exist['configurations'] += additionalConfigs

with open('.vscode/launch.json', 'w') as wr:
    json.dump(exist, wr)


