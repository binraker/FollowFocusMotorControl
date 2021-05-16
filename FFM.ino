//Zhiyun crane2 FF motor (CMF-01) control
//For "reasons" they have used a micro-USB connecotr to send serial data. D+ = Tx, D- =Rx. make sure oyou also supply 5v as well otherwise it will power up but the motor wont turn.

//17 incs per tooth.
//24 tooth gear

//408 incs per rev

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
}



char dat[] = {0xA5, 0x5A, 0x0A, 0xFF, 0x0F, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0xB9, 0x49};
char *datptr = dat;

int calcrc(char *ptr, int count) //from http://web.mit.edu/6.115/www/amulet/xmodem.htm
{
    int  crc;
    char i;

    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}
char pos = 0;
byte pos2 = 0;

void sendPacket()
{
  int CRC;
  CRC = calcrc(datptr,13);
  dat[14] = (CRC>>8) & 0xFF;
  dat[13] = (CRC) & 0xFF;
  for (int i = 0;  i < 15 ; i++)
  {
    Serial.write(dat[i]);
  }
   
  delay(18);
}

void loop() {
  
  dat[10] = pos;
  sendPacket();

  pos2 +=4;
  if (pos2 == 0)
  {
    //delay(500);
    pos -=102 ;
  }

}
