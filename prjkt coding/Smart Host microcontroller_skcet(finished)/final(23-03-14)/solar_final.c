#include <16F877A.h>
#fuses NOWDT,HS,PUT, NOPROTECT,BROWNOUT, NOLVP, NOCPD, NOWRT
#device adc=10
#use delay(clock=11059200)

#byte portb=0x06
#byte portc=0x07
#byte portd=0x08
#byte porte=0x09

#byte trisb=0x86
#byte trisc=0x87
#byte trisd=0x88
#byte trise=0x89

#bit reg=portb.7
#bit rw=porte.1
#bit en=porte.2

#bit motor1_p=portb.0
#bit motor1_n=portb.1
#bit motor2_p=portb.2
#bit motor2_n=portb.3
#bit relay1=portb.4
#bit relay2=portb.5

#bit RELAY3=portc.0
#bit RELAY4=portc.1

unsigned int16 value1=0, value2=0, value3=0, value4=0, value5=0, value6=0;
unsigned char sec = 0, time = 0;
int1 flag=0 , glag = 0;
int z=0;

void display(unsigned int16);
void command(unsigned char com);
void data(unsigned char da);

void command(unsigned char com){
   portd=com;
   reg=0;
   rw=0;
   en=1;
   delay_ms(1);
   en=0;
}

void data(unsigned char da){
   portd=da;
   reg=1;
   rw=0;
   en=1;
   delay_ms(1);
   en=0;
}

#int_TIMER0
TIMER0_isr()
{
   time++;
   if(time>=43)
   {
      time=0;
      flag=1;
      glag=1;

   }
}

void main()
{
   portd=0x00;
   porte=0x00;

   trisd=0x00;
   trise=0x01;

   portb=0x00;
   trisb=0x00;

   portc=0x00;
   trisc=0x00;

   command(0x38); delay_ms(5);
   command(0x06); delay_ms(5);
   command(0x0c); delay_ms(5);
   command(0x01); delay_ms(5);

   command(0x80);
   data("SOLAR TRACKING");

   set_timer0(0x00); //set TMR0 initial value to zero
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256); //set timer0 mode

   setup_adc( ADC_CLOCK_INTERNAL );
   setup_adc_ports(AN0_AN1_AN2_AN3_AN4_AN5);

   enable_interrupts(INT_TIMER0); //enable timer overflow interrupt

   enable_interrupts(GLOBAL); //enable global interrupt


   delay_ms(2000);
   command(0x01);
   delay_ms(2);


   while(1)
   {

     if(glag == 1)
     {
      glag=0;
      set_adc_channel(0);
      delay_us(100);         //wait for the acquisition time
      value1=read_adc();
      delay_us(4);

      set_adc_channel(1);
      delay_us(100);         //wait for the acquisition time
      value2=read_adc();
      delay_us(4);

      set_adc_channel(2);
      delay_us(100);         //wait for the acquisition time
      value3=read_adc();
      delay_us(4);

      set_adc_channel(3);
      delay_us(100);         //wait for the acquisition time
      value4=read_adc();
      delay_us(40);

      set_adc_channel(4);
      delay_us(100);         //wait for the acquisition time
      value5=read_adc();
      value5=value5*2;
      delay_us(40);

      set_adc_channel(5);
      delay_us(100);         //wait for the acquisition time
      value6=read_adc();
      value6=value6*2;
      delay_us(40);
     }
      if((sec == 0) || (sec == 6))
      {
         delay_ms(50);
         command(0x01);
         delay_ms(2);
      }

      if(flag==1)
      {
         flag=0;
         sec++;
         if(sec >= 12)
            sec = 0;
      }

      if(sec < 6)
      {
         z=0;

         RELAY3=1;
         RELAY4=0;

         command(0x80);
         data("L1:");
         display(value1);
         command(0x88);
         data("L2:");
         display(value2);
         command(0xC0);
         data("L3:");
         display(value3);
         command(0xC8);
         data("L4:");
         display(value4);
      }

      else
      {
         z=1;

         RELAY3=0;
         RELAY4=1;

         command(0x80);
         data("BATTERY1 :");
         display(value5);
         command(0xC0);
         data("BATTERY2 :");
         display(value6);
      }

      if((value1+200) > value2)
      {
         motor1_p = 1;
         motor1_n = 0;

      }


      if((value2+200) > value1)
      {
         motor1_p = 0;
         motor1_n = 1;

      }


      if((value3+200) > value4)
      {
         motor2_p = 1;
         motor2_n = 0;
      }


      if((value4+200) > value3)
      {
         motor2_p = 0;
         motor2_n = 1;
      }


     if(((value1-value2) < 300) || ((value2-value1) < 300))
      {
         motor1_p = 0;
         motor1_n = 0;
      }


      if(((value3-value4) < 300) || ((value4-value3) < 300))
      {
         motor2_p = 0;
         motor2_n = 0;
      }


      if(((value5)+50) > (value6))
      {
         relay1=1;
         relay2=0;
      }


      if(((value6)+50) > (value5))
      {
         relay1=0;
         relay2=1;
      }

   }

}


void display(unsigned int16 n)
{
   int i = 0;
   int a[4];

   for(i=0;i<=3;i++)
   {
      a[i]=n%10;
      n=n/10;
   }

   data(a[3]|0x30);
   data(a[2]|0x30);
   data(a[1]|0x30);
   data(a[0]|0x30);

   if(z==1)
   {
   command(0x8c);
   data('.');
   command(0xcc);
   data('.');
   }
   delay_ms(300);
}
