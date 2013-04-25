# -*- python -*-
# ex: set syntax=python:
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.

from buildbot.process import factory
from buildbot.steps.source import Mercurial
from buildbot.steps.shell import *
from custom.buildbot_ext.steps.shellAddons import *
from buildbot.steps.trigger import Trigger


####### Utility Functions for ignoring certain file types
def startPerformanceRun(change):
    for name in change.files:
        if name.endswith(('.cpp','.h','.py','asm','.as')):
            return True
        elif name.endswith('.abc'):
            # Only run if abc is not in esc dir
            if '/esc/' not in name:
                return True
    return False
    
def startCompile(change):
    '''Determine whether we want to start a compile pass based on the files that
        have changed.  Only skip compile if ALL changes are in the ignore criteria.
    '''
    compile = True
    for name in change.files:
        # ignore all changes to buildbot master files
        if ('/buildbot/master/' in name) or ('utils/hooks/' in name):
            compile = False
        # ignore changes to the runsmokes*.txt files
        elif ('runsmokes' in name) and name.endswith('.txt'):
            compile = False
        # ignore the build trigger files that are added
        elif ('change-' in name):
            compile = False
        else:
            return True
    return compile



############################
####### COMMON BUILD STEPS
############################

# For an explanation of the WithProperties('(%(silent:-)s') syntax see
# http://djmitche.github.com/buildbot/docs/current/#Using-Build-Properties

def compile_generic(name, shellname, args, upload, features=""):
    # factory.addStep(compile_generic(name="Release", shellname="avmshell", args="", upload="false", features="+AVMSYSTEM_32BIT"))
    # upload: if true build will be uploaded to asteam, this is normaly done in the upload buildstep
    #         but is here for shells that are compiled in deep-testing
    return BuildShellCommand(
            command=['../all/compile-generic.sh', WithProperties('%s','revision'), '%s' % args, '%s' % shellname, '%s' % upload, '%s' % features],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting %s build...' % name,
            descriptionDone='finished %s build' % name,
            name="Build_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts"
            )


def test_generic(name, shellname, vmargs, config, scriptargs, _timeout=1200):
    # factory.addStep(test_generic("Release", "avmshell", "", "", ""))
    return TestSuiteShellCommand(
            command=['../all/run-acceptance-generic.sh', WithProperties('%s','revision'), '%s' % shellname, '%s' % vmargs, '%s' % config, '%s' % scriptargs],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run %s vmtests...' % name,
            descriptionDone='finished %s vmtests' % name,
            name="Testsuite_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts",
            timeout=_timeout
            )
    

def test_generic_ssh(name, shellname, vmargs, config, scriptargs):
    # factory.addStep(test_generic_ssh("Release", "avmshell", "", "", ""))
    return TestSuiteShellCommand(
            command=['../all/run-acceptance-generic-ssh.sh', WithProperties('%s','revision'), '%s' % shellname, '%s' % vmargs, '%s' % config, '%s' % scriptargs],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run %s vmtests...' % name,
            descriptionDone='finished %s vmtests' % name,
            name="Testsuite_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts"
            )


def test_generic_adb(name, shellname, vmargs, config, scriptargs):
    # factory.addStep(test_generic_adb("Release", "avmshell", "", "", ""))
    return TestSuiteShellCommand(
            command=['../all/run-acceptance-generic-adb.sh', WithProperties('%s','revision'), '%s' % shellname, '%s' % vmargs, '%s' % config, '%s' % scriptargs],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run %s vmtests...' % name,
            descriptionDone='finished %s vmtests' % name,
            name="Testsuite_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts"
            )


def test_emulator_generic(name, shellname, vmargs, config, scriptargs):
    # factory.addStep(test_emulator_generic("Release", "avmshell", "", "", ""))
    return TestSuiteShellCommand(
            command=['../all/run-acceptance-emulator-generic.sh', WithProperties('%s','revision'), '%s' % shellname, '%s' % vmargs, '%s' % config, '%s' % scriptargs],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run %s vmtests...' % name,
            descriptionDone='finished %s vmtests' % name,
            name="Testsuite_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts",
            timeout=3600,
            )


