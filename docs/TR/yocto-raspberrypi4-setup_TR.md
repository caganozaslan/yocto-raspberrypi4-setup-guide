# Raspberry Pi 4 İçin Adım Adım Yocto Kurulumu

Bu doküman, Raspberry Pi 4 üzerinde çalışacak gömülü bir Linux sistemi geliştirmek amacıyla, Yocto Project kullanılarak özel bir işletim sistemi imajı oluşturma sürecini en küçük ayrıntısına kadar adım adım açıklamaktadır. Geliştirilen sistem; grafiksel arayüz desteği için LVGL (Light and Versatile Graphics Library), ağ üzerinden erişim için SSH ve Wi-Fi bağlantısı, dokunmatik ekran desteği ve terminal tabanlı temel araçları içerecek şekilde yapılandırılmıştır. Tüm işlemler Ubuntu yüklü bir WSL (Windows Subsystem for Linux) ortamında gerçekleştirilmiş olup, Yocto SDK aracılığıyla geliştirilen uygulamaların hedef donanımda derlenmesi ve çalıştırılması da ayrıntılı olarak ele alınmıştır.

## Çalışma Ortamı

Bu dökümanın doğru bir şekilde uygulanabilmesi için aynı çalışma ortamına sahip olunmalı ve gereklilikler yerine getirilmelidir.


- İşletim Sistemi: Windows 10/11
- WSL: WSL2
- Donanım: Raspberry Pi 4B
- Linux Dağıtımı: Ubuntu 20.04
- Ekran: Waveshare 7inch Capacitive Touch Display DSI LCD (800x480)

## Gereksinimler

- 16 GB ve üstü RAM
- 150 GB ve üstü boş disk alanı
- WSL kullanımına hazır en az 8 GB RAM
- WSL kullanımına hazır en az 32 GB SWAP alanı.
- UART to USB dönüştürücü
- SD Kart (8 GB minimum, önerilen 16 GB ve üstü)
- SD to USB dönüştürücü

### Bellek ve SWAP Alanı Kontrolü

WSL'in ne kadar RAM ve SWAP alanına sahip olduğunu görmek için WSL terminalinde **free -h** komutunu girin. Çıkan sonuçlar gereksinimlerin altındaysa ayar yapmak için gerekli talimatları uygulayabilirsiniz.

#### Bellek ve SWAP Alanı Ayarlama

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


## Yocto Proje Kurulumu ve İmaj Derleme Süreci
Bu aşamada Raspberry Pi 4 cihazı için sıfırdan bir Yocto ortamının nasıl kurulduğu, gerekli meta katmanların nasıl eklendiği, yapılandırma dosyalarının nasıl düzenlendiği ve son olarak sistemin başarılı bir şekilde derlenip .wic uzantılı bir imaj dosyasının nasıl oluşturulduğu adım adım belgelenmiştir. 

İlk olarak WSL ortamımıza projemizde kullanacağımız paketleri ekleyelim.

```bash
sudo apt update && sudo apt install -y \
    gawk wget git-core diffstat unzip texinfo \
    gcc build-essential chrpath socat cpio python3 \
    python3-pip python3-pexpect xz-utils debianutils \
    iputils-ping libsdl2-dev xterm zstd libssl-dev \
    locales tmux libncurses5 libncurses5-dev libncursesw5-dev \
    libtinfo5 libtinfo-dev libpixman-1-dev libwayland-dev \
    libdrm-dev libxkbcommon-dev libinput-dev \
    bzip2 curl
```
Listedeki paketler projemizin bütün gereksinimlerini karşılamaktadır. Ekstra olarak WSL çalışma ortamında yaygın olarak kullanılan paketler de gelecek projeler için eklenmiştir.

### Yocto Çalışma Dizinin Oluşturulması

Yocto projemizi çalışacağımız dizini oluşturacağız. Sabit bir klasör altında ilerlemek projenin düzeni açısından oldukça önemlidir. Dökümanı rahat takip edebilmek için aynı isimle klasör oluşturabilirsiniz. Eğer farklı bir isim kullanmak isterseniz ilgili bölümü düzenleyebilirsiniz.

```bash
mkdir -p ~/yocto-project-rpi
cd ~/yocto-project-rpi
```

