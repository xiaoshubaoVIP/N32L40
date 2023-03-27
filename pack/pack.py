# coding=gb2312
import os
import shutil
import subprocess


keil_path = "E:/Keil_v5"    # keil 软件安装根目录

keil_target = "n32l4"
uvproj = "../mdk/n32l4.uvprojx"
keil_output = "../mdk/Objects/"
config_file = "../code/ap/inc/config.h"

build_path = ""

        #前缀   匹配字符 截取到的字符
file = [["",    "BOARD_", ""],
        ["-",   "SUPPORT_RADIO_FUNCTION", ""],
        ["_",   "STANDARD_", ""],
        ["_",   "CHIP_", ""],
        ["_V",  "VERSION", ""]]

dir = [["",    "BOARD_", ""],
       ["-",   "SUPPORT_RADIO_FUNCTION", ""],
       ["_",   "CHIP_", ""],
       ["_V",  "VERSION", ""]]

class BuildError(RuntimeError):
    def __init__(self, arg):
        self.args = arg

# 计算校验和
def check_sum(path:str):
    sum = 0
    with open(path,"rb") as f:
            while (True):
                b = f.read(1)
                if not b: break
                sum += int.from_bytes(b, byteorder = "big")
    return (hex(sum))

# 生成keil编译文件
def build(std:str):
    uv4 = keil_path + "/UV4/UV4.exe"
    formelf = keil_path + "/ARM/ARMCC/bin/fromelf.exe"
    bin = keil_output + keil_target + ".bin"
    axf = keil_output + keil_target + ".axf"

    subprocess.run([uv4, "-j0", "-cr", uvproj, "-o", "./build.log"])

    with open("../mdk/build.log", "r") as f:
        for l in f:
            if "Target not created." in l: raise BuildError("build")
            if "Program Size" in l: print(l)

    subprocess.run([formelf, "--bin", "-o", bin, axf])

    sum = check_sum(bin)
    target_name = ""
    for f in file:
        if f[2]: target_name += f[0] + f[2]
    target_name += "_" + sum
    shutil.copyfile(keil_output + keil_target + ".hex", build_path + "/" + target_name + ".hex")

    return 

# 解析config参数
def parse_config(str:list):
    for f in file:
        if f[1] in str[1]: f[2] = str[2]
        
    for d in dir:
        if d[1] in str[1]: d[2] = str[2]
    return

# 修改config中的标准
def updateStandard(file, curr, fix):
    file_data = ""
    with open(file, "r", encoding = "gb2312") as f:
        for line in f:
            if "#define" in line:
                if curr in line: 
                    line = line.replace("//", "")
                    parse_config(line.replace("\"", "").split())
                if fix in line: line = "//" + line
                
            file_data += line

    with open(file, "w", encoding = "gb2312") as f:
        f.write(file_data)
    return

def creat(curr:str, fix:str):
    stand = {"UL":"STANDARD_UL2034_UL217", "EN": "STANDARD_EN50291_EN14604"}

    print("build " + curr)
    updateStandard(config_file, stand[curr], stand[fix])
    build(curr)
    return

def main():
    #读取config文件内容
    with open(config_file, "r") as file_object:
        for line in file_object:
            if "/" not in line and "#define" in line: parse_config(line.replace("\"", "").split())

    dir_name = ""
    for d in dir:
        if d[2]: dir_name += d[0] + d[2]

    global build_path
    build_path = "./build/" + dir_name

    #创建文件夹
    shutil.rmtree(build_path, ignore_errors = True)
    os.makedirs(build_path, exist_ok = True)
    print(dir_name + ":")

    #开始生成并打包
    try:
        creat("UL", "EN")
        creat("EN", "UL")
    except BuildError:
        print("build error, check build/build.log")
    else:
        print("all done.")
    finally:
        shutil.copyfile("../mdk/build.log", "./build/build.log")
    return

if __name__ == "__main__":
    main()
