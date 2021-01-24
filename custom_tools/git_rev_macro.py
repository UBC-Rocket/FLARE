import subprocess
import sys

subprocess.run(["git","submodule", "update", "--init", "--recursive"])

revision = (
        subprocess.check_output(["git", "rev-parse", "HEAD"])
            .strip()
            .decode("utf-8")
)
# Convention: CMake calls this with a command line argument; PlatformIO doesn't.
if len(sys.argv) == 1: # always has file name
    print(f'-D RADIO_CONFIG_PACKET_VERSION_STR=\\"{revision}\\"')
else:
    print(f'{revision}', end='')

