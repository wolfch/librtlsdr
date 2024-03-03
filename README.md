# librtlsdr
Turns your Realtek RTL2832 based DVB dongle into a SDR receiver

## Building with CMake

```console
cd librtlsdr/
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
```

## Building with autotools

```console
cd librtlsdr/
autoreconf -i
./configure
make
sudo make install
sudo ldconfig
```

## After building executables

The built executables (`rtl_sdr`, `rtl_tcp`, `rtl_test`, etc.) can be found in 
`librtlsdr/build/src/`.

In order to be able to use the dongle as a non-root user, you may install the appropriate udev rules file by calling:

```console
sudo make install-udev-rules
```

