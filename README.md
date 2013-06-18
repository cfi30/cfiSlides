cfiSlides 0.4
=========

Build instructions for Linux:
```
git clone https://bitbucket.org/cfi30/cfislides.git
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
