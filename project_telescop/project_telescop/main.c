/*
   project_telescop.c

   Created: 07.05.2021 10:38:52
   Author : user
*/

#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

void koord(double *A, double *h, double vr, double a, double l); //функция возвращает азимут А и высоту h звезды
void povorot(double A, double h, int mod);			//выполняет поворот двигателей на заданное колличество шагов
													//определяем mod(1, 2, 4, 8, 16, 32)-режим работы двигателя
void revers(double A, double h, int mod);			//возврат двигателя

void port_init(void);								//инициализация портов
void uart_init(void);								//UART
char* uart_read_com(void);							//
void circles_m1(uint16_t n, uint8_t rl);			//поворот ппервым и
void circles_m2(uint16_t n, uint8_t rl);			//вторым моторами

int64_t n1, n2, n12, time, a, l;
int32_t b = 131072, vr;
uint8_t md = 1, fl=0;
double B, C, t, s;
char *str = NULL;


int main(void) {
  port_init();
  uart_init();
  char* sr;
  while (1)
  {
    sr = uart_read_com();
	
	if(!strcmp(sr, "1")) {
		circles_m2(30, 1);
	}
	else if(!strcmp(sr, "2")) {
		circles_m2(30, 0);
	}
	else if(!strcmp(sr, "3")) {
		circles_m1(30, 1);
	}
	else if(!strcmp(sr, "4")) {
		circles_m1(30, 0);
	}
	else if(!strcmp(sr, "-")) {
		if(fl) 
		sr = uart_read_com();
		a = atof(sr);
		sr = uart_read_com();
		l = atof(sr);
		sr = uart_read_com();
		time = atof(sr);
		koord(&B, &C, time, a, l);
		povorot(B, C, md);
		
	}
  }
}

//инициализация портов для двигателей
void port_init(void) {
	DDRA =  0b11111111;     //первый двигатель
	PORTA = 0b00001111;		//

	DDRC = 0b11111111;		//второй двигатель
	DDRD = 0b11111111;		//
}

//инициализация UART
void uart_init(void) {					 
	UBRRL = 51;
	UCSRB |= (1 << RXEN) | (1 << TXEN);
}

unsigned char getch_Uart(void)  //  Получение байта
{
    while(!(UCSRA&(1<<RXC)));   //  Устанавливается, когда регистр свободен
    return UDR;
}

//функция для чтения байт
char* uart_read_com(void) {
    str=NULL;
	int i=0;
	char temp;
	while((temp=getch_Uart())!=43){                       //стоп символ "+", в ASCII №43
	  str = (char*)realloc(str, (++i) * sizeof(char));
	  str[i-1] = temp;
	}
	str = (char*)realloc(str, (++i) * sizeof(char));
	str[i-1]=0;
	return str;
}

//перевод в нужные координаты
void koord(double *A, double *h, double vr, double a, double l) {
  //uint32_t dn = 17, h0 = 18, min0 = 22, sec0 = 10 ;    //инициализируем сколько времени прошло после 2021.05.29 00:00:00
  //vr = dn * 86400 + h0 * 3600 + min0 * 60 + sec0;      //в секунды

  //uint32_t h1 = 11, min1 = 3, sec1 = 7;                //инициализируем прямое восхождение
  //double a = h1 * 3600 + min1 * 60 + sec1;             //a-прямое восхождение данной звезды в секундах

  //double l, ld = 56, lm = 16, ls = 1;                  //инициализируем склонение звезды
  //l = ld + lm / 60 + ls / 3600;                        //в градусах

  double f = 53.88305557;								 //f-широта местности наблюдения

  double s0 = 54460, k = 1.002737909350795;				 //s0 = 15:07:40 звездное время на момент солнечного(местного) времени 2021.05.29  00:00,
  s = s0 + vr * k;										 //a-прямое восхождение данной звезды
  t = s - a;
  while (t < 0 || t >= 86400) t = t - 86400;
  double td;											 //часовой угол в градусах
  td = t * 15 * 360 / 1296000;							 //

 
  double b, c, d;

  //sin h = sin f sin d + cos f cos d cos t
  b = sin(f * acos(-1) / 180) * sin(l * acos(-1) / 180)
      + cos(f * acos(-1) / 180) * cos(l * acos(-1) / 180) * cos(td * acos(-1) / 180);
  *h = asin(b) * 180 / acos(-1);						 //находим h высоту звезды

  //sin h sin A = cos l sin t
  c = -cos(l * acos(-1) / 180) * sin(td * acos(-1) / 180) / cos(*h * acos(-1) / 180);

  //sin z cos A = –cos f sin l + sin f cos l cos t       //находим А азимут звезды
  d = -(-cos(f * acos(-1) / 180) * sin(l * acos(-1) / 180) +
        sin(f * acos(-1) / 180) * cos(l * acos(-1) / 180) * cos(td * acos(-1) / 180)) / cos(asin(b)); 

  if (c > 0 && d > 0)       *A = asin(c) * 180 / acos(-1);
  else if (c > 0 && d < 0) *A = 180 - asin(c) * 180 / acos(-1);
  else if (c < 0 && d > 0) *A = 360 + asin(c) * 180 / acos(-1);
  else if (c < 0 && d < 0) *A = 180 - asin(c) * 180 / acos(-1);
}

