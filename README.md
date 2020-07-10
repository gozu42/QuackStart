
# QuackStart

how to get up and running with some [clusterducks](https://github.com/Code-and-Response/ClusterDuck-Protocol) with least effort.



## PREREQs

* any "esp32 with a lora chip" boards should do. 
* * check [platformio.ini](platformio.ini) for existing board definitions.

* assumes you have some kind of nixoid buildhost you know how to use.
* used examples are fedora and raspi.
* anything linux or mac should work, including linux-on-a-windows vms.


### serial port access

* google a distro specific guide.
* generally mucking around with udev and/or regular unix permission management.


#### fedora

```
sudo usermod -a -G dialout user
```




## install PIO

https://docs.platformio.org/en/latest/core/installation.html#installer-script


### install PIO on fedora

```
wget https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -O get-platformio.py
python3 get-platformio.py

echo 'export PATH="$PATH:~/.platformio/penv/bin"' >> ~/.bashrc
export PATH="$PATH:~/.platformio/penv/bin"
```


## install QS + CDP + RL

```
git clone https://github.com/gozu42/QuackStart
cd QuackStart

pio lib -g install https://github.com/jgromes/RadioLib
pio lib -g install https://github.com/Code-and-Response/ClusterDuck-Protocol
```

### build

```
pio run -e heltec_wifi_lora_32_V2
```

### install

```
pio run -e heltec_wifi_lora_32_V2 -t upload --upload-port /dev/ttyUSB0
```


