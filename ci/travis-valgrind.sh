#!/bin/bash
set -e

sh -c "$(curl -fsSL https://raw.githubusercontent.com/Linuxbrew/install/master/install.sh)"
test -d ~/.linuxbrew && PATH="$HOME/.linuxbrew/bin:$HOME/.linuxbrew/sbin:$PATH"
test -d /home/linuxbrew/.linuxbrew && PATH="/home/linuxbrew/.linuxbrew/bin:/home/linuxbrew/.linuxbrew/sbin:$PATH"
test -r ~/.bash_profile && echo "export PATH='$(brew --prefix)/bin:$(brew --prefix)/sbin'":'"$PATH"' >>~/.bash_profile
echo "export PATH='$(brew --prefix)/bin:$(brew --prefix)/sbin'":'"$PATH"' >>~/.profile

brew update
brew install valgrind

# Print version
echo "Valgrind version:"
valgrind --version

ci/travis-setup.sh
./configure --enable-debug --disable-hpcombi
make test_all -j4

unbuffer libtool --mode=execute valgrind --leak-check=full ./test_all "[quick][exclude:no-valgrind]" 2>&1 | tee valgrind.log

echo
( ! grep -i "Invalid" valgrind.log )
( ! grep -E "lost: [^0]" valgrind.log )
