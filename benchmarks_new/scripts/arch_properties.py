from subprocess import check_output
import subprocess


def get_cores_count():  # returns number of sockets of target architecture
    output = subprocess.check_output(["lscpu"])
    cores = -1
    for item in output.decode().split("\n"):
        if "Core(s) per socket:" in item:
            cores_line = item.strip()
            cores = int(cores_line.split(":")[1])
    if cores == -1:
        raise NameError('Can not detect number of cores of target architecture')
    return cores


def get_sockets_count():  # returns number of sockets of target architecture
    output = subprocess.check_output(["lscpu"])
    cores = -1
    for item in output.decode().split("\n"):
        if "Socket(s)" in item:
            sockets_line = item.strip()
            sockets = int(sockets_line.split(":")[1])
    if sockets == -1:
        raise NameError('Can not detect number of cores of target architecture')
    return sockets


def get_threads_count():
    return get_sockets_count()*get_cores_count()


def get_arch():  # returns architecture, eigher kunpeng920 or intel_xeon
    architecture = "unknown"
    output = subprocess.check_output(["lscpu"])
    for item in output.decode().split("\n"):
        if "Architecture:" in item:
            arch_line = item.strip()
            arch_str = arch_line.split(":")[1]
            if "aarch64" in arch_str:
                architecture = "kunpeng920"
            if "x86_64" in arch_str:
                architecture = "intel_xeon"
    return architecture
