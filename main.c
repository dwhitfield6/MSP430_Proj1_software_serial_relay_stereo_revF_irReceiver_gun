#include "msp430g2553.h"

 #define Relay BIT3
#define LED1 BIT0
#define LED2 BIT6
#define switch1 BIT4
#define switch2 BIT5
#define switch3 BIT0
#define IRrec BIT1
#define IRrec1 BIT2
                                                       // Hardware-related definitions
#define UART_TXD 0x02                                  // TXD on P1.1 (Timer0_A.OUT0)
#define UART_RXD 0x04                                  // RXD on P1.2 (Timer0_A.CCI1A)

#define UART_TBIT_DIV_2 (1000000 / (9600 * 2))         // Conditions for 9600 Baud SW UART, SMCLK = 1MHz
#define UART_TBIT (1000000 / 9600)

int delayswnum = 50;
int switchBuff1[20];
int switchBuff2[20];
int switchBuff3[20];
int i=0;
int ii=0;


// Globals for full-duplex UART communication
unsigned int txData;                                   // UART internal variable for TX
unsigned char rxBuffer;                                // Received UART character
unsigned int BlueData =0;

int switchh1 = 0;
int switchh2=0;
int switchh3=0;
int oldswitch1=0;
int oldswitch2=0;
int oldswitch3=0;
int count=0;
int loopcount =0;
int loopcountMAX =0;
int resetcount=0;
int bit =0;
int checksum =0;
int relayvalue =990;
int relayswitchvalue =0;
int switch3countoff =990;
int switch3counton =990;
int switch2countoff =990;
int switch2counton =990;
int switch1countoff =990;
int switch1counton =990;
int Bluenew =990;
int led1value =990;
int led2value =990;
int one =0;
int two=0;
int three =0;
int four =0;
int five =0;
int six =0;
int seven =0;
int eight =0;
int odd =0;



static volatile unsigned long data = 0;
static volatile unsigned long realdata = 0;
static volatile unsigned long nowdata = 0;
static volatile unsigned long oldrealdata = 0;

unsigned long rxdata = 0;

void TimerA_UART_tx(unsigned char byte);               // Function prototypes
void TimerA_UART_print(char *string);

