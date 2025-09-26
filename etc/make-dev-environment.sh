#!/bin/bash
is_sourced() {
    if [ -n "$ZSH_VERSION" ]; then
        case $ZSH_EVAL_CONTEXT in *:file:*) return 0 ;; esac
    else
        case ${0##*/} in dash | -dash | bash | -bash | ksh | -ksh | sh | -sh) return 0 ;; esac
    fi
    return 1 # NOT sourced.
}

is_sourced && sourced=1 || sourced=0
if [[ $sourced -ne 1 ]]; then
    echo This script must be sourced, rather than run directly. Please try again with:
    echo source $(basename "$0")
    exit
fi

unset sourced

if [[ $# -eq 0 ]]; then
    dev_env_pkg_manager=mamba
else
    dev_env_pkg_manager=$1
fi

if { $dev_env_pkg_manager env list | grep 'libsemigroups_dev'; } >/dev/null 2>&1; then
    echo The environment libsemigroups_dev already exists. Stopping ...
    unset dev_env_pkg_manager
    return
else
    echo Making libsemigroups_dev environment using $dev_env_pkg_manager ...
    echo
    $dev_env_pkg_manager env create -f dev-environment.yml
    if [[ $? -ne 0 ]]; then
        echo The environment was not successfully created. Stopping ...
        unset dev_env_pkg_manager
        return
    fi
    echo Activating libsemigroups_dev ...
    echo
    $dev_env_pkg_manager activate libsemigroups_dev
    if [[ $? -ne 0 ]]; then
        echo The environment cannot be activated. Please try activating it yourself.
        unset dev_env_pkg_manager
        return
    fi
fi

unset dev_env_pkg_manager