//поворот моторов (выбор дробления шага)
void povorot(double A, double h, int mod) {
  switch (mod) {
    case 1:
      PORTA = 0b00001111;
      PORTC = 0b00110000;
      break;

    case 2:
      PORTD = 0b00011011;
      PORTA = 0b00011010;
      break;

    case 4:
      PORTD = 0b00101011;
      PORTA = 0b00101010;
      break;

    case 8:
      PORTD = 0b00111011;
      PORTA = 0b00111010;
      break;

    case 16:
      PORTD = 0b01001010;
      PORTA = 0b01001010;
      break;

    case 32:
      PORTD = 0b01111111;
      PORTA = 0b01111101;
      break;
  }

  n1 = A * 2048 * md / 360 * 2;
  n2 = h * 2048 * md / 360 * 2;

  if (A <= 180)
    for (int64_t i = 0; i < n1; i++) {
      PORTA |= (1 << 1);
      PORTA &= ~(1 << 1);
      _delay_ms(2);
      //_delay_us(50);
    }
  else {
    n12 = b - n1;
    for (int64_t i = 0; i < n12; i++) {
      PORTA |= (1 << 3);
      PORTA &= ~(1 << 3);
      _delay_ms(2);
      // _delay_us(50);
    }
    PORTA |= (1 << 0);
  }

  if (h>0 && h<80)
    for (int64_t i = 0; i < n2; i++) {
      PORTC |= (1 << 6);
      PORTC &= ~(1 << 6);
      _delay_ms(2);
      //_delay_us(50);
    }
	
	PORTC = 0;
	PORTA = 0;
}


void revers(double A, double h, int mod) {
      PORTA = 0b00001111;
      PORTC = 0b00110000;

  n1 = A * 2048 * md / 360 * 2;
  n2 = h * 2048 * md / 360 * 2;

  if (A <= 180)
    for (int64_t i = 0; i < n1; i++) {
      PORTB = 0b00000000;
      PORTB = 0b00000001;
      _delay_ms(2);
      //_delay_us(50);
    }
  else {
    PORTD |= (1 << 0);
    n12 = b - n1;
    for (int64_t i = 0; i < n12; i++) {
      PORTB = 0b00000000;
      PORTB = 0b00000001;
      _delay_ms(2);
      //_delay_us(50);
    }
    PORTD &= (1 << 0);
  }

  if (h > 0 && h < 80)
    for (int64_t i = 0; i < n2; i++) {
      PORTC = 0b00000000;
      PORTC = 0b00000001;
      _delay_ms(2);
      //_delay_us(50);
    }

  PORTC = 0;
  PORTA = 0;
}

void circles_m1(uint16_t n, uint8_t rl) {
	if(rl) PORTC = 0b00110000;
	else   PORTC = 0b10110000;
	uint16_t i = 0;
	for(i = 0; i < n; i++) {
      PORTC |= (1 << 6);
      PORTC &= ~(1 << 6);
		_delay_ms(5);
		}
		PORTC = 0;
}

void circles_m2(uint16_t n, uint8_t ud) {
	if(ud) PORTA = 0b00001111;
	else   PORTA = 0b00001110;
	uint16_t i = 0;
	for(i = 0; i < n; i++) {
      PORTA |= (1 << 1);
      PORTA &= ~(1 << 1);
		_delay_ms(5);
		}
		PORTA = 0;
}

