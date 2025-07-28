user_tft_config=$(cat tft_config.txt | tr '\n' ' ' | sed -e "s/\ /\ -D/g" -e "s/-D$//g")
tft_config="-DUSER_SETUP_LOADED=1 -D${user_tft_config} -DLOAD_GLCD=1 -DLOAD_FONT2=1 -DLOAD_FONT4=1 -DLOAD_FONT6=1 -DLOAD_FONT7=1 -DLOAD_FONT8=1 -DLOAD_GFXFF=1 -DSMOOTH_FONT=1 -DSPI_FREQUENCY=27000000 -DSPI_READ_FREQUENCY=20000000"
echo $tft_config
