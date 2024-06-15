import os

if __name__ == '__main__':
    retcode = os.system('cog --check CMakeLists.txt')
    if retcode != 0:
        os.system('cog -r CMakeLists.txt')