### Yocto Çekirdeğinin ve Katmanların İndirilmesi

Bu projede çekirdek ve katmanların **Kirkstone** sürümü kullanılmıştır. Dökümanı referans alabilmek için lütfen aynı sürümü kullanın.

Poky çekirdek katmanının indirilmesi:

```bash
git clone -b kirkstone git://git.yoctoproject.org/poky
```

Tüm katmanlar Poky çekirdeğinin altında bulunmalıdır.

```bash
cd poky
```

Raspberry Pi destek katmanı ve ek paketler için Openembedded katmanı indirilmesi:

```bash
git clone -b kirkstone https://github.com/agherzan/meta-raspberrypi.git
git clone -b kirkstone https://github.com/openembedded/meta-openembedded.git
```

Bitbake aktif etmek için:

```bash
source oe-init-build-env
```
Katmanları Yocto'ya dahil etmek için sırasıyla:

```bash
bitbake-layers add-layer ../meta-raspberrypi
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-python
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-multimedia
```
Tüm katmanların doğru bir şekilde eklendiğinden olman için şu komutu çalıştırabilirsiniz:

```bash
bitbake-layers show-layers
```

Katman listesinde bu şekilde bir çıktı görmelisiniz. (Sıralama farklı olabilir.)

```
layer                 path                                                             priority
=========================================================================================================
meta                  /home/arda/yocto-project-rpi/poky/meta                                   5
meta-poky             /home/arda/yocto-project-rpi/poky/meta-poky                              5
meta-yocto-bsp        /home/arda/yocto-project-rpi/poky/meta-yocto-bsp                         5
meta-oe               /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-oe              5
meta-python           /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-python          5
meta-networking       /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-networking      5
meta-multimedia       /home/arda/yocto-project-rpi/poky/meta-openembedded/meta-multimedia      5
meta-raspberrypi      /home/arda/yocto-project-rpi/poky/meta-raspberrypi                        9
```

### Yocto Derleme Ayarlarının Yapılması

Projemiz için gerekli olan tüm katmanları Yocto'ya dahil ettik ancak katmanların içerisinde yalnızca ihtiyacımız olan araçları imajımıza dahil ederek optimize edilmiş bir Linux dağıtımına sahip olacağız.

local.conf ayarlarını yapmak için:

(~/yocto-project-rpi/poky dizininde olduğunuz varsayılır.)

```bash
nano build/conf/local.conf
```



Hedef cihazı ayarlamak için dosyada **MACHINE ??= "qemux86-64"** satırını bulun ve aşağıdaki gibi değiştirin.

```bash
MACHINE = "raspberrypi4-64"
```

Grafik belleğini ayarlamak ve UART desteğini aktifleştirmek için dosyanın en altına aşağıdaki satırları alt alta ekleyin.

```bash
ENABLE_UART = "1"
GPU_MEM = "128"
```
Bu satırlar ile seri port UART aktif hale getirilir ve GPU’ya 128MB video belleği atanır. Dokunmatik ekranlar ve HDMI çıkışı için önerilir.

DSI ekran desteği için aşağıdaki satırı en alta ekleyin: 
```bash
RPI_EXTRA_CONFIG += " \
dtoverlay=vc4-fkms-v3d \
"
```

SSH sunucusu, Wi-Fi bağlantı araçları, metin editörü (nano), sistem komutları (htop, wget, curl) vb. kullanılabilmek için aşağıdaki satırları en alta ekleyin:

```bash
IMAGE_INSTALL:append = " \
    linux-firmware-bcm43455 \
    linux-firmware \
    iw wpa-supplicant openssh nano bash htop \
    coreutils util-linux libstdc++ libstdc++-dev \
    libgcc wget curl iproute2 iputils net-tools \
"
```

Gelişmiş özellikleri aktif etmek için aşağıdaki satırları en alta ekleyin: 

```bash
IMAGE_FEATURES:append = " \
    package-management \
    ssh-server-openssh \
    tools-debug \
"
```

Eğer projenizde bir arayüz kullanacaksanız LVGL, Framebuffer, dokunmatik ve test araçları için aşağıdaki satırları dosyanın en altına ekleyin:

