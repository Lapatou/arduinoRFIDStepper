#include <SPI.h>
#include <RFID.h>

RFID ko(10, 9); //SDA ve RESET pinlerini parametre olarak gönderiyoruz

//motor için yön pini 7. pinde bağlı
//motor için pulls pini 8. pinde bağlı

//bilinen kartlar
byte kart1[5] = {179, 10 , 2 , 1 , 186};

//kapı açık mı ?
boolean kapi = true;
void setup() {
  //Debugin için
  Serial.begin(9600);
  //SPI (seri) haberleşmeyi başlatıyoruz
  SPI.begin();
  //motor için çıkış pinleri ayarlanıyor
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  //Kart Okuyucuyu devreye sokuyoruz (veri almaya hazırlıyoruz)
  ko.init();
  //motor 0 noktasına götürülüyor
  stepMotor(500, 2000, true);//2000 değeri kullanılan yere göre değişim gösterebilir.

}
void loop() {
  //kart gösteriliyor mu?
  if (ko.isCard()) {  
    // kartın içi okunuyor
    if (ko.readCardSerial()) {
      Serial.println("ID : ");
      Serial.print(ko.serNum[0]);
      Serial.print(" , ");
      Serial.print(ko.serNum[1]);
      Serial.print(" , ");
      Serial.print(ko.serNum[2]);
      Serial.print(" , ");
      Serial.print(ko.serNum[3]);
      Serial.print(" , ");
      Serial.print(ko.serNum[4]);
      Serial.println(" ");
    }
    Serial.println("okuma islemi bitti");
    //karşılaştırma işlemleri başlıyor.
    for (int i = 0 ; i < 5; i++) {
      if (ko.serNum[i] != kart1[i]) {
        kapi = false;
      }
    }
    //Bilinen kart okunduysa giriş için kapı açlıyor
    if (kapi == true) {

      Serial.println("Kapi Aciliyor");
      stepMotor(70, 2000, false);
      //Kilit 3 sn açık bekletildikten sonra otomatik olarak kendini tekrar kilitliyor.
      delay(3000);
      stepMotor(70, 2000, true);
      Serial.println("Kapi Kapaniyor");
    }
    kapi = true;
    //kart uzaklaşınca işlemi sonladırmak için halt komutu ile kart okuyucu uyku konumuna getiriliyor
    ko.halt();
  }

}

//stepper motoru kontrol etmek için basit metot
//hiz değişkeni minimum 70 civarında olmalıdır motora göre değişim gösterebilir 
void stepMotor(int hiz, int stepSayisi, boolean yon) {
//motorun döneceği yöne karar veriliyor
  if (yon == true) {
    digitalWrite(7, HIGH);
  } else {
    digitalWrite(7, LOW);
  }
//motorun ilerleyeceği miktar stepSayısı değişkeni ile belirtiliyor 
  for (int i = 0; i < stepSayisi; i++) {

    //motora 1 atım yollanarak 1 adım ilerletiliyor 
    digitalWrite(8, HIGH);
    //makanik aksanın elektroniğin hızıne yetişmesi için delay veriliyor 
    //delay mikro saniye yani saniyenin 1/1.000.000(10^-6) şeklinde gönderiliyor
    delayMicroseconds(hiz);
    digitalWrite(8, LOW);
    delayMicroseconds(hiz);
  }
}

