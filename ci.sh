#!/bin/sh
set -o errexit
set -o verbose


  #####################################
  #               SETUP               #
  #####################################

sudo apt-get update -y

# Needed for unit testing with qt https://github.com/pytest-dev/pytest-qt/issues/293
sudo apt-get install -y xvfb libxkbcommon-x11-0
sudo Xvfb :1 -screen 0 1024x768x24 </dev/null &
export DISPLAY=":1"

# Start setting up Python for GS
sudo apt-get install -y tk-dev
env PYTHON_CONFIGURE_OPTS="--enable-shared" pyenv install 3.7.9
pyenv global 3.7.9
python --version
python -m pip install --upgrade pip setuptools wheel


# Clone and setup GS for SIM based integration tests
cd ..
git clone https://github.com/UBC-Rocket/UBCRocketGroundStation.git
cd UBCRocketGroundStation
cat ../FLARE/required_ubcrgs.txt | xargs git checkout
python build.py --only setup
cd ..

# PlatformIO
pip install -U platformio
pio update


  #####################################
  #               TEST                #
  #####################################

cd FLARE

# PlatformIO build
pio run

# Build and unittest on x86
mkdir avionics/build
cd avionics/build
cmake ..
cmake --build .
ctest --progress

# SIM Integration tests
cd ../../../UBCRocketGroundStation
source venv/bin/activate
python -m pytest tests/integration_tests/test_sim.py
deactivate
