Import("env")
import os
import shutil


def before_upload(source, target, env):
    # do some actions
    bin_path = source[0].get_abspath()
    # copy file to disk
    print("Copying firmware to Qspi flash")
    shutil.copy(bin_path, "E:/fw.bin")
    print("Done copying firmware to Qspi flash")

def after_upload(source, target, env):
    print("after_upload")
    # do some actions
    # remove firmware.bin
    os.remove(source[0].get_abspath())

env.AddPreAction("upload", before_upload)
env.AddPostAction("upload", after_upload)