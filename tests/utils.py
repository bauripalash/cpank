import subprocess


class CpankHandler:
    cpank_exe : str = ""
    
    def _get_build_command(self , input : str) -> str:
        return "echo -e \"" + input + "\" | " + self.cpank_exe + " - " 
    
    def get(self , input : str) -> str:
        p = subprocess.Popen(self._get_build_command(input) , stdout=subprocess.PIPE , shell=True)
        (out , err) = p.communicate()
        p_stat = p.wait()

        _ = err
        _= p_stat

        return out.decode()
        
