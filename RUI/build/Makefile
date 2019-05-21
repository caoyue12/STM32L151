#RUI Version:2.0
#numbers of supported mcu is from 1 to 9
#numbers of supported connectivity chips is from 1-9,a-z,A-Z, at most 61
#numbers of supported sensors is from 1-9,a-z,A-Z, at most 61


PROJECT_NAME     := RUI
SDK_ROOT      := ../..
P :=
NUM := $(words $(P))

ifeq (1, $(word 1, $(P)))
    include nordic/Makefile_nRF
else ifeq (2, $(word 1, $(P)))
    include nordic/Makefile_nRF
else ifeq (3, $(word 1, $(P)))
    include stm/Makefile_stm32l151cb
else ifeq (4, $(word 1, $(P)))
    include stm/Makefile_stm32l073rz
endif

.PHONY: clean help

# clean target
clean:
	@find $(SDK_ROOT)/RUI/Source/ -name "*.o" | xargs rm -f
	rm -rf _build

# Print all targets that can be built
help:
	@echo	-e	\
"  ************************************************************************\n \
 *        \033[33;1m*****   *   *   *\033[0m                                             *\n  \
*        \033[33;1m*   *   *   *   *\033[0m                                             *\n  \
*        \033[33;1m*****   *   *   * \033[0m                                            *\n  \
*       \033[33;1m * *     *   *   *   Rakwirelss Unified Interface\033[0m              *\n  \
*        \033[33;1m*  *    *   *   *           Expanding Customization !!\033[0m        *\n  \
*        \033[33;1m*   *   *****   *                           ---From 2018\033[0m      *\n  \
************************************************************************\n  \
*  \033[32m1. Overview   \033[0m                                                      *\n  \
*  RUI is an open platform based on nordic low power chip, including   *\n  \
*  source code and compiler with gcc, designed by Rakwirelss. Besides, *\n  \
*  platform not only provides corresponding code suit for Rakwireless  *\n  \
*  hardware board, but also supports user-defined board, which is the  *\n  \
*  most attractive feature. A sensor drivers libraries is supplied and *\n  \
*  user just chooses the sensor they need and change the hardware pin  *\n  \
*  parameter in header file. Now RUI just includes nRF52832, more chip *\n  \
*  will be added gradually. RUI support gcc compile in windows,linux,  *\n  \
*  OS X, based SDK 15.0.0. So it is important to install toolchain and *\n  \
*  more details in README.                                             *\n  \
*                                                                      *\n  \
*  \033[32m2. How to use\033[0m                                                       *\n  \
*  Download SDK, put RUI in the root directory of sdk, then enter to   *\n  \
*   ../RUI/build/ and choose chip and sensor supplied below            *\n  \
*                                                                      *\n  \
*  \033[36mComputing\033[0m      * \033[35mConnectivity\033[0m    * \033[33mSensor\033[0m                           *\n  \
*                 *                 *                                  *\n  \
*  \033[36m1. nRF52832\033[0m    * \033[35m1. bc95-g\033[0m       * \033[33m1. bme280   2. L_70_R\033[0m            *\n  \
*  \033[36m2. nRF52840\033[0m    * \033[35m2. bg96\033[0m         * \033[33m3. lis2mdl  4. lis3dh\033[0m            *\n  \
*  \033[36m3. stm32L151CB\033[0m * \033[35m3. m35\033[0m          * \033[33m5. opt3001\033[0m  \033[33m6. sht31\033[0m             *\n  \
*  \033[36m4. stm32l073rz\033[0m * \033[35m4. sx1276(81x)\033[0m  * \033[33m7. ublox-max7  8. shtc3\033[0m          * \n  \
*                 * \033[35m5. sx1276(4600)\033[0m * \033[33m9. lps22hb\033[0m                       * \n  \
*                                                                      *\n  \
*  \033[32m3. Compile command  \033[0m                                                *\n  \
*                                                                      *\n  \
*  -- \033[31mmake clean\033[0m  -- clean all targets                                 *\n  \
*                                                                      *\n  \
*  -- \033[31mmake P=\"1 2 3 5\"\033[0m  -- mcu(1) commu(2) sensor(3 5 ...)             *\n  \
*  eg Rak_8212 compile cmd \033[31mmake P=\"1 2 1 3 4 5\"\033[0m                        *\n  \
*  choose the corresponding sensor and remember to change pin number   *\n  \
*  in RUI/Source/config/XXXX_board/pin_define.h                        *\n  \
*                                                                      *\n  \
*  -- \033[31mmake help\033[0m -- show RUI description                                *\n  \
*                                                                      *\n  \
*  \033[32m4. Custome\033[0m                                                          *\n  \
*  LoRaWAN and sleep mode for all products have been added in RUI.     *\n  \
*  More features will be added further, so users can change in the     *\n  \
*  \033[31mMakefile_Custome\033[0m according to need. More details in RUI\doc\        *\n  \
************************************************************************"   \