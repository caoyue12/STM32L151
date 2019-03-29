Welcome to RUI, which supply you with a suitable development environment for RAK product or your own board!
Now RUI supports several platforms like: nRF52832, nRF52840, stm32L151CB, and many sensors like acc, LTE...

1. Install gcc toolchain, more details in RUI/doc/

2. Nordic develop
Download the SDK according to your board like below:
	2.1 nRF52832
	SDK version: nRF5_SDK_15.0.0_a53641a
	softdevice: s132_nrf52_6.0.0_softdevice.hex(in RUI/build/nordic/hex/)
	2.2 nRF52840
	SDK version: nRF5_SDK_15.2.0_9412b96
	softdevice: s140_nrf52_6.1.1_softdevice.hex(in RUI/build/nordic/hex/)
	
Then put RUI in the root directory of sdk, enter RUI/build/ , execute make help, it will show how to compile

3. STM32 develop

Enter RUI/build/ , execute make help, it will show how to compile

4. Directory introduction 
	
	RUI
	|———————build
	|         |————————nordic    				Nordic gcc makefile script
    |		  |————————stm		 				stm32 gcc makefile script
	|		  |————————MakeFile  				Root makefile
	|		  |————————MakeFile_Custome         user features define makefile
	|———————doc									RUI details doc
	|———————Source
	|		  |————————external					external lib or src like LoRaWAN
    |		  |————————stm		 				stm32 source code (same as below)
	|		  |————————nordic  				    nordic source code	
	|					 |————————app
	|					 |————————board
	|					 |————————config
	|					 |————————driver
    |					 |————————hal
	|					 |————————driver
    |					 |————————include
	|					 |————————service
	|					 |————————driver
    |					 |————————main.c
	|———————tools	