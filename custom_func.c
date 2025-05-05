/*_____ I N C L U D E S ____________________________________________________*/
// #include <stdio.h>
#include <string.h>
#include "r_smc_entry.h"

#include "misc_config.h"
#include "custom_func.h"
#include "retarget.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

volatile struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TIMER_PERIOD_100MS                    (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_TIMER_PERIOD_SPECIFIC                 (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_CSIH0_SS0                             (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_CSIH0_SS1                             (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_CSIH0_SS2                             (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_TRIG_BTN1                             (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_TRIG_BTN2                             (flag_PROJ_CTL.bit7)

#define FLAG_PROJ_TRIG_1                                (flag_PROJ_CTL.bit8)
#define FLAG_PROJ_TRIG_2                                (flag_PROJ_CTL.bit9)
#define FLAG_PROJ_TRIG_3                                (flag_PROJ_CTL.bit10)
#define FLAG_PROJ_TRIG_4                                (flag_PROJ_CTL.bit11)
#define FLAG_PROJ_TRIG_5                                (flag_PROJ_CTL.bit12)
#define FLAG_PROJ_TRIG_6                                (flag_PROJ_CTL.bit13)
#define FLAG_PROJ_SPI_TX_END                            (flag_PROJ_CTL.bit14)
#define FLAG_PROJ_SPI_RX_END                            (flag_PROJ_CTL.bit15)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned short counter_tick = 0U;
volatile unsigned long ostmr_tick = 0U;
volatile unsigned long btn_counter_tick = 0U;

#define BTN_PRESSED_LONG                                (2500U)

#pragma section privateData

const unsigned char dummy_3 = 0x5AU;

volatile unsigned char dummy_2 = 0xFFU;

volatile unsigned char dummy_1;

#pragma section default

volatile unsigned long g_u32_counter = 0U;

unsigned char g_uart0rxbuf = 0U;                                 /* UART0 receive buffer */
unsigned char g_uart0rxerr = 0U;                                 /* UART0 receive error status */

#define SPI_LEN                                         (64U)
unsigned short t_buffer[SPI_LEN] = {0U};
unsigned short r_buffer[SPI_LEN] = {0U};

#define SS2_HIGH                                        (PORT.P8 |= 1U<<4U)
#define SS2_LOW                                         (PORT.P8 &= ~(1U<<4U))

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/
extern volatile uint32_t  g_taud0_ch0_width;

unsigned long btn_get_tick(void)
{
	return (btn_counter_tick);
}

void btn_set_tick(unsigned long t)
{
	btn_counter_tick = t;
}

void btn_tick_counter(void)
{
	btn_counter_tick++;
    if (btn_get_tick() >= 60000U)
    {
        btn_set_tick(0U);
    }
}

void ostmr_tick_counter(void)
{
	ostmr_tick++;
}

void ostmr_1ms_IRQ(void)
{
	ostmr_tick_counter();
}

void ostimer_dealyms(unsigned long ms)
{
    R_Config_OSTM0_Start();
    ostmr_tick = 0U;

    while(ostmr_tick < ms);

    R_Config_OSTM0_Stop();

}

unsigned short get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned short t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000U)
    {
        set_tick(0U);
    }
}

void delay_ms(unsigned long ms)
{
    unsigned long tickstart = get_tick();
    unsigned long wait = ms;
	unsigned long tmp = 0U;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000U -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
}


unsigned char is_spi_rx_end(void)
{
    return FLAG_PROJ_SPI_RX_END;
}

void spi_rx_end(unsigned char flag)
{
    FLAG_PROJ_SPI_RX_END = flag;
}


unsigned char is_spi_tx_end(void)
{
    return FLAG_PROJ_SPI_TX_END;
}

void spi_tx_end(unsigned char flag)
{
    FLAG_PROJ_SPI_TX_END = flag;
}

void SPI_init(void)
{
    spi_tx_end(0U);
    spi_rx_end(0U);
}

/*
    idx : 
    CSIH0_CS_CHIP_0 // SS0
    CSIH0_CS_CHIP_1 // SS1
    CSIH0_CS_CHIP_2 // GPIO
*/
void SPI_process(unsigned long idx)
{
    unsigned char i = 0U;
    unsigned char len = 0U;
    static unsigned char cnt0 = 0U;
    static unsigned char cnt1 = 0U;
    static unsigned char cnt2 = 0U;

    switch(idx)
    {
        case _CSIH_SELECT_CHIP_0:
            len = 32U;
            for(i = 0U ; i < len ; i++)
            {
                t_buffer[i] = i + cnt0;
            }

            t_buffer[0U] = 0x5AU;
            t_buffer[1U] = 0x5AU;
            t_buffer[len-2U] = 0xA5U;
            t_buffer[len-1U] = 0xA5U;

            spi_tx_end(0U);
            R_Config_CSIH0_Send_Receive((uint16_t*)t_buffer,len,(uint16_t*)r_buffer,_CSIH_SELECT_CHIP_0);
            while(!is_spi_tx_end());

            cnt0 += 0x10U;

            break;

        case _CSIH_SELECT_CHIP_1:
            len = 16U;
            for(i = 0U ; i < len ; i++)
            {
                t_buffer[i] = i + cnt1;
            }

            t_buffer[0U] = 0x5BU;
            t_buffer[1U] = 0x5BU;
            t_buffer[len-2U] = 0xB5U;
            t_buffer[len-1U] = 0xB5U;

            spi_tx_end(0U);
            R_Config_CSIH0_Send_Receive((uint16_t*)t_buffer,len,(uint16_t*)r_buffer,_CSIH_SELECT_CHIP_1);
            while(!is_spi_tx_end());

            cnt1 += 0x10U;

            break;

        case _CSIH_SELECT_CHIP_2:
            SS2_LOW;

            len = 8U;
            for(i = 0U ; i < len ; i++)
            {
                t_buffer[i] = i + cnt2;
            }

            t_buffer[0U] = 0x5CU;
            t_buffer[1U] = 0x5CU;
            t_buffer[len-2U] = 0xC5U;
            t_buffer[len-1U] = 0xC5U;

            while(CSIH0.STR0 & (_CSIH_TRANSFER_WORKING_STATE));//while(CSIH0.STR0 & (1U << 7U));
            spi_tx_end(0U);
            R_Config_CSIH0_Send_Receive((uint16_t*)t_buffer,len,(uint16_t*)r_buffer,_CSIH_SELECT_CHIP_2);
            while(!is_spi_tx_end());
            while(CSIH0.STR0 & (_CSIH_TRANSFER_WORKING_STATE));//while(CSIH0.STR0 & (1U << 7U));

            SS2_HIGH;

            cnt2 += 0x10U;

            break;
    }

}

void tmr_1ms_IRQ(void)
{
    tick_counter();

    if ((get_tick() % 1000U) == 0U)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 1U;
    }

    if ((get_tick() % 100U) == 0U)
    {
        FLAG_PROJ_TIMER_PERIOD_100MS = 1U;
    }

    if ((get_tick() % 50U) == 0U)
    {
        FLAG_PROJ_TIMER_PERIOD_SPECIFIC = 1U;
    }	

    Button_Process_long_counter();
}

void LED_Toggle(void)
{
    static unsigned char flag_gpio = 0U;
		
    GPIO_TOGGLE(0,14);//PORT.PNOT0 |= 1u<<14;
	
	if (!flag_gpio)
	{
		flag_gpio = 1U;
        GPIO_HIGH(P8,5);//PORT.P8 |= 1u<<5;
	}
	else
	{
		flag_gpio = 0U;
		GPIO_LOW(P8,5);//PORT.P8 &= ~(1u<<5);
	}	
}

void loop(void)
{
	// static unsigned long LOG1 = 0U;
    static unsigned char ss_idx = 0U;

    Button_Process_in_polling();

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0U;

        g_u32_counter++;
        LED_Toggle();   
        // tiny_printf("timer:%4d\r\n",LOG1++);
    }

    if (FLAG_PROJ_TIMER_PERIOD_100MS)
    {        
        FLAG_PROJ_TIMER_PERIOD_100MS = 0U;
        
        switch(ss_idx)
        {
            case 0:
                SPI_process(_CSIH_SELECT_CHIP_0);
                ss_idx = 1;
                break;
            case 1:
                SPI_process(_CSIH_SELECT_CHIP_1);
                ss_idx = 2;
                break;
            case 2:
                SPI_process(_CSIH_SELECT_CHIP_2);
                ss_idx = 0;
                break;
        }
    }

    if (FLAG_PROJ_TIMER_PERIOD_SPECIFIC)
    {
        FLAG_PROJ_TIMER_PERIOD_SPECIFIC = 0U;     

    }

}

// F1KM S1 EVB , P8_2/INTP6 , set both edge 
void Button_Process_long_counter(void)
{
    if (FLAG_PROJ_TRIG_BTN2)
    {
        btn_tick_counter();
    }
    else
    {
        btn_set_tick(0U);
    }
}

void Button_Process_in_polling(void)
{
    static unsigned char cnt = 0U;

    if (FLAG_PROJ_TRIG_BTN1)
    {
        FLAG_PROJ_TRIG_BTN1 = 0U;
        tiny_printf("BTN pressed(%d)\r\n",cnt);

        if (cnt == 0U)   //set both edge  , BTN pressed
        {
            FLAG_PROJ_TRIG_BTN2 = 1U;
        }
        else if (cnt == 1U)  //set both edge  , BTN released
        {
            FLAG_PROJ_TRIG_BTN2 = 0U;
        }

        cnt = (cnt >= 1U) ? (0U) : (cnt+1U) ;
    }

    if ((FLAG_PROJ_TRIG_BTN2 == 1U) && 
        (btn_get_tick() > BTN_PRESSED_LONG))
    {         
        tiny_printf("BTN pressed LONG\r\n");
        btn_set_tick(0U);
        FLAG_PROJ_TRIG_BTN2 = 0U;
    }
}

void Button_Process_in_IRQ(void)    
{
    FLAG_PROJ_TRIG_BTN1 = 1U;
}


void UARTx_ErrorCheckProcess(unsigned char err)
{
    if (err)          /* Check reception error */
    {   
        /* Reception error */
        switch(err)
        {
            case _UART_PARITY_ERROR_FLAG:   /* Parity error */
                tiny_printf("uart rx:Parity Error Flag\r\n");
                break;
            case _UART_FRAMING_ERROR_FLAG:  /* Framing error */
                tiny_printf("uart rx:Framing Error Flag\r\n");
                break;
            case _UART_OVERRUN_ERROR_FLAG:  /* Overrun error */
                tiny_printf("uart rx:Overrun Error Flag\r\n");
                break;
            case _UART_BIT_ERROR_FLAG:      /* Bit error */
                tiny_printf("uart rx:Bit Error Flag\r\n");
                break;
        }
        g_uart0rxerr = 0U;
    }
}

void UARTx_Process(unsigned char rxbuf)
{    
    if (rxbuf == 0x00U)
    {
        return;
    }

    if (rxbuf > 0x7FU)
    {
        tiny_printf("invalid command\r\n");
    }
    else
    {
        tiny_printf("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {
            case '1':
                FLAG_PROJ_TRIG_1 = 1U;
                break;
            case '2':
                FLAG_PROJ_TRIG_2 = 1U;
                break;
            case '3':
                FLAG_PROJ_TRIG_3 = 1U;
                break;
            case '4':
                FLAG_PROJ_TRIG_4 = 1U;
                break;
            case '5':
                FLAG_PROJ_TRIG_5 = 1U;
                break;
            case '6':
                FLAG_PROJ_TRIG_6 = 1U;
                break;

            case 'X':
            case 'x':
            case 'Z':
            case 'z':
                RH850_software_reset();
                break;

            default:       
                // exception
                break;                
        }
    }
}

void RH850_software_reset(void)
{
    uint32_t  reg32_value;

    reg32_value = 0x00000001UL;
    WPROTR.PROTCMD0 = _WRITE_PROTECT_COMMAND;
    RESCTL.SWRESA = reg32_value;
    RESCTL.SWRESA = (uint32_t) ~reg32_value;
    RESCTL.SWRESA = reg32_value;
    while (WPROTR.PROTS0 != reg32_value)
    {
        NOP();
    }
}

void RLIN3_UART_SendChar(unsigned char c)
{
    /*
        UTS : 0 - transmission is not in progress    
    */
    while (((RLN30.LST & _UART_TRANSMISSION_OPERATED) != 0U));    
    RLN30.LUTDR.UINT16 = c;
    // RLN30.LUTDR.UINT8[L] = (unsigned char) c;  
}

void SendChar(unsigned char ch)
{
    RLIN3_UART_SendChar(ch);
}

void hardware_init(void)
{
    EI();

    R_Config_TAUJ0_3_Start();
    R_Config_OSTM0_Start();

    /*
        LED : 
            - LED18 > P0_14
            - LED17 > P8_5 
        UART : 
            - TX > P10_10
            - RX > P10_9    
    */
    R_Config_UART0_Receive(&g_uart0rxbuf, 1U);
    R_Config_UART0_Start();
   
    /*
        button :
            - P8_2/INTP6
    */
    R_Config_INTC_INTP6_Start();
    
    /*
        CSIH
        SC:P0_2
        SI:P0_1 , Serial data input signal 
        SO:P0_3 , Serial data output signal 
        SS0:P8_0 , hardware SS control
        SS1:P8_3 , hardware SS control
        SS2:P8_4 , GPIO , default output HIGH
    */
    R_Config_CSIH0_Start();

    tiny_printf("\r\nhardware_init rdy\r\n");

}