void main(void){

  WDTCTL = WDTPW + WDTHOLD;                            // Stop watchdog timer
                                       // Initialize all GPIO
  P1SEL |= UART_TXD + UART_RXD;
  P1REN |= (switch1 + switch2);
  P2REN |= switch3;
  P1DIR |= (UART_TXD + LED1 + LED2);                            // Set all pins but RXD to output
  P2DIR |= (Relay);
P1OUT |= (switch1 + switch2);
P2OUT |= (switch3);

  TA0CCTL0 = OUT;                                      // Set TXD Idle as Mark = '1'
  TA0CCTL1 = SCS + CM1 + CAP + CCIE;                   // Sync, Neg Edge, Capture, Int
  TA0CTL = TASSEL_2 + MC_2;                            // SMCLK, start in continuous mode

  TA1CCTL0 |= CCIE;                             // CCR0 interrupt enabled
  TA1CTL = TASSEL_2 + MC_1 + ID_2;           // SMCLK/8, upmode
  TA1CCR0 =  20;

  P2IE |= IRrec; // P1.3 interrupt enabled
  P2IES &= ~IRrec; // P1.3 interrupt enabled
  P2IFG &= ~IRrec; // P1.3 IFG cleared
  P2IE |= IRrec1; // P1.3 interrupt enabled
  P2IES |= IRrec1; // P1.3 interrupt enabled
  P2IFG &= ~IRrec1; // P1.3 IFG cleared

  __enable_interrupt(); // enable all interrupts

  TimerA_UART_print("G2553 TimerA UART\r\n");          // Send test message
  TimerA_UART_print("READY.\r\n");

  while(1)
  {

    BlueData = rxBuffer;
    if(resetcount == 200)
       {
         loopcountMAX=loopcount;
         loopcount =0;
         realdata = rxdata >> 1;
         _delay_cycles(100);
       }
  	if(resetcount>1000)
		{
			resetcount =990;
		}

  	if(realdata == 0x0E7152AD) //gun code
  	{
  		relayvalue=10;
  		relayswitchvalue =1;
  		realdata =0;
  	}
  	if((P2IN & switch3) == switch3)
  	{
  		switchBuff3[i] = 1;
  	}
  	else
  	{
  		switchBuff3[i] = 0;
  	}

  	if((P1IN & switch1) == switch1)
  	{
  		switchBuff1[i] = 1;
  	}
  	else
  	{
  		switchBuff1[i] = 0;
  	}

  	if((P1IN & switch2) == switch2)
  	{
  		switchBuff2[i] = 1;
  	}
  	else
  	{
  		switchBuff2[i] = 0;
  	}

  	i++;
  	if(i>18)
  	{
  		i=0;
  	}
switch3counton =0;
switch3countoff =0;
switch2counton =0;
switch2countoff =0;
switch1counton =0;
switch1countoff =0;
  	for(ii=0;ii<19;ii++)
  	{
  		if(switchBuff3[ii] == 1)
  		{
  			switch3counton++;
  		}
  		else
  		{
  			switch3countoff++;
  		}
  		if(switchBuff2[ii] == 1)
  		{
  			switch2counton++;
  		}
  		else
  		{
  			switch2countoff++;
  		}
  		if(switchBuff1[ii] == 1)
  		{
  			switch1counton++;
  		}
  		else
  		{
  			switch1countoff++;
  		}
  	}
  	if(switch3counton > 17)
  	{
  		switchh3 = 1;
  	}
  	else if (switch3countoff > 17)
  	{
  		switchh3 = 0;
  	}
  	if(oldswitch3 != switchh3)
  	{
  		relayvalue =0;
  		relayswitchvalue = 1;
  		led1value =0;
  		led2value =0;
  	}

  	if(switch2counton > 17)
  	{
  		switchh2 = 1;
  	}
  	else if (switch2countoff > 17)
  	{
  		switchh2 = 0;
  	}
  	if(oldswitch2 != switchh2)
  	{
  		relayvalue =0;
  		relayswitchvalue = 1;
  		led1value =0;
  		led2value =0;
  	}

  	if(switch1counton > 17)
  	{
  		switchh1 = 1;
  	}
  	else if (switch1countoff > 17)
  	{
  		switchh1 = 0;
  	}
  	if(oldswitch1 != switchh1)
  	{
  		relayvalue =0;
  		relayswitchvalue = 1;
  		led1value =0;
  		led2value =0;
  	}
  	if(oldswitch2 != switchh2)
  	{
  		relayvalue =0;
  		relayswitchvalue = 1;
  		led1value =0;
  		led2value =0;
  	}
  	if(oldswitch3 != switchh3)
  	{
  		relayvalue =0;
  		relayswitchvalue = 1;
  		led1value =0;
  		led2value =0;
  	}

  	_delay_cycles(20);

  	if(relayvalue == 50 && relayswitchvalue == 1)
  	{
  		P2OUT ^= Relay;
  		relayswitchvalue = 0;
  	}
  	relayvalue++;
  	if(relayvalue > 1000)
  	{
  		relayvalue =990;
  	}

  	if(Bluenew == 20)
  	{
  		if(BlueData == 25)
  		{
  	 		relayvalue =0;
  	  		relayswitchvalue = 1;
  	  		led2value =0;
  		}
  	}
  	if(led1value <500)
  	{
  		P1OUT |= LED1;
  	}
  	else
  	{
  		P1OUT &= ~LED1;
  	}
  	if(led2value <500)
  	{
  		P1OUT |= LED2;
  	}
  	else
  	{
  		P1OUT &= ~LED2;
  	}
  	led1value++;
  	if(led1value >1000)
  	{
  		led1value =990;
  	}
 	led2value++;
  	if(led2value >1000)
  	{
  		led2value =990;
  	}
  	Bluenew++;
  	if(Bluenew >1000)
  	{
  		Bluenew =990;
  	}
    resetcount++;
    if(resetcount >1000)
    {
      resetcount =999;
    }
    if(count >1000)
    {
      count =999;
    }

 	oldswitch1 = switchh1;
 	oldswitch2 = switchh2;
 	oldswitch3 = switchh3;
  	oldrealdata = realdata;
  }
}


