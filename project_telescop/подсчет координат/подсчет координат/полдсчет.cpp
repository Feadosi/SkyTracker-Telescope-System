#include<iostream>
#include <iomanip>

using namespace std;


int main(){

//Polar star

int dn=14, h0=22, min0=42, sec0=10 ;                                //инициализируем сколько времени прошло после 2021.05.29 00:00:00
int vr=dn*86400+h0*3600+min0*60+sec0;                             //в секунды
                            
int h1=2, min1=59, sec1=3;                                        //инициализируем прямое восхождение
double a=h1*3600+min1*60+sec1;                                    //a-прямое восхождение данной звезды в секундах
	
double l, ld=89, lm=21, ls=14;                                    //инициализируем склонение звезды
l=ld+lm/60+ls/3600;                                               //в градусах
		
double f=53.88305557;                                            //f-широта местности наблюдения 
	                                                                  
double s0=54460, k=1.002737909350795, s, t;	                      //s0 = 15:07:40 звездное время на момент солнечного(местного) времени 2021.05.29  00:00, 
s=s0+vr*k;												          //a-прямое восхождение данной звезды
t=s-a;
while(t<0 || t>=86400) t=t-86400;
double td;                                                        //часовой угол в градусах
td=t*15*360/1296000;                                              //

//sin h = sin φ sin δ + cos φ cos δ cos t
double b,c,d,A,h;                          
	
//sin h = sin φ sin δ + cos φ cos δ cos t         
b=sin(f*acos(-1)/180)*sin(l*acos(-1)/180)
+cos(f*acos(-1)/180)*cos(l*acos(-1)/180)*cos(td*acos(-1)/180);
h=asin(b)*180/acos(-1);                                          //находим h высоту звезды
	
///sin h sin A = cos δ sin t
c=-cos(l*acos(-1)/180)*sin(td*acos(-1)/180)/cos(h*acos(-1)/180);
	
//sin z cos A = –cos φ sin δ + sin φ cos δ cos t
d=-(-cos(f*acos(-1)/180)*sin(l*acos(-1)/180)+
sin(f*acos(-1)/180)*cos(l*acos(-1)/180)*cos(td*acos(-1)/180))/cos(asin(b));     //находим А азимут звезды
  
if(c>0 && d>0)       A=asin(c)*180/acos(-1);
	else if(c>0 && d<0) A=180-asin(c)*180/acos(-1);
	else if(c<0 && d>0) A=360+asin(c)*180/acos(-1);
	else if(c<0 && d<0) A=180-asin(c)*180/acos(-1);

	cout<<"A="<<A<<"      h="<<h<<"      t="<<t<<"      s="<<s/1000<<h<<"      vr="<<vr<<endl;
	
system("pause");
return 0;
}

 
