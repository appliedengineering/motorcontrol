#!/bin/bash

###################################
# Run using "source ./loadenv.sh" #
###################################

# Check if Python 3 is installed
if [ ! type "python" > /dev/null ]; then
    echo "Couldn't find python. Install python 3 first."
elif [ ! $(python -c"import sys; print(sys.version_info.major)") -eq 3 ]; then
	echo "The command 'python' is not running python 3 (probably python 2). Please install python 3 to run as 'python' instead."
else
    echo "Found python 3."
fi

# Check if pip is installed
if [ ! type "pip" > /dev/null ]; then
    echo "Couldn't find pip. Install pip for python 3 first."
fi

# Check if virtualenv is installed
if [ ! type "virtualenv" > /dev/null ]; then
    pip install virtualenv
fi

# Create virtualenv if does not exist 
if [ ! -d ".env" ]; then
    virtualenv .env
    source .env/bin/activate
    pip install -r requirements.txt
else
    source .env/bin/activate
fi

