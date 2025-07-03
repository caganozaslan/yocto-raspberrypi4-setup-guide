# Sıkça Sorulan Sorular
Bu döküman, Raspberry Pi 4 için Yocto imajı derleme ve çalıştırma sürecinde sıkça sorulan sorular ve problemleri içermektedir.

## Bitbake Derlemesi Takılma Sorunu
Yocto derleme sürecinde bitbake komutu uzun süre çalışırken aniden takılabilir veya hiç ilerlemez. Bu, özellikle WSL (Windows Subsystem for Linux) altında çalışan kullanıcılar için sıkça görülen bir problemdir. Genellikle WSL’nin varsayılan RAM ve SWAP ayarlarının yetersiz kalması nedeniyle sistem kaynakları tükenir ve derleme yarıda kalır.

### Bellek ve SWAP Alanı Kontrolü ve Ayarlama
WSL'in ne kadar RAM ve SWAP alanına sahip olduğunu görmek için WSL terminalinde **free -h** komutunu girin. RAM 12 GB ve SWAP 32 GB altındaysa aşağıdaki adımlar ile gerekli ayarları yapabilirsiniz.

WSL yapılandırma dosyası Windows tarafında, kullanıcı klasörünüzde bulunur:

```bash
C:\Users\<KULLANICI_ADINIZ>\.wslconfig
```
Dosya mevcut değilse, Not Defteri ile oluşturun ve içeriğini şu şekilde ayarlayın:
```bash
[wsl2]
memory=12GB
processors=4
swap=32GB
localhostForwarding=true
```

> **memory:** WSL’ye ayrılacak maksimum RAM miktarı

> **processors:** Kullanılacak maksimum işlemci çekirdeği

> **swap:** WSL içinde oluşturulacak sanal bellek boyutu

> **localhostForwarding:** Geliştirme sırasında localhost port yönlendirmesi için gereklidir

Ayarları yaptıktan sonra WSL'i yeniden başlattığınızda ayarlarınız güncellenmiş olacaktır.

> Not: Sisteminiz daha düşük RAM'e sahipse, RAM'i 8 GB, SWAP'i 48 GB yapabilirsiniz.

## Siyah Ekran Sorunu
Raspberry Pi 4 için Yocto imajı oluşturma sürecinde en yaygın karşılaşılan sorun siyah ekran problemidir. Siyah ekran sorununu tespit etmek için aşağıdaki kontrolleri sırasıyla yapın:

### Backlight Kontrolü
Ekranınızın arkasını çevirdiğinizde beyaz arka aydınlatma ışıklarını görüyor musunuz? Ekranda hiçbir şey olmasa bile ekrana güç girişi olduğunda arka aydınlatmaların yanması gerekmektedir. Eğer arka aydınlatmalar yanmıyorsa birkaç temel sebepten kaynaklanır:

- Ekranın güç girişleri bağlı olmayabilir.
- Güç girişleri ters bağlanmış olabilir.
- Ekran arızalı olabilir.

Eğer arka aydınlatmalar yanmıyorsa fiziki bağlantıları mutlaka kontrol edin ve doğru bağlandığından emin olun. Eğer yanlış bağlantı yaptıysanız bağlantı pinlerini düzeltseniz bile ekran artık kullanılmaz hale gelmiş olabilir.

### Framebuffer (fbdev) Mevcut mu?

Yocto tabanlı gömülü sistemlerde, grafik arayüzlerin ekranda görünmesi için framebuffer (fbdev) altyapısının düzgün çalışması gerekir. Framebuffer, işletim sisteminin grafik verisini donanıma aktardığı düşük seviyeli bir arayüzdür. Eğer sistemde framebuffer aygıtı (/dev/fb0) bulunmuyorsa, ekran görüntü veremez. Bu genellikle kernel yapılandırması, device tree ayarları ya da overlay eksikliği gibi nedenlerden kaynaklanır. Kontrol için aşağıdaki adımları uygulayın:


### Framebuffer Çalışıyor Mu?
Framebuffer'ın gerçekten çalışıp çalışmadığını kontrol etmek için ekrana random piksel yazabiliriz. Terminalde aşağıdaki komutu çalıştırın:

```bash
cat /dev/urandom > /dev/fb0
```
Eğer terminalde hiçbir hata görmüyorsanız ve ekranda parazit gibi pikseller oluşuyorsa framebuffer sorunsuz çalışıyor demektir. Bu durumda hata konsol aktif edilmemesinden veya yanlış yerde çalışmasından kaynaklanıyordur.

```bash
chvt 1
```

```bash
chvt 2
```

```bash
chvt 3
```

komutlarını sırayla deneyerek konsol ekranını görmeye çalışın. Eğer konsol hala gözükmüyorsa Yocto imajı oluşturma sürecini doğru yaptığınızdan emin olun.

### Framebuffer Aygıtı Var Mı?

UART ile bağlanarak terminalde aşağıdaki komutu çalıştırın:

```bash
ls /dev/fb*
```

Eğer **No such file or directory** çıktısı alıyorsanız framebuffer mevcut değil demektir. Eğer hiçbir çıktı yoksa, framebuffer doğru yüklenemiyor demektir. 

Framebuffer hatalarının pek çoğu **config.txt** dosyasındaki yapılandırma eksiklerinden kaynaklanır. **config.txt** dosyasında **dtoverlay=vc4-fkms-v3d** satırının olduğundan emin olun. Eğer buna rağmen sorun devam ediyorsa ilgili satırı kaldırıp **dtoverlay=vc4-kms-v3d** satırını ekleyerek deneyin. 

> Not: Her iki satırın aynı anda bulunmadığından mutlaka emin olun.

