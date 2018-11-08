#!/usr/bin/env python
'''
    Make a release of libsemigroups from the current directory.
'''
# pylint: disable=

import os
import argparse
import sys
import re
import time
import textwrap
import subprocess
import shutil

from git import Repo, GitCommandError

###############################################################################
# Globals
###############################################################################

__WRAPPER = textwrap.TextWrapper(break_on_hyphens=False, width=80)
__VERBOSE = False
__TODAY = time.strftime("%d/%m/%Y")
__OLD_VERSION = None
__NEW_VERSION = None

###############################################################################
# Helpers
###############################################################################


def _wrap_string(string):
    assert isinstance(string, str)
    return '\n'.join(__WRAPPER.wrap(string))


def _pad_string(string, extra=0):
    string += ' ' * (extra + 50 - len(string))
    return string


def _red_string(string):
    assert isinstance(string, str)
    return _wrap_string('\033[31m%s\033[0m' % string)


def _neon_green_string(string):
    assert isinstance(string, str)
    return _wrap_string('\033[40;38;5;82m%s\033[0m' % string)


def _exit_abort(message=''):
    if message != '':
        sys.exit(_red_string(message + '! Aborting!'))
    else:
        sys.exit(_red_string('Aborting!'))


def _exit_error(message):
    sys.exit(_red_string('Error! ' + message))


def _exit_killed(*arg):
    for pro in arg:
        pro.terminate()
        pro.wait()
    sys.exit(_red_string('\nKilled!'))


def _info_statement(message):
    sys.stdout.write(_neon_green_string(message) + '\n')


def _info_verbose(message):
    if __VERBOSE:
        message = '\033[36m%s\033[0m\n' % message
        sys.stdout.write(message)


def _exec_string(string):
    '''
    Execute the string in a subprocess.
    '''
    _info_verbose(string)
    try:
        null = open(os.devnull, 'w')
        subprocess.check_call(string,
                              shell=True,
                              stdout=null,
                              stderr=subprocess.STDOUT)
    except KeyboardInterrupt:
        _exit_killed()
    except (subprocess.CalledProcessError, OSError):
        _exit_error(string + ' failed')

###############################################################################
# Read files, get things from file
###############################################################################


def _get_file_contents(filename):
    if not (os.path.exists(filename) and
            os.path.isfile(filename)):
        _exit_error('Cannot find the file ' + filename)

    try:
        return open(filename, 'r').read()
    except KeyboardInterrupt:
        _exit_killed()
    except IOError:
        _exit_error('Cannot read ' + filename)


def _new_version():
    '''
    Returns the version number we are releasing.
    '''
    global __NEW_VERSION
    if __NEW_VERSION is not None:
        return __NEW_VERSION

    __NEW_VERSION = _get_file_contents('VERSION').strip()
    match = re.compile(r'\d+.\d+.\d+').match(__NEW_VERSION)
    if not match or not match.group(0) == __NEW_VERSION:
        _exit_abort('The version number should be of the form x.y.z but is ' +
                    __NEW_VERSION)
    return __NEW_VERSION


def _old_version():
    '''
    Returns the last released version number.
    '''
    global __OLD_VERSION
    if __OLD_VERSION is not None:
        return __OLD_VERSION

    _info_verbose('Fetching tags from origin  . . .')
    try:
        repo = Repo(os.getcwd())
        origin = repo.remotes.origin
        origin.fetch(tags=True)
    except GitCommandError as e:
        _exit_abort(str(e.stderr.strip()))

    # Check if a tag exists for the version we are trying to release
    versions = [str(x)[1:] for x in repo.tags if str(x)[0] == 'v']
    versions.sort(key=lambda s: map(int, s.split('.')))
    __OLD_VERSION = versions[-1]
    return __OLD_VERSION


def _is_major_release():
    return _old_version()[0] != _new_version()[0]


def _is_minor_release():
    old = _old_version().split('.')
    new = _new_version().split('.')
    return old[0] == new[0] and old[1] != new[1]


