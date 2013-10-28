__author__ = 'yw'
# -*- coding: utf-8 -*

#copytree拷贝指定源目录下指定文件到指定目的目录，格式为jpg png gif

import os
import shutil

def cp_tree_ext(exts, src, dest):
    #Rebuild the director tree like src below dest and copy all files like XXX.exts to dest
    #exts:exetens seperate by blank like "jpg png gif"
    fp={}
    extss=exts.lower().split()
    for dn,dns,fns  in os.walk(src):
        for fl in fns:
            if os.path.splitext(fl.lower())[1][1:] in extss:
                if dn not in fp.keys():
                    fp[dn]=[]
                fp[dn].append(fl)
    for k,v in fp.items():
        relativepath=k[len(src)+1:]
        newpath=os.path.join(dest,relativepath)
        for f in v:
            oldfile=os.path.join(k,f)
            print("Copying ["+oldfile+"] To ["+newpath+"]")
            if not os.path.exists(newpath):
                os.makedirs(newpath)
            shutil.copy(oldfile,newpath)


if  __name__ =="__main__":

	cp_tree_ext("dll exe", "d:\src", "d:\dest")