#### HDMI ve DSI Aynı Anda Bağlı mı? 
Raspberry Pi 4 cihazında hem HDMI hem de DSI ekran aynı anda takılıysa görüntü önceliği karışabilir ve ekran tamamen siyah kalabilir. Sistem, görüntüyü varsayılan olarak HDMI üzerinden göndermeye çalışırken DSI ekran pasif kalabilir. 

- HDMI ekran bağlı olup olmadığını fiziki olarak kontrol edin, eğer HDMI ekran bağlıysa fiziki bağlantıyı kaldırın.
- HDMI ekran bağlantısını kaldırmanızda bir engel varsa veya sorun hala devam ediyorsa aşağıdaki satırı **config.txt** dosyasına ekleyerek HDMI ekran çıkışını yazılım tarafında kaldırabilirsiniz.

```bash
hdmi_blanking=2
```

## LVGL Uygulaması Başlatılıyor Ancak Ekranda Görünmüyor
Eğer ekranınızda sorunsuz bir şekilde görüntü alabiliyor fakat LVGL uygulamanız ekranda gözükmüyorsa sorun birkaç nedenden kaynaklanabilir.

### Masaüstü Ortamı Çalışması
Sistemde bir X11, Wayland veya herhangi bir masaüstü ortamı (örneğin Weston, GNOME, LXDE) çalışıyorsa, framebuffer erişimi bu masaüstü yöneticisi tarafından kilitlenir. Bu durumda uygulama arka planda çalışıyor görünür ama ekran hiçbir şey göstermez. Eğer sistemde bir masaüstü ortamı çalışıyorsa mutlaka devredışı bırakmanız gerekir. Çözüm için aşağıdaki komutları girebilirsiniz:

```bash
systemctl disable weston
systemctl disable xserver
systemctl disable graphical.target
```

> Yaygın olmayan bir masaüstü kullanıyorsanız belirtilen komutlar haricinde komutlar göndererek masaüstünü kapatın.

Test için aşağıdaki komutu girebilirsiniz:

```bash
cat /dev/urandom > /dev/fb0
```

Eğer ekranda parazitler görüyorsanız artık framebuffer boşta ve aktif kullanıma hazır anlamına gelmektedir.

### LVGL Yapılandırmanız Framebuffer ile Uyumlu mu?
Eğer sisteminizde herhangi bir masaüstü ortamı aktif değilse SDL, DRM gibi görüntü altyapıları kullanamazsınız. Bunlar bir masaüstüne ihtiyaç duyarlar. Eğer LVGL uygulamanız Framebuffer harici bir görüntü altyapısı kullanıyorsa ve masaüstü ortamınız yoksa uygulamanız görüntü vermeyecektir.

- **lv_conf.h** dosyasında framebuffer aktif olduğundan ve diğer tüm görüntü altyapılarının pasif olduğundan emin olmalısınız.

```bash
#define LV_USE_FBDEV 1
```
satırının dosyada bulunduğundan ve 1 olarak aktif halde olduğundan emin olun.

```bash
#define LV_USE_SDL 0
#define LV_USE_DRM 0
```
gibi diğer görüntü altyapılarının 0 yani pasif olarak tanımlandığından emin olun.

LVGL uygulamanızın main dosyasında aşağıdaki gibi tanımlama yapıldığından emin olun:

```bash
  lv_display_t * disp = lv_linux_fbdev_create();  
  lv_linux_fbdev_set_file(disp, "/dev/fb0");
```
> Not: Varsayılan olarak genellikle **/dev/fb0** kullanılır, ancak bazı durumlarda DSI ekran fb1 olarak atanabilir. Bu durumda **/dev/fb1** olarak tanımlamanız gerekir.


## Dokunmatik Çalışmama Sorunu
Dokunmatik ekranların çalışmama sorunları genellikle iki ana sebepten kaynaklanır: fiziksel bağlantı hataları ve sürücü eksikliği. Aşağıda önce fiziksel bağlantıların doğru yapıldığını kontrol edin, ardından sürücülerin ve yazılım bileşenlerinin düzgün yüklendiğinden emin olun.

### Fiziksel Bağlantı Hataları

- Örnek projemizde kullandığımız Waveshare ekranlar ve pek çok dokunmatik ekran dokunmatik veri aktarımını I2C hatları üzerinden yaparlar. Ekranın I2C pinlerinin Raspberry'deki (Veya kendi cihazınız) doğru pinlere bağlandığından emin olun. 
- Bağlantılar zaman içinde gevşeyebilir, çıkartıp yeniden sağlam olarak takarak fiziki bağlantının sağlandığından emin olun. 
- Eğer ekranınızda I2C, SPI gibi pinler bulunmuyorsa muhtemelen ekranınız dokunmatik verisini de görüntü kablosu üzerinden aktarıyordur. Bu durumda görüntü kablosunun hasarsız olduğundan ve sıkıca bağlı olduğundan emin olun.

### LVGL Sürücü Eksikliği

LVGL uygulamalarında dokunmatik desteği kullanabilmek için EVDEV sürücüsünün doğru şekilde yapılandırılmış olması gerekir. Projeniz main dosyasında aşağıdaki bölümleri kontrol edin:

```bash
#include "lvgl/src/drivers/evdev/lv_evdev.h"
```
Satırının varlığından emin olun.

```bash
lv_indev_t * indev = lv_evdev_create(LV_INDEV_TYPE_POINTER, "/dev/input/touchscreen0");
```

Satırının varlığından ve doğru yerde tanımlanmış olduğundan emin olun. Ayrıca, bu satırın hemen altına aşağıdaki kod bloğunu ekleyerek hata ayıklamasını kolaylaştırabilirsiniz:

```bash
 if(indev == NULL) {
        printf("Touch input error!\n");
    }
```