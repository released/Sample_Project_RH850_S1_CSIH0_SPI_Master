# Sample_Project_RH850_S1_CSIH0_SPI_Master
Sample_Project_RH850_S1_CSIH0_SPI_Master


update @ 2025/05/05

1. base on EVM : Y-BLDC-SK-RH850F1KM-S1-V2 , initial below function

- TAUJ0_0 : timer interval for 1ms interrupt

- UART : RLIN3 (TX > P10_10 , RX > P10_9) , for printf and receive from keyboard

- LED : LED18 > P0_14 , LED17 > P8_5 , toggle per 1000ms

- CSIH0 : 

        - SC:P0_2
		
        - SI:P0_1 , Serial data input signal 
		
        - SO:P0_3 , Serial data output signal 
		
        - SS0:P8_0 , hardware SS control
		
        - SS1:P8_3 , hardware SS control
		
        - SS2:P8_4 , GPIO SS control , default output HIGH


3. below is smart configurator CSIH0 setting

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/smc_CSIH0.jpg)


3. scenario : 

- change SPI chip select port and transmit data per 100 ms

- SS0 port , 

	- hardware control SS0

	- len is 32

	- header index 0/1:0x5A 
	
	- tail last byte/last byte -1:0xA5


below is smart configurator CSIH0 , about SS0 setting

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/smc_SS0.jpg)


below is LA capture (CH0:SS0,CH1:CLK,CH2:MOSI,CH3:MISO)

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/LA_SS0.jpg)

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/LA_SS0_CLK.jpg)


- SS1 port , 

	- hardware control SS1

	- len is 16 
	
	- header index 0/1:0x5B , 
	
	- tail last byte/last byte -1:0xB5

below is smart configurator CSIH0 , about SS1 setting

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/smc_SS1.jpg)


below is LA capture (CH4:SS1,CH1:CLK,CH2:MOSI,CH3:MISO)

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/LA_SS1.jpg)

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/LA_SS1_CLK.jpg)


- SS2 port , 

	- use GPIO to control SS2

	- len is 8
	
	- header index 0/1:0x5C 
	
	- tail last byte/last byte -1:0xC5

below is smart configurator CSIH0 , about SS2 setting

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/smc_SS2.jpg)


below is LA capture (CH5:SS2,CH1:CLK,CH2:MOSI,CH3:MISO)

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/LA_SS2.jpg)

![image](https://github.com/released/Sample_Project_RH850_S1_CSIH0_SPI_Master/blob/main/LA_SS2_CLK.jpg)



