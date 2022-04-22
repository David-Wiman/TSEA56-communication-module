# Installation
install json library:
```
sudo apt install nlohmann-json3-dev
```

Setup this repository:
```
git clone --recursive-submodules git@gitlab.liu.se:da-proj/TSEA56/2022/G08/communication-module.git
```
or
```
git clone git@gitlab.liu.se:da-proj/TSEA56/2022/G08/communication-module.git
git submodule update --init --recursive
```

Build and run:
```
make -j4
./main.out
```

## User groups
If a user other than pi wants to run this program they have to be in some groups.
```
sudo adduser USERNAME gpio
sudo adduser USERNAME i2c
sudo adduser USERNAME video
```
