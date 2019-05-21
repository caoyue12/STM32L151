
1. Run the "key_pair_generate.bat" script to generate a unique key pair. Including "dfu_public_key.c", "priv.pem" two files.
2. Open the official "secure_bootloader" routine and replace the key in the routine with the key from the "dfu_public_key.c" file. Modify "NRF_BL_DFU_ENTER_METHOD_BUTTON" to 0.
3. Compile and generate the firmware and rename it to "bootloader.hex". Copy to this folder.
4. In the "Makefile_Custome" file, configure "DFU_SUPPOR" to 1 (default is 0), compile the application firmware, and rename it to "app_original.hex". Copy to this folder.
5. Run the "product_file_generate.bat" script to generate the hex file "production_final.hex" containing SoftDevice, App, BootLoader and Setings. This file is downloaded to the chip via SWD and downloaded only once at the beginning. The subsequent firmware can be updated via the mobile terminal DFU.
6. When the application firmware is updated, rename the new firmware to "app_hew.hex" and copy it to this folder.
7. Run the "dfu_packet_generate.bat" script to generate an upgrade package "sdk15_dfu_s140.zip" for OTA DFU. Copy the upgrade package to the phone and use the "nRF Connect" tool for firmware upgrades.


Note: To test the DFU function during the development phase, follow steps 5 - 7. If the firmware is applied to a formal product, be sure to start from step 1 for safety.