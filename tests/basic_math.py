import subprocess
pankti_exe = "../zig-out/bin/pankti"
_example = "../sample/0.pank"

print(subprocess.check_output([pankti_exe , _example]))
