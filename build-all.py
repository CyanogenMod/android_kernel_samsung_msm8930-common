#! /usr/bin/env python

# Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Code Aurora nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Build the kernel for all targets using the Android build environment.
#
# TODO: Accept arguments to indicate what to build.

#
# Modify for supporting of the Samsung MSM8930 targets.
#

import glob
from optparse import OptionParser
import subprocess
import os
import os.path
import shutil
import sys
import tarfile

version = 'build-all.py, version 0.01'

build_dir = '../okernel'
make_command = ["zImage", "modules"]
make_env = os.environ
pwd = os.environ.get("PWD")
make_env.update({
        'ARCH': 'arm',
        'CROSS_COMPILE': pwd + '/../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-',
        'KCONFIG_NOTIMESTAMP': 'true' })
all_options = {}

def error(msg):
    sys.stderr.write("error: %s\n" % msg)

def fail(msg):
    """Fail with a user-printed message"""
    error(msg)
    sys.exit(1)

def check_kernel():
    """Ensure that PWD is a kernel directory"""
    if (not os.path.isfile('MAINTAINERS') or
        not os.path.isfile('arch/arm/mach-msm/Kconfig')):
        fail("This doesn't seem to be an MSM kernel dir")

def check_build():
    """Ensure that the build directory is present."""
    if not os.path.isdir(build_dir):
        try:
            os.makedirs(build_dir)
        except OSError as exc:
            if exc.errno == errno.EEXIST:
                pass
            else:
                raise

def update_config(file, str):
    print 'Updating %s with \'%s\'\n' % (file, str)
    defconfig = open(file, 'a')
    defconfig.write(str + '\n')
    defconfig.close()

