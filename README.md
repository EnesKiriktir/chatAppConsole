# deu-signal-EnesKiriktir
deu-signal-EnesKiriktir created by GitHub Classroom

Her iki kod dosyasında da json kütüphanesi bulunduğu için json header dosyası yüklü olmalıdır.
Yönergeler asağıda verilmiştir.

**********************************
-- JSON LIBRARY --

-->  sudo nano /etc/apt/sources.list

-->  http://ftp.tr.debian.org/debian/ jessie main

-->  sudo apt-get update

-->  sudo apt-get install libjson0-dev

-->  sudo apt-get install libjson-c-dev=0.11-4

-->  sudo apt-get install libjson0-dev

***********************************

Nasıl çalıştırılır?

-->  gcc 2016510052_server.c -o server.out -ljson -lpthread

-->  gcc 2016510052_client.c -o client.out -ljson -lpthread

***********************************
Nasıl kullanılır?

-->  -gcreate group_name(bu komut sonrasında kullanıcıya "-->Enter the group password:" mesajı gösterilecektir.
Bu kısımda grup şifresi girilebilir.)

-->  -whoami(kullanıcı phone number görüntülenir.)

-->  -join group_name(Gruba katılmak için yazılmalıdır. Yazıldıktan sonra "-->Enter the password:" mesajı ile 
gruba giriş için grubun şifresinin girilmesi istenecektir.)

-->  -exit(Bu komut ile client ın bağlantısı sonladırarak uygulamadan çıkılır.)

-->  -exit group_name(Gruptan çıkmak için kullanılan komuttur.)

-->  -send message(gruba gönderilmek istenen mesajı göndermek için kullanılır.)

