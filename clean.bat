@echo off
echo Delete all temp files

::================================================
del obj\*.* /q/s
rd  obj /q/s

::================================================
del Bin\*.ilk /q/s
del Bin\*.pdb /q/s

::================================================
del vc_src\ilx2sasp\*.user
del vc_src\ilx2sasp\*.ncb

