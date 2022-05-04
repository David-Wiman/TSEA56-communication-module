# Installation
install libraries:
```
sudo apt update
sudo apt install nlohmann-json3-dev
sudo apt install libboost-all-dev
```

Setup this repository:
```
git clone --recurse-submodules git@gitlab.liu.se:da-proj/TSEA56/2022/G08/communication-module.git
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

# Logs
Logs can be found in the `log` directory.

| Filename      | Description |
|---------------|-------------|
| `log.txt`     | General log |
| `i2c_log.txt` | Low level I2C log |

## Reading logs
Some usefull unix commands for reading logs:

Show the entire log:
```
cat log/log.txt
```

Show the log in real time (follow as it updates)
```
tail -f log/log.txt
```

Show the log, but only the lines that contains INFO
```
cat log/log.txt | grep "INFO"
# or
grep "INFO" log/log.txt
```

Show the log, but only the lines that contains ERROR, WARNING or INFO
```
grep -E "ERROR|WARNING|INFO" log/log.txt
```

Show the log, but highligt ERROR, WARNING and INFO
```
grep -E "ERROR|WARNING|INFO|" log/log.txt
```

Show the log in real time, but only the lines that contains ERROR, WARNING or INFO
```
tail -f log/log.txt | grep -E "ERROR|WARNING|INFO"
```
