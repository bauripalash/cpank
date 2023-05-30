import os
import subprocess

CPANK_ENV = "CPANK_EXE"
CPANK_DEBUG = "CPANK_DEBUG"
CPANK_EXE_DEFAULT_PATH = "zig-out/bin/pankti"


class CpankHandler:
    cpank_exe: str = ""
    is_debug: bool = False

    def __init__(self) -> None:
        exe_path_env = os.getenv(CPANK_ENV)
        if exe_path_env == None:
            self.cpank_exe = CPANK_EXE_DEFAULT_PATH
        else:
            self.cpank_exe = exe_path_env

        cpank_debug = os.getenv(CPANK_DEBUG)

        if cpank_debug != None and (
            cpank_debug == "true" or cpank_debug == "1" or cpank_debug == "t"
        ):
            self.is_debug = True


    def _get_build_command(self, input: str) -> str:
        return 'echo "' + input + '" | ' + self.cpank_exe + " - "

    def get(self, input: str) -> str:
        p = subprocess.Popen(
            self._get_build_command(input), stdout=subprocess.PIPE, shell=True
        )
        (out, err) = p.communicate()
        p_stat = p.wait()

        _ = err
        _ = p_stat

        result = out.decode()

        if not self.is_debug:
            return result
        else:
            return result.strip("~~p ")
