__author__ = 'yw'
# -*- coding: utf-8 -*

#copyFiles拷贝指定源目录下所有文件(除去svn文件）到指定目的目录，delete_extension方法删除该目录下指定类型文件

import os

def copyFiles(sourceDir,  targetDir):
    if sourceDir.find(".svn") > 0:
        return
    for file in os.listdir(sourceDir):
        sourceFile = os.path.join(sourceDir,  file)
        targetFile = os.path.join(targetDir,  file)
        if os.path.isfile(sourceFile):
            if not os.path.exists(targetDir):
                os.makedirs(targetDir)
            if not os.path.exists(targetFile) or(os.path.exists(targetFile) and (os.path.getsize(targetFile) != os.path.getsize(sourceFile))):
                    open(targetFile, "wb").write(open(sourceFile, "rb").read())
        if os.path.isdir(sourceFile):
            First_Directory = False
            copyFiles(sourceFile, targetFile)

def delete_extension(path,extension_list):
    for extension in extension_list:
        length=len(extension)
        for root ,dirs, files in os.walk(path):
            for file_path in files:
                if file_path[-length:]==extension:
                    os.remove(os.path.join(root,file_path))
                    print file_path+" has deleted"

if  __name__ =="__main__":
    copyFiles("d:\src", "d:\dest3")
    extension_list = ['bat']
    delete_extension("d:\dest3", extension_list)