void TimerA_UART_tx(unsigned char byte) {              // Outputs one byte using the Timer_A UART


  while (TACCTL0 & CCIE);                              // Ensure last char got TX'd

  TA0CCR0 = TAR;                                       // Current state of TA counter

  TA0CCR0 += UART_TBIT;                                // One bit time till first bit

  TA0CCTL0 = OUTMOD0 + CCIE;                           // Set TXD on EQU0, Int

  txData = byte;                                       // Load global variable

  txData |= 0x100;                                     // Add mark stop bit to TXData

  txData <<= 1;                                        // Add space start bit


}


void TimerA_UART_print(char *string) {                 // Prints a string using the Timer_A UART

  while (*string)
    TimerA_UART_tx(*string++);
}


#pragma vector = TIMER0_A0_VECTOR                      // Timer_A UART - Transmit Interrupt Handler

   __interrupt void Timer_A0_ISR(void) {

  static unsigned char txBitCnt = 10;
 // TA1CCTL0 = ~CCIE;
  TA0CCR0 += UART_TBIT;                                // Add Offset to CCRx

  if (txBitCnt == 0) {                                 // All bits TXed?

    TA0CCTL0 &= ~CCIE;                                 // All bits TXed, disable interrupt

    txBitCnt = 10;                                     // Re-load bit counter
  }
  else {
    if (txData & 0x01)
      TA0CCTL0 &= ~OUTMOD2;                            // TX Mark '1'
    else
      TA0CCTL0 |= OUTMOD2;                             // TX Space '0'
  }
  txData >>= 1;                                        // Shift right 1 bit
  txBitCnt--;
 // TA1CCTL0 = CCIE;
}


#pragma vector = TIMER0_A1_VECTOR                      // Timer_A UART - Receive Interrupt Handler

  __interrupt void Timer_A1_ISR(void) {

  static unsigned char rxBitCnt = 8;

  static unsigned char rxData = 0;

  switch (__even_in_range(TA0IV, TA0IV_TAIFG)) {       // Use calculated branching

    case TA0IV_TACCR1:                                 // TACCR1 CCIFG - UART RX

         TA0CCR1 += UART_TBIT;                         // Add Offset to CCRx

         if (TA0CCTL1 & CAP) {                         // Capture mode = start bit edge

           TA0CCTL1 &= ~CAP;                           // Switch capture to compare mode

           TA0CCR1 += UART_TBIT_DIV_2;                 // Point CCRx to middle of D0
         }
         else {
           rxData >>= 1;

           if (TA0CCTL1 & SCCI)                        // Get bit waiting in receive latch
             rxData |= 0x80;
           rxBitCnt--;

           if (rxBitCnt == 0) {                        // All bits RXed?

             rxBuffer = rxData;                        // Store in global variable

             rxBitCnt = 8;                             // Re-load bit counter

             TA0CCTL1 |= CAP;                          // Switch compare to capture mode

           }
         }
         break;
   }
  Bluenew =0;
}


#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0 (void)
{
  count++;
}



#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
 // P1OUT ^= debug;
  TA1CTL |= TACLR;

  if(loopcount == 0)
  {
    rxdata =0;
  }
  if (loopcount == 50)
  {
    one = count;
  }
  else if (loopcount ==51)
  {
    two = count;
  }
  else if(loopcount ==52)
  {
    three = count;
  }
  else if(loopcount ==53)
  {
    four = count;
  }
  else if(loopcount == 54)
  {
    five = count;

  }
  else if(loopcount == 55)
  {
    six = count;

  }
  else if(loopcount == 56)
  {
    seven = count;

  }
  else if(loopcount == 57)
  {
    eight = count;

  }

  odd = (loopcount & 0x1);
  if(odd == 0)
  {
    	if(count >13 && count < 24)
    		{
    			bit = 1;
    		}
    	else
    	{
			bit = 0;
    	}

    		    rxdata += bit;
    		    rxdata<<=1;
  }

loopcount++;
  count=0;
  resetcount=0;
  P2IFG &= ~IRrec; // P1.3 IFG cleared
  P2IFG &= ~IRrec1; // P1.3 IFG cleared
}










