# OMEN RGB Driver - Basit Makefile
# Sadece çalışsın yeter!

KERNEL_VERSION := $(shell uname -r)
KERNEL_DIR := /lib/modules/$(KERNEL_VERSION)/build
PWD := $(shell pwd)

# Module tanımı
obj-m += omen_kernel_mainline_final.o

# Basit CFLAGS
ccflags-y += -Wall -O2

# Ana hedef
all:
	@echo "🔨 OMEN RGB driver build ediliyor..."
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
	@if [ -f "omen_kernel_mainline_final.ko" ]; then \
		echo "✅ Build başarılı!"; \
		ls -la omen_kernel_mainline_final.ko; \
	else \
		echo "❌ Build başarısız!"; \
	fi

# Test - sadece build kontrolü
test: all
	@echo "🧪 Basit test..."
	@if [ -f "omen_kernel_mainline_final.ko" ]; then \
		echo "✅ Module dosyası var"; \
		modinfo omen_kernel_mainline_final.ko | head -5; \
	else \
		echo "❌ Module dosyası yok"; \
	fi

# Kurulum
install: all
	@echo "📦 Kurulum..."
	@if [ $$(id -u) -ne 0 ]; then \
		echo "❌ Root gerekli: sudo make install"; \
		exit 1; \
	fi
	cp omen_kernel_mainline_final.ko /lib/modules/$(KERNEL_VERSION)/extra/ || mkdir -p /lib/modules/$(KERNEL_VERSION)/extra/ && cp omen_kernel_mainline_final.ko /lib/modules/$(KERNEL_VERSION)/extra/
	depmod -a
	@echo "✅ Kurulum tamam"

# Yükleme
load: install
	@echo "🚀 Module yükleniyor..."
	@if [ $$(id -u) -ne 0 ]; then \
		echo "❌ Root gerekli: sudo make load"; \
		exit 1; \
	fi
	-rmmod omen_kernel_mainline_final 2>/dev/null
	modprobe omen_kernel_mainline_final
	@echo "✅ Module yüklendi"
	@if [ -f /proc/omen_rgb ]; then \
		echo "✅ /proc/omen_rgb hazır"; \
	else \
		echo "⚠️  /proc/omen_rgb yok - dmesg kontrol et"; \
	fi

# Kaldırma
unload:
	@echo "🛑 Module kaldırılıyor..."
	@if [ $$(id -u) -ne 0 ]; then \
		echo "❌ Root gerekli: sudo make unload"; \
		exit 1; \
	fi
	-rmmod omen_kernel_mainline_final
	@echo "✅ Module kaldırıldı"

# Test - gerçek hardware
hardware_test: load
	@echo "🔴 Hardware test..."
	@if [ $$(id -u) -ne 0 ]; then \
		echo "❌ Root gerekli: sudo make hardware_test"; \
		exit 1; \
	fi
	@if [ -f /proc/omen_rgb ]; then \
		echo "green" > /proc/omen_rgb && echo "✅ Yeşil: OK" || echo "❌ Yeşil: FAIL"; \
		sleep 1; \
		echo "red" > /proc/omen_rgb && echo "✅ Kırmızı: OK" || echo "❌ Kırmızı: FAIL"; \
		sleep 1; \
		echo "blue" > /proc/omen_rgb && echo "✅ Mavi: OK" || echo "❌ Mavi: FAIL"; \
		sleep 1; \
		echo "black" > /proc/omen_rgb && echo "✅ Siyah: OK" || echo "❌ Siyah: FAIL"; \
	else \
		echo "❌ /proc/omen_rgb yok"; \
	fi

# Temizlik
clean:
	@echo "🧹 Temizlik..."
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *.o *.ko *.mod.c *.mod *.order *.symvers .*.cmd
	rm -rf .tmp_versions/
	@echo "✅ Temizlik tamam"

# Durum
status:
	@echo "📊 Durum:"
	@if [ -f "omen_kernel_mainline_final.ko" ]; then \
		echo "  ✅ Build: OK"; \
	else \
		echo "  ❌ Build: YOK"; \
	fi
	@if lsmod | grep -q "omen_kernel_mainline_final"; then \
		echo "  ✅ Loaded: OK"; \
	else \
		echo "  ❌ Loaded: YOK"; \
	fi
	@if [ -f "/proc/omen_rgb" ]; then \
		echo "  ✅ Interface: OK"; \
	else \
		echo "  ❌ Interface: YOK"; \
	fi

# Yardım
help:
	@echo "🔧 OMEN RGB Driver - Basit Makefile"
	@echo "=================================="
	@echo ""
	@echo "Komutlar:"
	@echo "  make all           - Build et"
	@echo "  make test          - Basit test"
	@echo "  make install       - Kur (root gerekli)"
	@echo "  make load          - Yükle (root gerekli)"
	@echo "  make unload        - Kaldır (root gerekli)"
	@echo "  make hardware_test - Hardware test (root gerekli)"
	@echo "  make clean         - Temizle"
	@echo "  make status        - Durum göster"
	@echo "  make help          - Bu yardım"
	@echo ""
	@echo "Hızlı kullanım:"
	@echo "  make all && sudo make load"
	@echo "  sudo make hardware_test"
	@echo ""

.PHONY: all test install load unload hardware_test clean status help
