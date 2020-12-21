import subprocess

revision = (
        subprocess.check_output(["git", "rev-parse", "HEAD"])
            .strip()
            .decode("utf-8")
)
print(f'-D RADIO_CONFIG_PACKET_VERSION_STR=\\"{revision}\\"')

