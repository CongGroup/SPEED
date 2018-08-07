all:
	cd speed_client && make clean && make
	cd speed_server && make clean && make
	