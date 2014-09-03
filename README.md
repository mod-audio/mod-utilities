mod-utilities
=============

Some utilities lv2 plugins developed by mod team

Here you've got some Butterwoth filters,
some crossovers based on these filters,
simple gain plugins without zipper noise,
two switch box plugins, a switch trigger,
a toogle switch and a peakmeter.

Note2Midi is EXPERIMENTAL and it's NOT WORKING yet.

Installation:

	make
	sudo make install

You also can install only the plugins you want.
To do this, enter the folder of the effect that you want and type the following comands:

	make
	sudo make install

In this way, the default instalation path is /usr/local/lib/lv2/, and this can be modified passing the variable INSTALL_PATH to make install, e.g.:

	sudo make install INSTALL_PATH=/usr/lib/lv2