def _is_patch_release():
    old = _old_version().split('.')
    new = _new_version().split('.')
    return old[0] == new[0] and old[1] == new[1] and old[2] != new[2]


def _stable_branch():
    split = _new_version().split('.')
    return 'stable-%s.%s' % (split[0], split[1])


def _rc_branch():
    return 'rc-v' + _new_version()


def push_and_pull_request():
    origin = Repo(os.getcwd()).remotes.origin
    origin.push(_rc_branch)
    origin.pull_request(_stable_branch(), _rc_branch)

###############################################################################
# Prerelease checks
###############################################################################


def _check_readme():
    '''
    Check if the README.md is up to date.
    '''
    readme = _get_file_contents('README.md')
    if readme.find(_old_version()) != -1:
        _exit_abort('Found old version number ' + _old_version() +
                    ' in README.md:'
                    + str(readme.count('\n', 0, readme.find(_old_version()))))

    _info_verbose('README.md is ok!')


def _check_change_log():
    '''
    Check the date and version number in the CHANGELOG.md file.
    '''
    contents = _get_file_contents('CHANGELOG.md')
    regex = re.compile(r'##\s*Version\s*' + _new_version() +
                       r'\s*\(released\s*(\d\d/\d\d/\d\d\d\d)\)')
    match = regex.search(contents)

    if match:
        if match.group(1) != __TODAY:
            _exit_abort('The date in CHANGELOG.md is ' + match.group(1)
                        + ' but today is ' + __TODAY)
    else:
        _exit_abort('The entry for version %s in CHANGELOG.md'
                    % _new_version() + ' is missing or incorrect')


def _git_remote_branch_exists(repo, branch):
    _info_verbose('Checking if branch origin/%s exists . . .' % branch)
    try:
        repo.remotes.origin.fetch(branch)
        _info_verbose('branch %s exists!' % branch)
        return True
    except Exception:
        _info_verbose('branch %s does not exist!' % branch)
        return False


def _git_local_branch_exists(repo, branch):
    _info_verbose('Checking if branch %s exists . . .' % branch)
    try:
        repo.git.checkout(branch, b=branch)
        _info_verbose('branch %s exists!' % branch)
        return True
    except Exception:
        _info_verbose('branch %s does not exist!' % branch)
        return False


def _git_check_origin(repo, git_branch):
    try:
        _info_statement('Fetching from origin/%s  . . .' % git_branch)
        origin = repo.remotes.origin
        origin.fetch(git_branch)
        commits_behind = repo.iter_commits(git_branch + '..origin/'
                                           + git_branch)
        nr_behind = sum(1 for c in commits_behind)
        if nr_behind > 0:
            _exit_abort('The local repo is behind origin/%s by %d'
                        % (git_branch, nr_behind) + ' commits')
        _info_verbose('The local repo is up-to-date with origin/'
                      + git_branch)
    except GitCommandError as e:
        _exit_abort(str(e.stderr.strip()))


def _git_check_gh_pages():
    msg = ('\033[31mTry:\ngit clone -b gh-pages --single-branch '
           + 'git@github.com:gap-packages/Semigroups.git '
           + 'gh-pages\033[0m')

    if not os.path.exists('gh-pages'):
        print(_red_string('Cannot find the gh-pages directory!'))
        print(msg)
        _exit_abort()
    elif not os.path.isdir('gh-pages'):
        print(_red_string('gh-pages is not a directory! Delete it!'))
        print(msg)
        _exit_abort()

    try:
        repo = Repo(os.getcwd() + '/gh-pages')
    except Exception:
        print(_red_string('Cannot find the gh-pages repo!'))
        print(msg)
        _exit_abort()

    _git_check_origin(repo, 'gh-pages')
    _info_verbose('gh-pages are ok!')