```bash
EXTRA_IMAGE_INSTALL:append = " \
    lvgl \
    lv-drivers \
    lv-lib-png \
    libsdl2 \
    libevdev \
    tslib \
    fbset \
    evtest \
    nano gdb strace procps coreutils less file which \
"
```

LVGL uygulamalarınızı SDK üzerinde derleyeceksiniz aşağıdaki satırları ekleyerek LVGL uygulamalarının derlenmesi için gerekli olan paketleri SDK'ya dahil edebilirsiniz.

```bash
TOOLCHAIN_HOST_TASK:append = " \
    nativesdk-cmake \
    nativesdk-pkgconfig \
    nativesdk-libsdl2 \
"

TOOLCHAIN_TARGET_TASK:append = " \
    libevdev \
"
```

Belirtilenler haricinde dosyadaki hiçbir alana müdahale etmeyin. Dosya işlemleriniz bittiyse kaydedin ve çıkın. (CTRL + O » CTRL + X)

Tüm yapılandırma tamamlandıktan sonra, artık imaj dosyasını oluşturabiliriz. Derleme işlemine zaman alabilir. Özellikle ilk derlemede 1-3 saat arasında değişkenlik gösterir.

```bash
bitbake core-image-base
```

#### Derlemenin Tamamlanamadan Durdurulması

Bitbake derlemesi sürerken eğer istemli veya istemsiz şekilde durdurmak zorunda kalırsanız derlemeye kaldığınız yerden devam edebilirsiniz. Derlemeye yeniden başlamadan önce cache temizliği yaparak tüm hata ihtimallerini ortadan kaldırabilirsiniz. Derlemeye yeniden başlamak için;

Yeniden WSL'i açın ve bitbake'i aktif hale getirin:

```bash
cd yocto-project-rpi/poky
source oe-init-build-env
```

Geçici dosyaları ve cache'leri temizleyin:
```bash
bitbake -c cleansstate core-image-base
```

İmajı yeniden derlemeye başlayın:
```bash
bitbake core-image-base
```

