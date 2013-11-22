mod-utilities
=============

Some utilities lv2 plugins developed by mod team

Here you've got some Butterwoth filters,
some crossovers based on these filters,
a simple gain plugin without zipper noise and
a switch box plugin.

Installation:

	./make.sh
	sudo ./make_install.sh
	
You also can install only the plugins you want.
To do this, enter the folder of the effect that you want and type the following comands:

	make
	sudo make install

In this way, the default instalation path is /usr/local/lib/lv2/, and this can be modified passing the variable INSTALL_PATH to make install, e.g.:

	sudo make install INSTALL_PATH=/usr/lib/lv2

