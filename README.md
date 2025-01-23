# Barkod Takip Sistemi

Modern ve kullanıcı dostu bir barkod takip sistemi. Son kullanma tarihi yaklaşan ürünleri kolayca takip edebilir, toplu düzenlemeler yapabilir ve detaylı raporlar alabilirsiniz.

## Özellikler

- 🎯 Kolay ve hızlı ürün ekleme
- 📊 Sezgisel kullanıcı arayüzü
- 🔍 Gelişmiş arama ve filtreleme
- 📅 Son kullanma tarihi takibi
  - Renk kodlu uyarı sistemi (kırmızı: <10 gün, sarı: <30 gün)
  - Otomatik bildirimler
- ✨ Modern ve koyu tema desteği
- 🔄 Toplu düzenleme ve silme işlemleri
- 📝 Detaylı loglama sistemi
- 💾 Otomatik veri yedekleme

## Kurulum

### Son Kullanıcı İçin

1. [Releases](https://github.com/aliyldm/BarkodTakip/releases) sayfasından en son sürümü indirin
2. İndirilen ZIP dosyasını istediğiniz bir konuma çıkarın
3. `BarkodTakip.exe` dosyasını çalıştırın

### Geliştiriciler İçin

#### Gereksinimler

- Windows 10/11
- Qt 6.5.0 veya üzeri
- CMake 3.16 veya üzeri
- Visual Studio 2019/2022 (MSVC derleyicisi ile)

#### Derleme

```bash
# Projeyi klonlayın
git clone https://github.com/aliyldm/BarkodTakip.git
cd BarkodTakip

# Build klasörü oluşturun
mkdir build && cd build

# CMake ile proje dosyalarını oluşturun
cmake .. -G "Visual Studio 17 2022" -A x64

# Projeyi derleyin
cmake --build . --config Release
```

## Kullanım

### Ürün Ekleme

1. Ana ekranda barkod, marka ve son kullanma tarihi bilgilerini girin
2. "Ekle" butonuna tıklayın

### Toplu Düzenleme

1. Ana ekranda "Toplu Düzenle" butonuna tıklayın
2. Düzenlemek istediğiniz ürünleri seçin
3. Yapılacak değişiklikleri belirtin ve "Uygula" butonuna tıklayın

### Raporlama

- Son kullanma tarihi yaklaşan ürünler otomatik olarak renklendirilir:
  - 🔴 Kırmızı: 10 günden az kaldı
  - 🟡 Sarı: 30 günden az kaldı
  - ⚪ Normal: 30 günden fazla var

## Veri Depolama

- Ürün verileri JSON formatında saklanır
- Varsayılan konum: `%APPDATA%/BarkodTakip/products.json`
- Log dosyaları: `%APPDATA%/BarkodTakip/logs/`

## Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın.

## Katkıda Bulunma

1. Bu depoyu fork edin
2. Yeni bir branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add some amazing feature'`)
4. Branch'inizi push edin (`git push origin feature/amazing-feature`)
5. Bir Pull Request oluşturun

## İletişim

Ali YILDIRIM - [@aliyldm](https://github.com/aliyldm)

Proje Linki: [https://github.com/aliyldm/BarkodTakip](https://github.com/aliyldm/BarkodTakip) 