### İmajı SD Karta Yazma
Derleme bittikten sonra oluşan .wic.bz2 dosyası **tmp/deploy/images/raspberrypi4-64/** klasöründe olacaktır.

Dosyayı çıkartmak için:
```bash
cd tmp/deploy/images/raspberrypi4-64/
bunzip2 -f core-image-base-raspberrypi4-64.wic.bz2
```

Yaklaşık 30 saniyenin ardından .wic uzantılı dosya çıkarılacaktır. İmajın bulunduğu klasöra Windows'ta kolayca ulaşmak için:
```bash
explorer.exe .
```

balenaEtcher ile .wic imajını yazmak için şu adımlar izlenmelidir:

1. [balenaEtcher](https://www.balena.io/etcher/) uygulamasını indirip bilgisayarınıza kurun.
2. Uygulamayı açın.
3. "Flash from file" seçeneğine tıklayın ve daha önce çıkardığınız `.wic` uzantılı imaj dosyasını seçin.
4. "Select target" butonuna tıklayarak SD kartınızı seçin.
5. "Flash!" butonuna tıklayarak yazma işlemini başlatın.
6. Yazma işlemi bittikten sonra Etcher otomatik olarak doğrulama yapar. "Flash Complete!" mesajı göründüğünde işlem tamamlanmış demektir.

## config.txt Ayarları
`config.txt`, Raspberry Pi'nin önyükleme sırasında donanım yapılandırmasını belirleyen temel bir yapılandırma dosyasıdır. Bu dosya, GPU bellek miktarı, ekran çözünürlüğü, video çıkışı, overlay (sürücü eklentileri) ve diğer donanım ayarlarını tanımlar. Yocto tarafından oluşturulan imajlar bu dosyayla birlikte gelir, ancak özel donanımlar (örneğin DSI ekranlar, dokunmatik paneller veya ek sensörler) kullanıldığında bu dosyada bazı manuel düzenlemeler yapılması gerekebilir.

Projemizde ekran ve dokunmatik ayarları için düzenlemeler yapılması gerekmektedir. SD kartınızı Windows üzerinde açtığınızda **boot** klasörünü görüntüleyebilir ve içerisindeki dosyaları düzenleyebilirsiniz. **boot** klasörü içerisinde **config.txt** dosyasını bulun ve;


- Eğer Raspberry Pi 4'ün UART'ını aktif etmek istiyorsanız `enable_uart=1`
satırının dosyada mevcut olduğundan emin olun. 
- DSI çıkışından görüntü alabilmek için `dtoverlay=vc4-fkms-v3d` veya `dtoverlay=vc4-kms-v3d` satırlarından biri bulunmalıdır. Eğer arayüzünüz Framebuffer kullanıyorsa `dtoverlay=vc4-fkms-v3d` etkin olmalı, modern bir GPU hızlandırmalı Qt veya Wayland tabanlı sistem kullanacaksanız `dtoverlay=vc4-kms-v3d` satırı etkin olmalı. Her iki satır kesinlikle aynı anda bulunmamalı!
- Eğer ekranınızın dokunmatik özelliğini kullanmak istiyorsanız `dtparam=i2c_arm` satırını bulun, `dtparam=i2c_arm=on` olarak düzenleyin. Ardından dosyanın en altına `dtoverlay=rpi-ft5406` satırını ekleyin.

Dosyayı kaydedin ve kapatın. Artık SD kartınız Raspberry Pi cihazınıza takılmaya hazır. 

> ⚠️ **UYARI:**  
> SD kartı bilgisayarınızdan çıkarmadan önce mutlaka **"XX Storage Aygıtını Çıkar"** işlemini yapınız.  
> Aksi halde imajın bozulmasına ve sistemin açılmamasına neden olabilir.

## Raspberry Pi 4 Yapılandırması

### Raspberry Pi 4 UART Bağlantısı Kurulumu

Raspberry Pi 4’e doğrudan seri terminal erişimi sağlamak için bir USB to UART dönüştürücü kullanılabilir. Bu yöntem, sistemde ekran veya ağ bağlantısı olmasa bile cihazla doğrudan terminal üzerinden iletişim kurmayı sağlar.

---

#### 1. Donanımı Bağlayın

- USB to UART dönüştürücünüzü şu şekilde bağlayın:
  - **Dönüştürücü TX** → Raspberry Pi'nin **RX** pinine
  - **Dönüştürücü RX** → Raspberry Pi'nin **TX** pinine
  - **Dönüştürücü GND** → Raspberry Pi'deki herhangi bir **GND** pinine

> 📌 **Not:** Raspberry Pi 4 GPIO pin dizilimi için [resmi GPIO şemasına](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-pinout) göz atabilirsiniz.

---

#### 2. Bilgisayarda COM Port Numarasını Belirleyin

Bilgisayarınıza USB üzerinden bağlı dönüştürücünün hangi portta tanındığını görmek için:
  - Başlat menüsüne sağ tıklayın → **Aygıt Yöneticisi**'ni açın
  - "Bağlantı Noktaları (COM ve LPT)" altında `"USB Serial Port (COMx)"` şeklinde listelenen aygıtı bulun
  - `COMx` port numarasını bir kenara not edin

---

#### 3. PuTTY Uygulamasını İndirin ve Kurun

- Seri bağlantı için [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html) uygulamasını indirin ve kurun
- Kurulumda varsayılan ayarları kullanabilirsiniz

---

#### 4. PuTTY ile Seri Bağlantı Kurun

- PuTTY’yi başlatın
- "Session" sekmesinde:
  - **Connection type**: *Serial*
  - **Serial line**: `COMx` (örnek: `COM3`)
  - **Speed (baud rate)**: `115200`
- **Open** butonuna tıklayın

> Bağlantı açıldığında ekran boşsa, Enter tuşuna basın. Login ekranı gelmelidir.

---

#### 5. root Kullanıcısı ile Giriş Yapın

Login ekranı göründüğünde aşağıdaki şekilde giriş yapabilirsiniz:

```plaintext
raspberrypi login: root
```

Şifre istenmeyecektir. Yocto sistemlerde root kullanıcısı varsayılan olarak şifresizdir. İleride güvenlik için şifre ekleyebilirsiniz.

### Wi-Fi Kurulumu ve SSH ile Bağlantı

Raspberry Pi 4 cihazının internete bağlanabilmesi ve uzak erişim sağlanabilmesi için Wi-Fi yapılandırması yapılmalı, ardından SSH bağlantısı kurulmalıdır. Aşağıdaki adımlar Yocto sistem üzerinde manuel olarak Wi-Fi yapılandırmasını ve SSH ile bağlantıyı açıklar.

---

#### 1. wpa_supplicant yapılandırma klasörünü oluşturun (eğer yoksa)

```bash
mkdir -p /etc/wpa_supplicant
```

---

#### 2. Wi-Fi yapılandırma dosyasını oluşturun

```bash
nano /etc/wpa_supplicant/wpa_supplicant.conf
```

İçeriği şu şekilde yazın:

```
ctrl_interface=/var/run/wpa_supplicant
ctrl_interface_group=0
update_config=1

network={
    ssid="WiFi_AGI"
    psk="sifre"
}
```

> `ssid` ve `psk` alanlarını kendi kablosuz ağ bilgilerinize göre güncelleyin.

---

#### 3. Wi-Fi bağlantısını başlatın

```bash
wpa_supplicant -B -i wlan0 -c /etc/wpa_supplicant/wpa_supplicant.conf
udhcpc -i wlan0
```
Bu işlem sonunda kablosuz ağ bağlantısı başlamış olur ve IP adresi istenmiş olur. Eğer bu adımda bir hata alırsanız lütfen yapılandırma dosyanızdaki Wi-Fi adresi ve şifresini kontrol edin.

---

#### 4. IP adresini kontrol edin

```bash
ip addr show wlan0
```

Bu komut sonucunda gördüğünüz çıktıda 192.168.1.XX/24 formatında gördüğünüz IP adresi cihazınızın IP adresidir.

---

#### 5. SSH ile bağlantı kurun

Aynı ağda olan bilgisayardan SSH ile bağlantı kurabilirsiniz. Bunun için WSL terminalinde komutunu girin:

```bash
ssh root@192.168.1.x
```

> Yocto sistemlerde `root` kullanıcısı şifresiz olarak giriş yapabilir.

WSL terminalinizde **root@raspberrypi4-64:~#** şeklinde görüntülemeye başladıysanız SSH bağlantınız başarıyla tamamlanmış demektir. Çeşitli komutlar deneyerek bağlantıyı test edebilirsiniz.

# SDK Oluşturma ve Uygulama Derleme


Yocto tarafından oluşturulan SDK (Software Development Kit) sayesinde, Raspberry Pi 4 gibi hedef cihazlar için uygulamaları host sistemde (örneğin WSL altında çalışan Ubuntu) kolayca derleyebilirsiniz. SDK içinde derleyici, kütüphaneler ve hedef sistemle uyumlu tüm geliştirme araçları yer alır. Böylece, hedef cihaz üzerinde değil, kendi bilgisayarınızda uygulamanızı derleyip çalıştırılabilir ikili dosyalar üretebilirsiniz.

Bu bölümde SDK’yı nasıl kuracağınızı ve nasıl aktif edeceğinizi adım adım göreceksiniz.

Öncelikle Bitbake aktif edin:

```bash
cd yocto-project-rpi/poky
source oe-init-build-env
```

Bitbake ile SDK derleme sürecini başlatın:

```bash
bitbake core-image-base -c populate_sdk
```
> Not: Yocto imaj derlemesi yaparken hangi derleme çeşidini kullandıysanız ona göre ayarlama yapmalısınız. Örneğin: bitbake core-image-sato -c populate_sdk

SDK derlemesi zaman alacaktır. Derleme sürecinde kapatmadan bekleyin.

Ardından SDK'nın oluşturulduğu klasöre gidelim.
```bash
cd tmp/deploy/sdk/
ls
```
Eğer SDK başarıyla oluşturulduysa .sh uzantılı bir adet dosya göreceksiniz. Bizim projemiz için bu poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh isminde. Bu SDK kurulum dosyasıdır. Bu dosyayı çalıştırarak kurulumu tamamlayabiliriz. 

```bash
chmod +x poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh
./poky-glibc-x86_64-core-image-base-cortexa72-raspberrypi4-64-toolchain-4.0.26.sh
```

Kurulum esnasında size SDK install directory soracaktır. Eğer varsayılan klasör ile devam etmek isterseniz boş bırakınız. Özel bir klasör istiyorsanız klasör ismi girebilirsiniz. Varsayılan olarak /opt/poky/4.0.26/ klasörüne kurulur.

Kurulum tamamlandıktan sonra SDK'yı kullanmak için:

```bash
source /opt/poky/4.0.26/environment-setup-cortexa72-poky-linux
```
komutunu giriniz. Artık derleme için gerekli include yolları ayarlanır, imajınıza uygun derlemeler yapabilirsiniz. SDK'nın aktif olduğunu kontrol etmek için:

```bash
echo $CC
```
Çıktı `aarch64-poky-linux-gcc...` şeklinde olmalıdır. Eğer bunu görüyorsanız tüm işlemler sorunsuz tamamlanmıştır.

> Not: WSL'i her yeniden başlatmanızda SDK ortamını yeniden aktifleştirmeniz gerekir. Ayrıca, SDK aktifken Bitbake aktif edilemez.

# LVGL Ortamının Kurulumu
LVGL uygulamaları oluşturabilmek ve derlemek için uygun bir ortam hazırlayacağız. Ardından conf dosyalarında ayarlamalar yaparak kendi yazılımlarımızı istediğimiz şekilde derleyeceğiz.

Öncelikle çalışmalarımızı yapacağımız klasörü oluşturalım:

```bash
mkdir lvgl-project-rpi
cd lvgl-project-rpi
```

Ana yapının hazır olduğu LVGL linux projesini indirelim:

```bash
git clone https://github.com/lvgl/lv_port_linux.git
cd lv_port_linux/
git submodule update --init --recursive
```

Ortamın çalışmasını test etmek için hiçbir ayara müdahale etmeden demo uygulamasını derleyebilirsiniz:

```bash
cmake -B build -S .
make -C build -j
```

Örnek uygulama **lvgl-project-rpi\lv_port_linux\build\bin** klasöründe olacaktır. Bu uygulama SDK'nız ile derlendiği için Windows sisteminizde çalıştıramazsınız.

## Kendi LVGL Uygulamamızı Derlemek

Aşağıdaki kaynaklardan LVGL’nin nasıl kullanılacağına dair daha fazla bilgi alabilir ve örnek uygulamaları inceleyebilirsiniz:

- 📘 [LVGL Resmi Belgeleri](https://docs.lvgl.io/latest/en/html/index.html)  
- 💻 [LVGL PC Simulator (Geliştirme ve Test)](https://github.com/lvgl/lv_port_pc_visual_studio)  


Bu kaynaklar üzerinden temel widget kullanımını, olay (event) sistemini ve stil (style) yapılarını öğrenebilir, sonrasında kendi uygulamanızı oluşturabilirsiniz.

Kendi örnek uygulamamızı Windows Simulator üzerinde test ederek geliştirdik. **my_app.cpp** ve **my_app.h** olmak üzere 2 dosyadan oluşan basit bir arayüzümüz var. Bu arayüzü derleyebilmek için dosyalarımızı `src` klasörü altına yerleştiriyoruz.


LVGL simülatörü olarak yapılandırılmış bu `main.c` dosyasında, varsayılan olarak `lv_demo_widgets()` ve `lv_demo_widgets_start_slideshow()` fonksiyonları çağrılarak LVGL'nin örnek demoları başlatılır. Kendi arayüzünüzü çalıştırmak için bu fonksiyonlar yerine kendi oluşturduğunuz `my_app()` fonksiyonunu çağırmanız gerekir.

Bunun için aşağıdaki adımları uygulayabilirsiniz:

---

`my_app.h` dosyasını dahil edin:

- `main.c` içinde diğer `#include` satırlarının altına aşağıdaki satırı ekleyin:

```c
#include "my_app.h"
```

---

Aşağıdaki satırları bulun ve **yorum satırı haline getirin**:

```c
// lv_demo_widgets();
// lv_demo_widgets_start_slideshow();
```

Bunun yerine kendi fonksiyonunuzu ekleyin:

```c
my_app();
```

### lv_conf.h Ayarları

`lv_conf.h` dosyasında derlemeye nelerin dahil edilip edilmeyeceği konusunda geniş çaplı ayarlar mevcuttur. Dosyanın default halinde standart widgetların pek çoğu aktif durumdadır. Kendi uygulamanızda kullandığınız LVGL widgetlarının `#define LV_USE_CHECKBOX   1` gibi bulunduğundan emin olun. Eğer değeri 0 ise, 1 olarak güncelleyin. Uygulamanın daha düşük kaynak tüketmesi için kullanmadığınız tüm widgetların değerini 0 olarak düzenleyebilirsiniz.


### ⚙️ CMakeLists.txt Dosyasının Düzenlenmesi

Kendi yazdığınız `my_app.c` dosyasını derleyebilmek için `CMakeLists.txt` dosyasına birkaç küçük ama önemli düzenleme yapmanız gerekir. Bu dosya, derleme sistemine hangi kaynak dosyaların kullanılacağını, programın adını ve hangi kütüphanelerin bağlanacağını söyler.

---

Öncelikle `CMakeLists.txt` dosyasında şu satırı bulun:

```cmake
add_executable(lvglsim src/main.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
```

Bu satır, oluşturulacak programın adını (`lvglsim`) ve hangi dosyaların derleneceğini belirtir.  
Burada `src/main.c` ana dosyanızdır. Siz kendi arayüzünüzü `my_app.c` içinde yazdığınız için bu satıra `src/my_app.c` dosyasını da eklemelisiniz.

Aşağıdaki gibi güncelleyin:

```cmake
add_executable(lvglsim src/main.c src/my_app.c ${LV_LINUX_SRC} ${LV_LINUX_BACKEND_SRC})
```

> Not: Eğer uygulamanızın adı `my_app.cpp` ise `.cpp` olarak yazmalısınız. C dosyasıysa `.c` yazın.

### Görüntüleme Altyapısı Ayarları

LVGL'nin Linux üzerinde çalışabilmesi için görüntülerin ekrana nasıl aktarılacağını belirleyen bir sistem kullanılır. Buna "backend" denir. Örneğin:

- Geliştirme sırasında test yapmak için genelde **SDL** kullanılır. Bu ekranda bir pencere açar.
- Gerçek cihazlarda doğrudan ekrana çıkış almak için **Framebuffer (FBDEV)** tercih edilir.

Bu sistemlerden hangisinin kullanılacağı `lv_conf.h` dosyasından belirlenir. Aşağıdaki gibi sadece bir tanesini açık bırakmalısınız:

```c
#define LV_USE_SDL 1
#define LV_USE_LINUX_FBDEV 0
```

Yani burada **SDL açık**, FBDEV **kapalı**dır.

Hangi sistemin açık olduğunu anlamak ve değiştirmek için sadece bu dosyayı açıp satırların başındaki `1` ve `0` değerlerini değiştirmeniz yeterlidir. Birden fazla sistem aynı anda açık olursa çakışma olabilir, bu yüzden sadece birini açık bırakmalısınız.

Görüntüleme sistemi seçildikten sonra (örneğin SDL veya FBDEV), bu sistemin başlatılması `main.c` dosyasında yapılır. Kod içinde şu satır bulunur:

```c
driver_backends_init_backend(...);
```

Bu satır, seçtiğiniz backend'i otomatik olarak başlatır. CMakeLists.txt dosyasında seçilen backend'i derlemeye dahil etmek için gerekli ayarlar mevcuttur. Yani siz sadece `lv_conf.h` dosyasından kullanmak istediğiniz sistemi seçin, gerisini sistem sizin yerinize halleder.


Yaptığınız değişikliklerin geçerli olması için projeyi yeniden derlemeyi unutmayın:

```bash
cmake -B build -S .
make -C build -j
```

Oluşan çıktıyı **build/bin** dizininde bulabilirsiniz. Daha fazla gelişmiş ayar için LVGL geliştirici dökümanlarını inceleyebilirsiniz.

Derlenen uygulamanız Raspberry Pi cihazınızda çalıştırılmaya hazırdır. SSH ile dosyanızı cihaza gönderebilir ve çalıştırabilirsiniz.


