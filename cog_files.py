import os

retcode = os.system('cog --check CMakeLists.txt')
if retcode != 0:
    os.system('cog -r CMakeLists.txt')
