git clone https://github.com/yyuu/pyenv.git pyenv
export PYENV_ROOT="pyenv"
export PATH="$PYENV_ROOT/bin":"$PATH"
eval "$(pyenv init -)"
pyenv install $PYVERSION
pyenv local $PYVERSION
export PYCMD="python"
export PIP="pip"
