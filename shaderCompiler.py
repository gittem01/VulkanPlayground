import pathlib
import os
import time
from sys import platform, exit

pathOfFile = os.path.dirname(os.path.abspath(__file__))

if platform == "linux" or platform == "linux2":
    compilerDir = "/usr/bin"
    compilerExe = compilerDir + "/glslc"
elif platform == "win32":
    compilerDir = "C:/VulkanSDK/1.2.189.2/Bin"
    compilerExe = compilerDir + "/glslc.exe"
else:
    exit("only tested for linux and windows")

shaderFolders = ["shaders"] # folders to check for shader files

fileMap = {}

def getChangedFiles():
    global fileMap, shaderFolders

    changedFiles = []
    for shaderFolder in shaderFolders:
        l = os.walk(pathOfFile + "/" + shaderFolder)
        
        for files in l:
            for file in files[2]:
                if file.split(".")[-1] == "spv":
                    continue
                filePath = files[0] + "/" + file
                fileN = pathlib.Path(filePath)
                if filePath not in fileMap:
                    changedFiles.append(filePath)
                else:
                    if (fileN.stat().st_mtime != fileMap[filePath]):
                        changedFiles.append(filePath)
                fileMap[filePath] = fileN.stat().st_mtime

    return changedFiles

def getFolder(fullLocation):
    return "/".join(fullLocation.split("/")[:-1])

def getName(fullLocation):
    return "".join(fullLocation.split("/")[-1].split(".")[:-1])

def compileShaders(shaderFiles):
    for shaderFile in shaderFiles:
        commandString = compilerExe + " " + shaderFile + " -o " + shaderFile + ".spv"
        print(commandString)
        os.system(commandString)

while 1:
    changedFiles = getChangedFiles()
    if (len(changedFiles) > 0):
        print(len(changedFiles), "Shader file changed. Compling now...\n")
        compileShaders(changedFiles)
        print("\nCompilation done\n")

    time.sleep(0.1)