sync_clean = ShellCommand(
            command=["rm", "-Rf", "repo"],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Remove the old repository...',
            descriptionDone='Finished Removing the old repository',
            name='Source_Clean',
            workdir="../",
            haltOnFailure="True",
            usePTY=False)
            # Do not use PTY when calling 'rm' as it will sometimes report a failure.
            #    http://buildbot.net/trac/ticket/284
            #    http://buildbot.net/trac/ticket/255

def sync_clone(url):
    return ShellCommand(
            command=["hg", "clone", url, "repo"],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Cloning the source repository...',
            descriptionDone='Finished cloning the source repository',
            name='Source_Clone',
            workdir="../",
            haltOnFailure="True")


sync_clone_sandbox = SandboxClone(
            dest="repo",
            changeDir="changes/deep/processed",
            description='Cloning the source repository...',
            descriptionDone='Finished cloning the source repository',
            name='Source_Clone',
            workdir="../",
            haltOnFailure="True")

sync_update = ShellCommand(
            command=["hg", "update", "--clean",  "--rev", WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Updating the source repository...',
            descriptionDone='Finished updating the source repository',
            name='Source_Update',
            workdir="../repo",
            haltOnFailure="True")

# pull new changes into an existing repository.  Used on promote-build slave to
# save time instead of deleting / recloning entire repo.
sync_pull = ShellCommand(
            command=["hg", "pull"],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Pulling new changes to source repository...',
            descriptionDone='Finished pulling new changes to source repository',
            name='Source_Pull',
            workdir="../repo",
            haltOnFailure="True")

def bb_slaveupdate(slave):
    return ShellCommand(
            command=['cp','-R','repo/build/buildbot/slaves/%s/scripts' % slave, 'repo/build/buildbot/slaves/scripts'],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            workdir='../',
            description='Updating SLAVE buildscripts',
            name='BB_SLAVEUpdate',
            haltOnFailure="True",
            usePTY=False)
            # Do not use PTY when calling 'cp' as it will sometimes report a failure.
            #    http://buildbot.net/trac/ticket/284
            #    http://buildbot.net/trac/ticket/255

bb_lockacquire = BuildShellCommand(
            command=['../all/lock-acquire.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Acquiring machine lock...',
            descriptionDone='Acquired machine lock...',
            name="LockAcquire",
            workdir="../repo/build/buildbot/slaves/scripts")

bb_lockrelease = BuildShellCommand(
            command=['../all/lock-release.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Releasing machine lock...',
            descriptionDone='Released machine lock...',
            name="LockRelease",
            workdir="../repo/build/buildbot/slaves/scripts")

verify_builtinabc = BuildShellCommand(
            command=['../all/verify-builtinabc.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to verify generated builtin files..',
            descriptionDone='generated builtin files diff completed',
            name="Verify_BuiltinABC",
            workdir="../repo/build/buildbot/slaves/scripts")

verify_tracers = BuildShellCommand(
            command=['../all/verify-tracers.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to verify generated exactgc tracer files..',
            descriptionDone='generated exactgc tracer files diff completed',
            name="Verify_Tracers",
            workdir="../repo/build/buildbot/slaves/scripts")

compile_buildcheck = BuildShellCheckCommand(
            command=['../all/build-check.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting build check...',
            descriptionDone='build check completed',
            name='Build_Check',
            workdir="../repo/build/buildbot/slaves/scripts")

## Local version runs a local script and not the common
compile_buildcheck_local = BuildShellCheckCommand(
            command=['./build-check.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting build check...',
            descriptionDone='build check completed',
            name='Build_Check',
            workdir="../repo/build/buildbot/slaves/scripts")

compile_testmedia = BuildShellCommand(
            command=['../all/build-acceptance-tests.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to build test abcs...',
            descriptionDone='vm test abcs built.',
            name="Compile_AS_testcases",
            workdir="../repo/build/buildbot/slaves/scripts")

download_testmedia = BuildShellCommand(
            command=['../all/download-acceptance-tests.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to downloads test abcs...',
            descriptionDone='vm test abcs downloaded.',
            name="Download_AS_testcases",
            workdir="../repo/build/buildbot/slaves/scripts",
            haltOnFailure="True" )

test_smoke = TestSuiteShellCommand(
            command=['../all/run-smoketests.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run smoke tests...',
            descriptionDone='finished smoke tests.',
            name="SmokeTest",
            workdir="../repo/build/buildbot/slaves/scripts")

test_smoke_ssh = TestSuiteShellCommand(
            command=['../all/run-smoketests-ssh.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run smoke tests...',
            descriptionDone='finished smoke tests.',
            name="SmokeTest",
            workdir="../repo/build/buildbot/slaves/scripts")

test_smoke_local = TestSuiteShellCommand(
            command=['./run-smoketests.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run smoke tests...',
            descriptionDone='finished smoke tests.',
            name="SmokeTest",
            workdir="../repo/build/buildbot/slaves/scripts")

test_emulator_smoke_mobile = TestSuiteShellCommand(
            command=['../all/run-smoketests-arm-emulator.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run smoke tests...',
            descriptionDone='finished smoke tests.',
            name="SmokeTest",
            workdir="../repo/build/buildbot/slaves/scripts")

def test_selftest(name, shellname):
    # factory.addStep(test_selftest("Release", "avmshell"))
    return TestSuiteShellCommand(
            command=['../all/run-selftest-generic.sh', WithProperties('%s','revision'), '%s' % shellname],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting selftest %s...' % name,
            descriptionDone='finished selftest %s.' % name,
            name="Testsuite_Selftest_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts"
            )

def test_selftest_ssh(name, shellname):
    # factory.addStep(test_selftest_ssh("Release", "avmshell"))
    return TestSuiteShellCommand(
            command=['../all/run-selftest-generic-ssh.sh', WithProperties('%s','revision'), '%s' % shellname],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting selftest %s...' % name,
            descriptionDone='finished selftest %s.' % name,
            name="Testsuite_Selftest_%s" % name,
            workdir="../repo/build/buildbot/slaves/scripts"
            )

test_commandline = TestSuiteShellCommand(
            command=['../all/run-commandline-tests.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting commandline tests...',
            descriptionDone='finished commandline tests.',
            name="Testsuite_Commandline",
            workdir="../repo/build/buildbot/slaves/scripts")

test_differential = TestSuiteShellCommand(
            command=['../all/run-acceptance-avmdiff.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting vm acceptance differential testing...',
            descriptionDone='finished vm acceptance differential testing.',
            name="Testsuite_Differential",
            workdir="../repo/build/buildbot/slaves/scripts")

test_misc = TestSuiteShellCommand(
            command=['../all/run-misc-tests.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to run misc tests...',
            descriptionDone='finished misc tests.',
            name="MiscTest",
            workdir="../repo/build/buildbot/slaves/scripts")

util_upload_asteam = BuildShellCheckCommand(
            command=['../all/upload-asteam.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Upload bits to ASTEAM...',
            descriptionDone='Upload to ASTEAM completed',
            name='Upload_ASTEAM',
            workdir="../repo/build/buildbot/slaves/scripts")

## Local version runs a local script and not the common
util_upload_asteam_local = BuildShellCheckCommand(
            command=['./upload-asteam.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Upload bits to ASTEAM...',
            descriptionDone='Upload to ASTEAM completed',
            name='Upload_ASTEAM',
            workdir="../repo/build/buildbot/slaves/scripts")

util_upload_mozilla = BuildShellCheckCommand(
            command=['../all/upload-mozilla.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Upload bits to MOZILLA...',
            descriptionDone='Upload to MOZILLA completed',
            name='Upload_MOZILLA',
            workdir="../repo/build/buildbot/slaves/scripts")

## Local version runs a local script and not the common
util_upload_mozilla_local = BuildShellCheckCommand(
            command=['./upload-mozilla.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Upload bits to MOZILLA...',
            descriptionDone='Upload to MOZILLA completed',
            name='Upload_MOZILLA',
            workdir="../repo/build/buildbot/slaves/scripts")

util_process_clean = BuildShellCommand(
            command=['../all/util-process-clean.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Zombie hunting...',
            descriptionDone='Zombie hunt completed',
            name='Util_ZombieKiller',
            workdir="../repo/build/buildbot/slaves/scripts",
            alwaysRun="True" )

util_process_clean_ssh = BuildShellCommand(
            command=['../all/util-process-clean-ssh.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Zombie hunting...',
            descriptionDone='Zombie hunt completed',
            name='Util_ZombieKiller',
            workdir="../repo/build/buildbot/slaves/scripts",
            alwaysRun="True" )

util_acceptance_clean_ssh = BuildShellCommand(
            command=['../all/util-acceptance-clean-ssh.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Cleaning acceptance tests...',
            descriptionDone='Cleaning acceptance tests completed',
            name='Util_CleanAcceptanceTests',
            workdir="../repo/build/buildbot/slaves/scripts",
            alwaysRun="True" )

util_clean_buildsdir = BuildShellCommand(
            command=['../all/util-clean-buildsdir.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Remove build folders older than one week...',
            descriptionDone='Finished removing build folders',
            name="Clean_Build_Dir",            
            workdir="../repo/build/buildbot/slaves/scripts",
            alwaysRun="True" )

perf_prepare = BuildShellCommand(
            command=['./prepare.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='Preparing for performance run...',
            descriptionDone='Preparation complete...',
            name="Preparation",
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

perf_release = PerfShellCommand(
            command=['../all/run-performance-release.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release performance tests...',
            descriptionDone='finished release performance tests.',
            name='Release',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

perf_release_arm = PerfShellCommand(
            command=['../all/run-performance-release-arm.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release performance tests...',
            descriptionDone='finished release performance tests.',
            name='Release',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

perf_release_interp = PerfShellCommand(
            command=['../all/run-performance-release-interp.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release-interp performance tests...',
            descriptionDone='finished release-interp performance tests.',
            name='ReleaseInterp',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

perf_release_arm_interp = PerfShellCommand(
            command=['../all/run-performance-release-arm-interp.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release-interp performance tests...',
            descriptionDone='finished release-interp performance tests.',
            name='ReleaseInterp',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

perf_release_jit = PerfShellCommand(
            command=['../all/run-performance-release-jit.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release-jit performance tests...',
            descriptionDone='finished release-jit performance tests.',
            name='ReleaseJIT',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

perf_release_arm_jit = PerfShellCommand(
            command=['../all/run-performance-release-arm-jit.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release-jit performance tests...',
            descriptionDone='finished release-jit performance tests.',
            name='ReleaseJIT',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts")

def acceptance_performance(name, shellname, vmargs="", config="", scriptargs=""):
            return test_generic(
                name="Acceptance_Performance_%s" % name, shellname=shellname, vmargs=vmargs, config=config,
                scriptargs="--threads=1 --addtoconfig=-performance %s" % scriptargs)

def deep_codecoverage(compilecsv, testcsv):
    return BuildShellCommand(
            command=['../all/codecoverage-runner.sh', '-b', WithProperties('%s','revision'), '--compilecsv=%s' % compilecsv, '--testcsv=%s' % testcsv],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting code coverage...',
            descriptionDone='finished code coverage',
            name='CodeCoverage',
            timeout=3600,
            workdir="../repo/build/buildbot/slaves/scripts"
            )

deep_codecoverage_process = BuildShellCommand(
            command=['../all/codecoverage-process.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting to process coverage data...',
            descriptionDone='finished processing coverage data',
            name='CodeCoverageProcess',
            workdir="../repo/build/buildbot/slaves/scripts"
            )

deep_release_esc = BuildShellCommand(
            command=['../all/run-release-esc.sh', WithProperties('%s','revision')],
            env={'branch': WithProperties('%s','branch'), 'silent':WithProperties('%s','silent')},
            description='starting release-esc tests...',
            descriptionDone='finished release-esc tests.',
            name='Release-esc',
            workdir="../repo/build/buildbot/slaves/scripts")

def deep_run_brightspot(name,shell,testargs):
    return TestSuiteShellCommand(
        command=['../all/run-brightspot.sh', WithProperties('%s','revision'), shell, testargs],
        env={'branch': WithProperties('%s','branch'), 'silent': WithProperties('%s','silent')},
        description='running %s BrightSpot...' % name,
        descriptionDone='finished running %s BrightSpot.' % name,
        name='BrightSpot %s' % name,
        workdir='../repo/build/buildbot/slaves/scripts',
        timeout=3600)