def _check_git():
    '''
    Checks if the git branch name is <stable-x.y> when the version number is
    x.y.z.
    '''
    repo = Repo(os.getcwd())

    # Check in correct branch . . .
    split = _new_version().split('.')
    expected_branch = 'rc-v' + _new_version()
    stable_branch = 'stable-%s.%s' % (split[0], split[1])
    git_branch = repo.active_branch.name

    if git_branch != expected_branch:
        _exit_abort('In branch %s should be %s'
                    % (git_branch, expected_branch))
    _info_verbose('Current branch is ' + git_branch)

    # Check for local changes . . .
    if repo.is_dirty():
        _exit_abort('There are uncommitted changes')
    elif len(repo.untracked_files) > 0:
        _exit_abort('There are untracked files')
    _info_verbose('No local changes or untracked files in repo')

    # Check for changes in origin . . .
    if _git_remote_branch_exists(repo, stable_branch):
        # TODO must check that the local branch exists too
        _git_check_origin(repo, stable_branch)
    _git_check_origin(repo, 'master')


def _check_version_numbers():

    repo = Repo(os.getcwd())
    # This assumes that version numbers are strictly increasing
    # Check if a tag exists for the version we are trying to release
    versions = [str(x)[1:] for x in repo.tags if str(x)[0] == 'v']
    versions.sort(key=lambda s: map(int, s.split('.')))
    first_version, last_version = versions[0], versions[-1]

    min_version = last_version.split('.')
    min_version[-1] = str(int(last_version.split('.')[-1]) + 1)
    min_version = '.'.join(min_version)

    if _new_version() in versions:  # Check if version already exists
        _exit_abort('Version %s already exists, must be %s'
                    % (_new_version(), min_version)
                    + ' or higher')
    elif (map(int, _new_version().split('.'))
          < map(int, first_version.split('.'))):
        # Check that version number is not too small
        _exit_abort('The version number should be at least %s but is %s'
                    % (min_version, _new_version()))

    # Check if some version number has been skipped
    last_version = map(int, last_version.split('.'))
    this_version = map(int, _new_version().split('.'))
    if this_version[0] > last_version[0] + 1:
        # Skipped a major version
        correct_version = str(last_version[0] + 1) + '.0.0'
        _exit_abort('The version number should be %s but is %s'
                    % (correct_version, _new_version()))
    elif last_version[0] != this_version[0]:
        if this_version[1] > 0 or this_version[2] > 0:
            # Making a major release not of the form x.0.0
            correct_version = str(last_version[0] + 1) + '.0.0'
            _exit_abort('The version number should be %s but is %s'
                        % (correct_version, _new_version()))
    elif this_version[1] > last_version[1] + 1:
        # Skipped a minor version
        correct_version = (str(last_version[0]) + '.'
                           + str(last_version[1] + 1) + '.0')
        _exit_abort('The version number should be %s but is %s'
                    % (correct_version, _new_version()))
    elif last_version[1] != this_version[1]:
        if this_version[2] > 0:
            # Making a minor release not of the form x.y.0
            correct_version = (str(last_version[0]) + '.'
                               + str(last_version[1] + 1) + '.0')
            _exit_abort('The version number should be %s but is %s'
                        % (correct_version, _new_version()))
    elif last_version[2] + 1 != this_version[2]:
        # Making a patch release not of the form x.y.z + 1 when the last
        # release was x.y.z
        correct_version = (str(last_version[0]) + '.' + str(last_version[1])
                           + '.' + str(last_version[2] + 1))
        _exit_abort('The version number should be %s but is %s'
                    % (correct_version, _new_version()))

    _info_verbose('The version number %s agrees with the existing versions in'
                  % _new_version() + ' the repo')


__CHECKS = [_check_version_numbers, _check_git, _check_readme, _git_check_gh_pages]
        #_check_change_log,

###############################################################################
# Actually make a release
###############################################################################


