# OMEN RGB Driver - Linux

OMEN/Victus laptoplar için RGB aydınlatma kontrolü.

## Hızlı Başlangıç

### 1. Build Et
```bash
make -f Makefile_simple all
```

### 2. Yükle ve Test Et
```bash
sudo make -f Makefile_simple load
sudo make -f Makefile_simple hardware_test
```

### 3. Kullan
```bash
# Yeşil yap
echo "green" | sudo tee /proc/omen_rgb

# Kırmızı yap  
echo "red" | sudo tee /proc/omen_rgb

# Mavi yap
echo "blue" | sudo tee /proc/omen_rgb

# Beyaz yap
echo "white" | sudo tee /proc/omen_rgb

# Kapat
echo "black" | sudo tee /proc/omen_rgb
```

### 4. Durum Kontrol
```bash
# Driver durumu
make -f Makefile_simple status

# Cihaz bilgisi
cat /proc/omen_rgb
```

## Desteklenen Cihazlar

- HP OMEN laptoplar
- HP Victus laptoplar  
- HP Pavilion Gaming laptoplar
- HP OMEN Desktop'lar

## Gereksinimler

- Linux kernel headers
- GCC
- Root yetkisi (sudo)

### Ubuntu/Debian
```bash
sudo apt-get install linux-headers-$(uname -r) build-essential
```

### Fedora/RHEL
```bash
sudo dnf install kernel-devel gcc make
```

## Sorun Giderme

### Driver yüklenmiyor
```bash
# Kernel log kontrol et
dmesg | tail -20

# OMEN cihazı var mı kontrol et
sudo dmidecode | grep -i "omen\|victus\|pavilion"
```

### /proc/omen_rgb yok
```bash
# Module yüklü mü kontrol et
lsmod | grep omen

# Yeniden yükle
sudo make -f Makefile_simple unload
sudo make -f Makefile_simple load
```

### Renk değişmiyor
```bash
# ACPI hatası var mı kontrol et
dmesg | grep -i "omen\|acpi"

# Farklı renk dene
echo "green" | sudo tee /proc/omen_rgb
```

## Kaldırma

```bash
# Module'ü kaldır
sudo make -f Makefile_simple unload

# Dosyaları temizle
make -f Makefile_simple clean
```

## Yardım

```bash
make -f Makefile_simple help
```

## Lisans

GPL v2