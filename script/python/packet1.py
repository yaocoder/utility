__author__ = 'yw'
# -*- coding: utf-8 -*

#copytree拷贝指定源目录下所有文件到指定目的目录，delete_extension方法删除该目录下指定类型文件

import os
import time
from shutil import *

def copytree(src, dst, symlinks=False, ignore=None):
    names = os.listdir(src)
    if ignore is not None:
        ignored_names = ignore(src, names)
    else:
        ignored_names = set()

    if not os.path.isdir(dst):   # This one line does the trick
        os.makedirs(dst)
    errors = []
    for name in names:
        if name in ignored_names:
            continue
        src_name = os.path.join(src, name)
        dst_name = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(src_name):
                link_to = os.readlink(src_name)
                os.symlink(link_to, dst_name)
            elif os.path.isdir(src_name):
                copytree(src_name, dst_name, symlinks, ignore)
            else:
                # Will raise a SpecialFileError for unsupported file types
                copy2(src_name, dst_name)
        # catch the Error from the recursive copytree so that we can
        # continue with other files
        except Error, err:
            errors.extend(err.args[0])
        except EnvironmentError, why:
            errors.append((src_name, dst_name, str(why)))
    try:
        copystat(src, dst)
    except OSError, why:
        if WindowsError is not None and isinstance(why, WindowsError):
            # Copying file access times may fail on Windows
            pass
        else:
            errors.extend((src, dst, str(why)))
    if errors:
        raise Error, errors

def delete_extension(path,extension_list):
    for extension in extension_list:
        length=len(extension)
        for root ,dirs, files in os.walk(path):
            for file_path in files:
                if file_path[-length:]==extension:
                    os.remove(os.path.join(root,file_path))
                    print file_path+" has deleted"

if __name__ == '__main__':
    src = 'd:/src'
    str_time = time.strftime('%Y%m%d',time.localtime(time.time()))
    dst = 'd:/dest-' + str_time
    copytree(src, dst)
    extension_list = ['pdb', 'exp', 'ilk', 'lib']
    delete_extension(dst, extension_list)