def scan_configs():
    """Get the full list of defconfigs appropriate for this tree."""
    names = {}
    for n in glob.glob('arch/arm/configs/msm8930_biscotto_[t]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_cratertd_[c]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_baffinvetd_[c]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_express2_[at]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_golden_[astuv]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_higgs_spr_rev03_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_jasper2_vzw_[r]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_ks02_[sk]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_lt02_[asc]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_melius_[acklstuzv]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_melius_eur_*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_serrano_[astuv][a-z][cortw]*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_serrano_eur_*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    for n in glob.glob('arch/arm/configs/msm8930_express_eur_*_defconfig'):
        names[os.path.basename(n)[8:-10]] = n
    return names

class Builder:
    def __init__(self, logname):
        self.logname = logname
        self.fd = open(logname, 'w')

    def run(self, args):
        devnull = open('/dev/null', 'r')
        proc = subprocess.Popen(args, stdin=devnull,
                env=make_env,
                bufsize=0,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT)
        count = 0
        # for line in proc.stdout:
        rawfd = proc.stdout.fileno()
        while True:
            line = os.read(rawfd, 1024)
            if not line:
                break
            self.fd.write(line)
            self.fd.flush()
            if all_options.verbose:
                for i in range(line.count('\n')):
                    count += 1
                    if count == 64:
                        count = 0
                        print
                    sys.stdout.write('.')
                sys.stdout.flush()
            else:
                sys.stdout.write(line)
                sys.stdout.flush()
        print
        result = proc.wait()

        self.fd.close()
        return result

failed_targets = []

def build(target):
    base_defconfig = []
    variant_defconfig = []
    debug_defconfig = ''
    print 'Building %s' % target
    if target[0:12] == 'biscotto_tmo':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:8]
    elif target[0:15] == 'cratertd_chn_3g':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:8]
    elif target[0:17] == 'baffinvetd_chn_3g':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:10]
    elif target[0:15] == 'higgs_spr_rev03':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:5]
    elif target[0:17] == 'jasper2_vzw_rev02':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:7]
    elif target[0:4] == 'ks02':
        base_defconfig = 'msm8930_%s_01_defconfig' % target[0:4]
    elif target[0:8] == 'lt02_chn':
        base_defconfig = 'msm8930_%s-chn_defconfig' % target[0:4]
    elif target[0:6] == 'melius':
        base_defconfig = 'msm8930_%s_01_defconfig' % target[0:6]
    elif target[0:11] == 'serrano_eur':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:7]
    elif target[0:7] == 'serrano':
        base_defconfig = 'msm8930_%s_usa_defconfig' % target[0:7]
    elif target[0:7] == 'express':
        base_defconfig = 'msm8930_%s_defconfig' % target[0:7]
    else:
        base_defconfig = 'msm8930_%s_defconfig' % target[:-4]
    variant_defconfig = 'msm8930_%s_defconfig' % target

    if all_options.debug == 'eng':
        debug_defconfig = '%s_eng_defconfig' % base_defconfig[:-10]
        if target[0:4] == 'ks02':
            debug_defconfig = '%s_eng_defconfig' % base_defconfig[:12]
        elif target[0:6] == 'melius':
            debug_defconfig = '%s_eng_defconfig' % base_defconfig[:14]
        elif base_defconfig[0:19] == 'msm8930_serrano_usa':
            debug_defconfig = '%s_eng_defconfig' % base_defconfig[0:15]
    
    if all_options.specified:
        print 'Targets specified option [%s]' % all_options.specified
        splitp = all_options.specified.index(':')
        base_defconfig = 'msm8930_%s_defconfig' % all_options.specified[:splitp]
        variant_defconfig = 'msm8930_%s_defconfig' % all_options.specified[splitp+1:]
	
    print 'Base defconfig : %s' % base_defconfig
    print 'Variant defcon : %s' % variant_defconfig
    print 'Debug defconfi : %s' % debug_defconfig
    dest_dir = build_dir
    log_name = '%s/log-%s.log' % (build_dir, target)
    zImage_name = '%s/arch/arm/boot/zImage' % (dest_dir)
    copy_zImage_name = '%s/zImage' % (build_dir)

    print 'Building %s in %s log %s' % (target, dest_dir, log_name)
    if not os.path.isdir(dest_dir):
        os.mkdir(dest_dir)
    defconfig = 'arch/arm/configs/%s' % base_defconfig
    dotconfig = '%s/.config' % dest_dir
    savedefconfig = '%s/defconfig' % dest_dir
    shutil.copyfile(defconfig, dotconfig)

    devnull = open('/dev/null', 'r')
    subprocess.check_call(['make', 'O=%s' % dest_dir,
        'VARIANT_DEFCONFIG=%s' % variant_defconfig,
        'DEBUG_DEFCONFIG=%s' % debug_defconfig,
	'SELINUX_DEFCONFIG=selinux_defconfig',
	'SELINUX_LOG_DEFCONFIG=selinux_log_defconfig',
        '%s' % base_defconfig], env=make_env, stdin=devnull)
    devnull.close()

    if not all_options.updateconfigs:
        build = Builder(log_name)

        result = build.run(['make', 'O=%s' % dest_dir] + make_command)

        if result != 0:
            if all_options.keep_going:
                failed_targets.append(target)
                fail_or_error = error
            else:
                fail_or_error = fail
            fail_or_error("Failed to build %s, see %s" % (target, build.logname))

    # Copy the defconfig back.
    if all_options.configs or all_options.updateconfigs:
        devnull = open('/dev/null', 'r')
        subprocess.check_call(['make', 'O=%s' % dest_dir,
            'savedefconfig'], env=make_env, stdin=devnull)
        devnull.close()
        shutil.copyfile(savedefconfig, defconfig)

    shutil.copyfile(zImage_name, copy_zImage_name)
    shutil.copyfile(zImage_name, '%s.%s' % (copy_zImage_name, target))
   
    # make boot.img, (zImage+ramdisk.img)
    ramdisk_name ='%s/ramdisk.img.%s' % (build_dir,target)
    if (os.path.isfile(ramdisk_name)):
        print "Execute mkbootimg using ramdisk.img.%s" % target
    	shutil.copyfile(ramdisk_name, '%s/ramdisk.img' % build_dir)
        result = subprocess.call("mkbootimg.sh ../okernel ../okernel", shell=True)
        if result != 0:
            print "Skip mkbootimg, there is not mkbootimg.sh"
        else:
            shutil.move('%s/boot.tar' % (build_dir), '%s/boot_%s.tar' % (build_dir,target))
    else:
        print "Skip mkbootimg, tar only zImage for %s targets" % target
    	tarball_name = '%s/boot_%s.tar' % (build_dir, target)
        tar = tarfile.open(tarball_name, "w")
        tar.add(copy_zImage_name, arcname='boot.img')
        tar.close()
        
    print "End build %s targets\n" % target

