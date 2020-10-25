#!/bin/bash

###################################
# Run using "source ./loadenv.sh" #
###################################

# Check if Python 3 is installed
if ! type "python3" > /dev/null; then
    echo "Couldn't find python. Install python 3 and pip 3 first."
elif [ ! $(python3 -c"import sys; print(sys.version_info.major)") -eq 3 ]; then
	echo "The command 'python3' is not running python 3 (probably python 2). Please install python 3 to run as 'python3' and pip 3 to run as 'pip3' instead."
else
    echo "Found python 3."
fi

# Check if pip is installed
if ! type "pip3" > /dev/null; then
    echo "Couldn't find pip. Install pip for python 3 first."
else
    echo "Found pip."
fi

# Check if virtualenv is installed
if ! type "virtualenv" > /dev/null; then
    apt install -y python3-virtualenv
else
    echo "Found virtualenv."
fi

# Create virtualenv if does not exist 
if [ ! -d ".env" ]; then
    virtualenv .env --system-site-packages
    source .env/bin/activate
    pip3 install -r requirements.txt
else
    source .env/bin/activate
fi