def _release():
    _info_statement('Starting release process . . .')
    repo = Repo(os.getcwd())
    start_branch = repo.active_branch
    try:
        if not _git_remote_branch_exists(repo, _rc_branch()):
            _exit_abort('origin/%s does not exist!\n' % _rc_branch()
                        + 'Did you forget to push?')
        # TODO check if the local branch is up to date with the remote one
        # TODO check if the CI actually passed
        branch = _stable_branch()
        if _is_major_release() and not _git_remote_branch_exists(repo, branch):
            _info_statement('Creating branch %s . . .' % branch)
            branch = repo.create_head(branch)
        else:
            _info_statement('Checking out branch %s . . .' % branch)
            repo.git.checkout(branch)
            branch = repo.active_branch
        _info_statement('Pulling from origin/%s into %s . . .'
                        % (_rc_branch(), branch.name))
        repo.remotes.origin.pull(refspec='+refs/heads/%s:refs/remotes/origin/%s'
                                 % (_rc_branch(), _rc_branch()))

        _info_verbose('Updating HEAD to %s . . .' % branch.name)
        repo.head.reference = branch
        _info_statement('Pushing to origin/%s . . .' % branch.name)
        repo.remotes.origin.push(branch)

        _info_statement('Tagging repo with v%s . . .' % _new_version())
        repo.create_tag('v%s' % _new_version())
        _info_statement('Pushing tag v%s to origin/%s . . .'
                        % (_new_version(), branch))
        repo.remotes.origin.push('v%s' % _new_version())

        _info_statement('Processing the documentation . . .')
        _exec_string('make doc')
        _exec_string('cp -r html/* gh-pages')

        os.chdir('gh-pages')
        repo = Repo(os.getcwd())
        repo.git.add(A=True)
        repo.index.commit('Version %s' % _new_version())
        _info_statement('Pushing to origin/gh-pages . . .')
        repo.remotes.origin.push('gh-pages')
        # TODO ask if we should delete the local rc-v*.*.* branch
        # TODO ask if we should delete the remote rc-v*.*.* branch
        # TODO the things below...
        _info_statement('This script is incomplete, don\'t forget to:')
        _info_statement('  * archive:  make dist')
        _info_statement('  * go to github and make a release announcement and '
                        + 'add the archive')
        _info_statement('SUCCESS!')
    except Exception as e:
        _info_statement('Something went wrong, reverting to branch %s . . .'
                        % start_branch.name)
        repo.git.checkout(start_branch)
        _exit_abort(str(e))

###############################################################################
# The main event
###############################################################################


def _parse_args():
    parser = argparse.ArgumentParser(prog='release-pkg',
                                     usage='%(prog)s [options]')

    parser.add_argument('--verbose', dest='verbose', action='store_true',
                        help='verbose mode (default: False)')
    parser.set_defaults(verbose=False)

    parser.add_argument('--dry-run', dest='dry_run', action='store_true',
                        help='dry run (default: False)')
    parser.set_defaults(dry_run=False)

    parser.add_argument('--gap-repo', nargs='*', type=str,
                        help='the gap repo directory (default: ~/gap)',
                        default='~/gap/')

    parser.add_argument('--pkg-dir', nargs='?', type=str,
                        help='the pkg directory (default: gap-repo/pkg/)',
                        default=None)

    parser.add_argument('--ignore-version', dest='ignore_version',
                        action='store_true',
                        help='ignore the version number (default: False)')
    parser.set_defaults(ignore_version=False)
    return parser.parse_args()


def _main():
    global __VERBOSE, __IGNORE_VERSION

    args = _parse_args()
    __VERBOSE = args.verbose
    __IGNORE_VERSION = args.ignore_version

    _info_statement('The version being released is: ' + _new_version())
    _info_statement('The last version released was: ' + _old_version())

    # Perform prerelease checks
    for check in __CHECKS:
        check()

    # actually publish?
    if args.dry_run:
        _info_statement('Dry run succeeded!')
        _info_statement('To complete the release make a pull request from '
                        + 'rc-v' + _new_version())
        # TODO actually make a PR here in code
        _info_statement('If the CI tests pass, then rebase the changes '
                        'on ' + _stable_branch()
                        + ' and rerun this script without --dry-run')
        _exit_abort()

    _release()
    sys.exit(0)

if __name__ == '__main__':
    try:
        _main()
    except KeyboardInterrupt:
        _exit_killed()