def build_many(allconf, targets):
    print "Building %d target(s)" % len(targets)
    targets.sort()
    print "Targets : %s" % targets
    for target in targets:
        if all_options.updateconfigs:
            update_config(allconf[target], all_options.updateconfigs)
        build(target)
    if failed_targets:
        fail('\n  '.join(["Failed targets:"] +
            [target for target in failed_targets]))

def main():
    global make_command

    check_kernel()
    check_build()

    configs = scan_configs()

    usage = ("""
           %prog [options] all				-- Build all targets
           ex) $ build-all.py all
           
           %prog [options] target			-- Build predefined targets
           ex) $ build-all.py express2_att
           
           %prog --list					-- List available predefined targets
           ex) $ build-all.py --list
           
           %prog -s base_def:variant_def target		-- Build specific targets
           ex) $ build-all.py -s serrano_usa:serrano_tmo serrano_tmo
           
           %prog [options] -d/-debug eng			-- DEBUG_DEFCONFIG 
           ex) $ build-all.py -d eng express2_att""")
    parser = OptionParser(usage=usage, version=version)
    parser.add_option('--configs', action='store_true',
            dest='configs',
            help="Copy configs back into tree")
    parser.add_option('--list', action='store_true',
            dest='list',
            help='List available targets')
    parser.add_option('-v', '--verbose', action='store_true',
            dest='verbose',
            help='Output to stdout in addition to log file')
    parser.add_option('--oldconfig', action='store_true',
            dest='oldconfig',
            help='Only process "make oldconfig"')
    parser.add_option('--updateconfigs',
            dest='updateconfigs',
            help="Update defconfigs with provided option setting, "
                 "e.g. --updateconfigs=\'CONFIG_USE_THING=y\'")
    parser.add_option('-j', '--jobs', type='int', dest="jobs", default=12,
            help="Number of simultaneous jobs")
    parser.add_option('-d', '--debug', type='str', dest="debug",
            help="To use DEBUG_DEFCONFIG")
    parser.add_option('-s', '--specify', type='str', dest="specified",
            help="To specify targets")
    parser.add_option('-l', '--load-average', type='int',
            dest='load_average',
            help="Don't start multiple jobs unless load is below LOAD_AVERAGE")
    parser.add_option('-k', '--keep-going', action='store_true',
            dest='keep_going', default=False,
            help="Keep building other targets if a target fails")
    parser.add_option('-m', '--make-target', action='append',
            help='Build the indicated make target (default: %s)' %
                 ' '.join(make_command))

    (options, args) = parser.parse_args()
    global all_options
    all_options = options

    if options.list:
        print "Available targets:"
        for target in configs.keys():
            print "   %s" % target
        sys.exit(0)

    if options.oldconfig:
        make_command = ["oldconfig"]
    elif options.make_target:
        make_command = options.make_target

    if options.jobs:
        make_command.append("-j%d" % options.jobs)
    if options.load_average:
        make_command.append("-l%d" % options.load_average)

    if args == ['all']:
        build_many(configs, configs.keys())
    elif len(args) > 0:
        targets = []
        for t in args:
            if t not in configs.keys():
                parser.error("Target '%s' not one of %s" % (t, configs.keys()))
            targets.append(t)
        build_many(configs, targets)
    else:
        parser.error("Must specify a target to build, or 'all'")

if __name__ == "__main__":
    main()
