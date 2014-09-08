## Software Defined Radio Notes

Using a Cinergy TStick+ (0ccd:00d7) with Elonics E4000 (RTL2832U) chipset.

## Software Install

    git clone https://github.com/steve-m/librtlsdr
    cd librtlsdr
    mkdir build
    cd build
    cmake ../ -DINSTALL_UDEV_RULES=ON
    make
    sudo make install
    sudo ldconfig

Make sure everything is working: ``rtl_test -t``

Tuning a radio statio (ex: KQED 88.5 FM): ``rtl_fm -f 88.5e6 -M wbfm -s 200000 -r 48000 - | aplay -r 48k -f S16_LE``

## Other software

    sudo apt-get install gnuradio-dev gqrx-sdr

## Links

 * http://sdr.osmocom.org/trac/wiki/rtl-sdr
 * http://www.superkuh.com/rtlsdr.html
