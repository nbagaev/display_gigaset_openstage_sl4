//Arduino test sketch for Gigaset OpenStage SL4 Display
//Assuming the display controller is ILI9163V

#define exit_sleep_mode           0x11 //Sleep Out
#define enter_normal_mode         0x13 //Normal Display Mode On
#define exit_invert_mode          0x20 //Display inversion off
#define set_pixel_format          0x3A //Interface Pixel Format
#define set_address_mode          0x36 //This command defines read/write scanning direction of frame memory
#define set_column_address        0x2A //Column address set
#define set_page_address          0x2B //Row address set
#define gamma_adjustment_enable   0xF2 //Gamma adjustment enable control
#define display_inversion_control 0xB4 //Display Inversion Control
#define power_control_1           0xC0 //Power Control 1
#define power_control_3           0xC2 //Power Control 3
#define power_control_4           0xC3 //Power Control 4
#define power_control_5           0xC4 //Power Control 5
#define vcom_control_2            0xC6 //VCOM_Control 2
#define gamma_adjustment_plus     0xE0 //Gamma adjustment (+ polarity)
#define gamma_adjustment_minus    0xE1 //Gamma adjustment (- polarity)
#define set_display_on            0x29 //Display On
#define read_display_status       0x09 //Read Display Status
#define write_memory_start        0x2C //Memory Write

uint8_t pin_scl = 13;  //serial clock
uint8_t pin_sda = 11;  //data input
uint8_t pin_scx = 10;  //chip enable
uint8_t pin_dcx = 9;  //data/command
uint8_t pin_resx = 8; //reset


const uint8_t gamma_plus[] PROGMEM = {0x02,0x0B,0x16,0x2D,0x2B,0x13,0x15,0x06,0x06,0x04,0x0C,0x0C,0x01,0x05,0x03,0x07};
const uint8_t gamma_minus[] PROGMEM = {0x04,0x1C,0x1E,0x25,0x18,0x11,0x06,0x15,0x07,0x04,0x05,0x01,0x0C,0x0C,0x04,0x06};
        
void send_data(uint8_t bits)
{
  digitalWrite(pin_dcx, HIGH);//set D/C HIGH = data
  shiftOut(pin_sda, pin_scl, MSBFIRST, bits);//send data bits    
  digitalWrite(pin_dcx, LOW);//set D/C LOW = command
  digitalWrite(pin_sda, HIGH);
}

void send_command(uint8_t bits)
{
  shiftOut(pin_sda, pin_scl, MSBFIRST, bits);//send command bits
  digitalWrite(pin_sda, HIGH);
}

void setup()
{
  pinMode(pin_scl, OUTPUT);
  pinMode(pin_sda, OUTPUT);
  pinMode(pin_resx, OUTPUT);
  pinMode(pin_scx, OUTPUT);
  pinMode(pin_dcx, OUTPUT);
  //init 
  digitalWrite(pin_resx, HIGH);
  digitalWrite(pin_scl, LOW);
  digitalWrite(pin_sda, HIGH);
  digitalWrite(pin_scx, LOW);
  digitalWrite(pin_dcx, LOW);
  //reset 
  digitalWrite(pin_resx, LOW);
  delay(1679);
  digitalWrite(pin_resx, HIGH);
  delay(120);  
  send_command(exit_sleep_mode);
  delay(5);
  send_command(enter_normal_mode);
  
  send_command(exit_invert_mode);
  
  send_command(set_pixel_format);
  send_data(0x05);//16 bit/pixel  
  
  send_command(set_address_mode); 
  send_data(0xC0);//orientation, RGB...
  
  send_command(set_column_address);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x7F);//127
  
  send_command(set_page_address);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x9F);//159
  
  send_command(0xF0); //????????????????
  send_data(0x5A);//????????????
  
  send_command(gamma_adjustment_enable);
  send_data(0x00);//disable?
  
  send_command(display_inversion_control);
  send_data(0x07);//frame inversion in all modes

  send_command(power_control_1);//Power Control 1
  send_data(0x05);//GVDD=4.55V
  send_data(0x01);//VCI1=2.7V

  send_command(power_control_3);//Power Control 3
  send_data(0x04);//Amount of Current in Operational Amplifier=Medium High in normal mode/full colors

  send_command(power_control_4);//Power Control 4
  send_data(0x03);//Amount of Current in Operational Amplifier=Medium in Idle mode/8-colors

  send_command(power_control_5);//Power Control 5
  send_data(0x01);//Amount of Current in Operational Amplifier=Small in Partial mode/full-colors

  send_command(vcom_control_2);
  send_data(0x0A);//??????????????????
  
  send_command(gamma_adjustment_plus);//Gamma adjustment (+ polarity)
  for(uint8_t i = 0; i < 16; i++)
  {
    send_data(pgm_read_byte_near(gamma_plus + i));
  }

  send_command(gamma_adjustment_minus);//Gamma adjustment (- polarity)
  for(uint8_t i = 0; i < 16; i++)
  {
    send_data(pgm_read_byte_near(gamma_minus + i));
  }
   
  send_command(set_display_on);
  delay(180);
  //send_command(read_display_status);
  //dummy clock cycle
  //answer:0xE0,0x53,0x04,0x00

  send_command(set_page_address);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x9F);//159
  
  send_command(write_memory_start);
  //fill display black
  for(int i = 0; i < 20480; i++)//128*160
  {
    send_data(0x00); send_data(0x00);
  }
  
  //print char A
  const uint8_t a[] = {0x18, 0x24, 0x42, 0x81, 0xFF, 0x81, 0x81, 0x81};
  /*  
  00011000
  00100100
  01000010  
  10000001
  11111111
  10000001
  10000001
  10000001
  */
  uint8_t line;

  send_command(set_column_address);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x07);//7
  
  send_command(set_page_address);
  send_data(0x00);  send_data(0x00);//0
  send_data(0x00);  send_data(0x07);//7
  
  send_command(write_memory_start);
  
  for(uint8_t i = 0; i < 8; i++)
  {
    line = a[i];
    for(uint8_t j = 0; j < 8; j++)
    {
      if((line & 0x80) == 0x80)
      {
        send_data(0xFF); send_data(0xFF);        
      }
      else
      {
        send_data(0x00); send_data(0x00);
      }
      line <<= 1;
    }    
  }
}

void loop()
{

}