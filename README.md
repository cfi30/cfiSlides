cfiSlides 0.4
=========

Build instructions for Linux:
```
git clone https://github.com/cfi30/cfiSlides.git
cd cfiSlides
qmake-qt4 CONFIG+=silent
make && cd build/
chmod +x ./cfiSlides.sh

# launch cfiSlides
./cfiSlides.sh
```
cfiSlides is tested on both Linux and Windows.

Requirements
------------

- Qt4
- MinGW (Windows only)
- phonon devel librairies
