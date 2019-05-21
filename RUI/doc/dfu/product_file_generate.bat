nrfutil settings generate --family NRF52840 --application app_original.hex --application-version 1 --bootloader-version 2 --bl-settings-version 1 settings.hex

mergehex.exe -m bootloader.hex settings.hex -o bootloader_merge.hex
mergehex.exe -m s140_nrf52_6.1.0_softdevice.hex app_original.hex bootloader_merge.hex -o production_final.hex
del settings.hex /s
del bootloader_merge.hex /s

pause