import os

proj_dir = os.path.dirname(os.path.realpath(__file__)) + "/.."
home = os.environ["USERPROFILE"]
pio_path = home + "/.platformio"
pio = pio_path + "/penv/Scripts/platformio.exe"
toolchain = pio_path + "/packages/toolchain-xtensa32/bin/xtensa-esp32-elf-"
firmware = "./build/antennatracker/firmware.elf"
pio_conf = proj_dir + "/platformio.ini"


pioContent = [line.strip() for line in open(pio_conf, "r")]
pioContent = [val for val in pioContent if ("[" not in val) and ("=" in val)]
pioContent = [val.split("=") for val in pioContent]

for val in pioContent:
    if len(val) < 2:
        continue
    val = [x.strip() for x in val]
    if val[0] == "upload_port":
        upload_port = val[1]
    elif val[0] == "monitor_speed":
        monitor_speed = val[1]