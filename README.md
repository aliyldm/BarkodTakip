# Barkod Takip Sistemi

Ürünlerin barkod numaralarını ve son kullanma tarihlerini takip etmek için geliştirilmiş bir masaüstü uygulaması.

## Özellikler

- Barkod ve marka bilgisi girişi
- Son kullanma tarihi takibi
- Renk kodlu uyarı sistemi:
  - 10 günden az kalan ürünler: Kırmızı
  - 10-30 gün arası kalan ürünler: Sarı
- Toplu düzenleme ve silme işlemleri
- Otomatik veri kaydetme
- Modern ve kolay kullanılabilir arayüz

## Gereksinimler

- Qt 6.x
- CMake 3.16 veya üstü
- C++17 uyumlu derleyici

## Kurulum

### Geliştirme için:

1. Repository'yi klonlayın:
```bash
git clone https://github.com/KULLANICIADI/barkod-takip.git
cd barkod-takip
```

2. Build klasörü oluşturun:
```bash
mkdir build
cd build
```

3. CMake ile projeyi yapılandırın:
```bash
cmake ..
```

4. Projeyi derleyin:
```bash
cmake --build .
```

### Son Kullanıcı için:

1. En son sürümü [Releases](https://github.com/KULLANICIADI/barkod-takip/releases) sayfasından indirin
2. BarkodTakip.zip dosyasını açın
3. barcode_tracker.exe dosyasını çalıştırın

Not: Windows'ta Visual C++ Redistributable gerekebilir.

## Kullanım

1. Ürün eklemek için:
   - Barkod numarasını girin
   - Marka adını girin
   - Son kullanma tarihini seçin
   - "Ekle" butonuna tıklayın

2. Ürünleri düzenlemek için:
   - Tek ürün: Ürünü seçip "Düzenle" butonunu kullanın
   - Toplu düzenleme: "Toplu Düzenle" butonu ile birden fazla ürünü aynı anda düzenleyin

3. Ürünleri silmek için:
   - Tek ürün: Ürünü seçip "Sil" butonunu kullanın
   - Toplu silme: "Toplu Sil" butonu ile birden fazla ürünü aynı anda silin

## Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın. 