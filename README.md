cfiSlides 0.5
=========

Build instructions for Linux:

```sh
git clone https://github.com/cfi30/cfiSlides.git
cd cfislides
qmake-qt5 CONFIG+=silent
make && cd build/
chmod +x ./cfislides.sh

# launch cfiSlides
./cfislides.sh
```

cfiSlides is tested on both Linux and Windows (MinGW).

Requirements
------------

cfiSlides requires Qt5 with the multimedia module.
