# Fast write microphone data in queue (Redis)

- Read data from UART
- Write to Redis


## Install prerequisites

### CMake

```bash
$ sudo apt install cmake
```

### Install hiredis

```bash
git clone https://github.com/redis/hiredis.git
cd hiredis
make
sudo make install
```

### Install redis-plus-plus

```bash
git clone https://github.com/sewenew/redis-plus-plus.git
cd redis-plus-plus
mkdir build
cd build
cmake ..
make
sudo make install
```
