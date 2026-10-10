# Mainan Robot Striker Humanoid Soccer ICHIRO ITS

## Gambaran Umum

Proyek ini merupakan simulasi permainan sepak bola robot dua dimensi berbasis terminal yang dibuat menggunakan C++17 dengan pendekatan Object-Oriented Programming (OOP). Robot berperan sebagai striker yang bertugas mencari bola, mendekati bola, menentukan posisi dan arah tendangan, kemudian menendang bola hingga masuk ke gawang lawan.

Simulasi berjalan menggunakan sistem tick, dengan satu tick mewakili satu detik. Setiap langkah robot mengikuti tiga tahapan utama, yaitu Sense untuk menerima informasi dari sensor, Think untuk menentukan keputusan, dan Act untuk menjalankan aksi.

## Status Pengembangan

Proyek ini telah menyelesaikan tiga tingkat pengembangan. Tingkat pertama mencakup pembuatan Field, Ball, Simulator, kelas abstrak Robot, dan Striker, beserta penerapan enkapsulasi, inheritance, method overriding, dan prinsip DRY.

Tingkat kedua menambahkan composition antara Robot dan Sensor serta exception handling untuk menangani aksi yang tidak valid.

Tingkat ketiga mencakup pembacaan konfigurasi dari file, penerapan State Pattern untuk mengatur perilaku robot, dan unit testing. Seluruh tingkat pengembangan tersebut telah diselesaikan.

## Fitur Utama

Program memungkinkan pengguna memasukkan koordinat robot, arah hadap robot, dan posisi bola melalui keyboard. Pengguna juga dapat menggunakan file konfigurasi untuk menentukan kondisi awal simulasi. Jika input tidak sesuai ketentuan, program akan menolaknya dan meminta pengguna memasukkan data kembali.

Apabila bola keluar dari lapangan, bola akan muncul kembali di titik tengah lapangan sehingga robot dapat melanjutkan permainan. Tampilan simulasi juga diperbarui pada posisi terminal yang sama agar pergerakan robot lebih mudah diamati.

Robot dapat menentukan strategi tendangannya secara otomatis. Sistem memilih posisi berdiri, arah hadap, dan salah satu dari tiga arah tendangan, yaitu lurus, diagonal kiri, atau diagonal kanan.

## Cara Menjalankan Program

Program menggunakan standard library C++17 sehingga tidak membutuhkan library eksternal tambahan. Pengguna dapat melakukan kompilasi menggunakan g++ atau CMake.

Pada Linux, macOS, atau Git Bash, program dapat dikompilasi dengan perintah berikut.

```bash
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/*.cpp -o striker_sim
```

Untuk Windows menggunakan PowerShell dan MinGW atau MSYS2, perintah kompilasinya adalah sebagai berikut.

```powershell
mkdir build -Force
g++ -std=c++17 -O2 -Iinclude (Get-ChildItem src\*.cpp).FullName -o build\striker_sim.exe
```

Setelah kompilasi selesai, program dapat dijalankan dengan beberapa pilihan. Tanpa argumen tambahan, program akan meminta pengguna memasukkan koordinat robot dan bola. Opsi `--default` menjalankan skenario bawaan, sedangkan nama file konfigurasi digunakan untuk menjalankan skenario tertentu.

Opsi `--animate 300` mengatur jeda animasi menjadi 300 milidetik per tick. Opsi `--no-clear` menampilkan setiap frame secara berurutan ke bawah, sedangkan `--quiet` hanya menampilkan frame terakhir dan hasil simulasi.

Pada Windows, gunakan `.\build\striker_sim.exe` sebagai pengganti nama executable biasa.

Program menggunakan kode keluar 0 ketika berhasil mencetak gol, kode 1 ketika tidak mencetak gol hingga batas waktu berakhir, dan kode 2 ketika konfigurasi tidak valid.

## Input dan Sistem Koordinat

Lapangan menggunakan sistem koordinat dengan titik pusat di posisi (0, 0). Pengguna perlu memasukkan lima nilai, yaitu koordinat x dan y robot, arah hadap robot, serta koordinat x dan y bola.

Nilai x berada pada rentang -4,5 hingga 4,5 meter, sedangkan nilai y berada pada rentang -3 hingga 3 meter. Arah hadap robot harus merupakan kelipatan 90 derajat. Arah 0 derajat menghadap ke timur atau gawang lawan, 90 derajat menghadap ke utara, 180 derajat menghadap ke barat, dan -90 derajat menghadap ke selatan.

Koordinat dapat menggunakan angka desimal, tetapi posisi akhirnya dibulatkan ke pusat petak terdekat dengan ukuran 0,5 meter. Program menolak input yang bukan angka, berada di luar lapangan, menggunakan arah hadap yang tidak sesuai, atau menempatkan robot dan bola pada petak yang sama.

Tampilan terminal menggunakan kode ANSI. Karena itu, program sebaiknya dijalankan melalui terminal modern seperti Windows Terminal, terminal VS Code, Linux, atau macOS. Jika karakter kontrol muncul sebagai teks, gunakan opsi `--no-clear`.

## Pengujian Program

Proyek ini dilengkapi unit test untuk memeriksa fungsi dan perilaku sistem. Pengujian dapat dikompilasi dengan g++ atau dijalankan melalui CMake. Hasil yang diharapkan adalah 49 dari 49 pengujian berhasil.

Output simulasi menampilkan nomor tick, state robot, aksi yang sedang dijalankan, posisi robot, arah hadap, dan posisi bola. Lapangan direpresentasikan menggunakan grid berukuran 18 × 12 petak, dengan setiap petak mewakili 0,5 meter.

Simbol R digunakan untuk robot, O untuk bola, @ untuk area pandang kamera, titik untuk area kosong, dan tanda pagar untuk gawang. Pesan tambahan akan ditampilkan apabila aksi ditolak atau bola harus dimunculkan kembali di tengah lapangan.

## Asumsi dan Aturan Simulasi

Robot bergerak pada grid dengan arah hadap berupa kelipatan 90 derajat. Dalam satu tick, robot dapat bergerak maju maksimal satu petak atau 0,5 meter dan berputar maksimal 90 derajat. Gerakan diagonal tidak diperbolehkan karena jaraknya melebihi batas pergerakan per tick.

Kamera robot memiliki area pandang berbentuk segitiga dengan tinggi 1,5 meter dan lebar alas 3,5 meter. Area ini mencakup 15 petak. Petak yang berada tepat di depan robot menjadi posisi bola yang ideal untuk ditendang, sementara tiga petak terdekat digunakan untuk menentukan pilihan arah tendangan.

Bola bergerak sejauh 3 meter pada tick pertama, kemudian melambat dengan jarak 2 meter dan 1 meter pada tick berikutnya. Setelah itu, bola berhenti dan posisinya disesuaikan ke pusat petak terdekat. Jika bola keluar lapangan, bola akan muncul kembali di petak tengah.

Gol dinyatakan berhasil apabila bola melewati garis x = 4,5 meter dengan nilai absolut y tidak lebih dari 1,5 meter.

Dalam penerapan sistem sensor, robot tidak membaca posisi bola secara langsung dari simulator. Robot hanya menerima informasi persepsi yang dihasilkan sensor berdasarkan area pandang kamera. Selanjutnya, robot memperkirakan posisi bola berdasarkan posisi dan arah hadapnya sendiri, kemudian menyimpan hasil perkiraan tersebut sebagai memori.

Robot juga memiliki model fisika tendangan internal. Model ini digunakan oleh KickPlanner untuk memperkirakan posisi akhir bola tanpa membaca keadaan internal simulator secara langsung.

## Alur Pengambilan Keputusan Robot

Perilaku robot diatur menggunakan State Pattern yang membagi proses permainan menjadi empat state utama, yaitu SearchState, ApproachState, AlignState, dan KickState.

Pada SearchState, robot mencari bola dengan berpatroli menuju enam titik pengamatan atau waypoint. Di setiap titik, robot berputar untuk memindai empat arah sehingga dapat mengamati seluruh lapangan.

Ketika bola ditemukan, robot berpindah ke ApproachState. Pada tahap ini, robot meminta rencana tendangan dari KickPlanner dan mencari jalur menuju posisi tembak menggunakan algoritma Breadth-First Search atau BFS. Jalur yang dipilih menghindari petak yang ditempati bola.

Setelah mencapai posisi tembak, robot memasuki AlignState. Robot menyesuaikan arah hadapnya berdasarkan rencana yang telah dibuat, kemudian memastikan melalui kamera bahwa bola berada tepat di petak depan. Jika posisi dan arah sudah benar, robot dapat melanjutkan ke tahap penendangan.

Pada KickState, robot menendang bola sesuai arah yang telah dipilih. Robot kemudian menyimpan perkiraan posisi akhir bola dan menunggu selama dua tick sebelum kembali mencari bola. Setelah bola ditemukan kembali, robot melanjutkan proses mendekati dan menendangnya menuju gawang.

Jika bola menghilang dari pengamatan atau rencana tendangan tidak lagi sesuai, robot dapat kembali ke state sebelumnya untuk menyusun strategi baru. Perpindahan state juga dirancang agar tidak membuang tick tambahan ketika robot hanya perlu berganti state.

## Strategi Perencanaan Tendangan

KickPlanner berfungsi menentukan rangkaian tendangan yang dapat membawa bola menuju gol dengan jumlah tendangan minimum.

Saat dibuat, planner menghitung kebutuhan tendangan dari setiap kemungkinan posisi bola berdasarkan berbagai kombinasi posisi berdiri dan arah tendangan. Perhitungan dilakukan menggunakan dynamic programming dengan memanfaatkan prediksi fisika bola.

Ketika menentukan rencana, robot hanya memilih tendangan yang termasuk dalam jalur optimal, yaitu tendangan yang mengurangi jumlah sisa tendangan minimum sebanyak satu. Jika terdapat beberapa pilihan, robot memilih rencana yang membutuhkan biaya perjalanan paling rendah, berdasarkan pergerakan dan rotasi robot.

Strategi ini membantu robot menghindari pergerakan berulang tanpa kemajuan.

Ketika bola ditendang keluar lapangan, planner memperhitungkan bahwa bola akan kembali ke tengah. Dengan mekanisme tersebut, seluruh 216 kemungkinan posisi bola dapat diselesaikan, termasuk posisi di sudut lapangan yang sebelumnya sulit dijangkau.

## Skenario Pengujian

Program telah diuji menggunakan beberapa skenario. Skenario bawaan berhasil mencetak gol pada tick ke-25. Ketika bola berada di depan robot dan segaris dengan gawang, gol tercapai pada tick ke-3.

Untuk kondisi bola berada di belakang robot, program berhasil mencetak gol pada tick ke-74. Skenario tendangan berantai ketika bola berada jauh dari gawang berhasil pada tick ke-28, sedangkan skenario bola di dekat dinding berhasil pada tick ke-40.

Pada skenario bola berada di sudut lapangan, bola ditendang keluar, muncul kembali di tengah, lalu berhasil menghasilkan gol pada tick ke-56. Sementara itu, konfigurasi dengan arah hadap 45 derajat ditolak dan menghasilkan pesan kesalahan dengan kode keluar 2.

Pengujian menyeluruh dilakukan pada 216 petak posisi bola dengan enam posisi awal robot. Sebanyak 1.290 skenario berhasil berakhir dengan gol dalam batas maksimal 90 tick, termasuk 562 skenario yang melibatkan kemunculan kembali bola di tengah lapangan.

Selain itu, unit test `striker_scores_from_every_interior_ball_cell` digunakan untuk memastikan bahwa seluruh 160 petak bagian dalam lapangan dapat menghasilkan gol.

## Exception Handling

Program menerapkan hierarki exception untuk menangani kesalahan saat robot melakukan aksi. Jenis kesalahan yang ditangani meliputi pelanggaran batas kecepatan, batas rotasi, batas lapangan, jalur yang terhalang, dan kondisi ketika tendangan tidak dapat dilakukan.

Simulator melempar exception melalui `executeAction()`. Selanjutnya, `step()` menangkap kesalahan tersebut, mencatatnya, dan memanggil `Robot::onActionRejected()` agar robot dapat membatalkan rencana yang tidak valid.

Kesalahan pada file konfigurasi atau posisi awal ditangani menggunakan `ConfigException`. Fungsi utama program kemudian menangkap exception tersebut dan menampilkan pesan kesalahan yang jelas kepada pengguna.

## Struktur Proyek

Folder `include` berisi deklarasi kelas dan header untuk komponen seperti Field, Ball, Sensor, Robot, Striker, StrikerState, KickPlanner, Navigator, dan Simulator.

Folder `src` berisi implementasi setiap komponen serta `main.cpp`, yang menangani input pengguna dan pilihan perintah program.

Folder `tests` berisi unit test dengan framework sederhana yang dibuat tanpa library eksternal. Folder `scenarios` menyimpan berbagai skenario pengujian dan file konfigurasi bawaan. Dokumentasi diagram kelas tersedia pada `docs/class_diagram.md`.

## Penerapan Object-Oriented Programming

Proyek ini menerapkan beberapa prinsip utama OOP.

Encapsulation digunakan dengan menjadikan atribut Robot bersifat private dan menyediakan akses melalui getter serta setter. Validasi dilakukan untuk mencegah nilai yang tidak sesuai, dengan exception dilempar apabila terjadi pelanggaran aturan.

Inheritance dan polymorphism diterapkan melalui kelas Striker yang mewarisi kelas Robot. Kelas Robot menyediakan method abstrak `think()` yang implementasinya ditentukan oleh kelas turunannya. Perilaku robot juga dikelola menggunakan kelas-kelas state.

Composition digunakan dengan menjadikan Sensor sebagai bagian dari Robot. Simulator juga memiliki komponen Field, Ball, dan Robot untuk membentuk keseluruhan lingkungan permainan.

Prinsip DRY atau Don't Repeat Yourself diterapkan dengan memusatkan operasi matematika seperti perhitungan jarak, arah, normalisasi sudut, dan transformasi koordinat pada komponen yang digunakan bersama. Logika fisika bola juga digunakan kembali oleh simulator dan planner agar pergerakan aktual sesuai dengan prediksi strategi.

## Pernyataan Penggunaan AI

AI digunakan dalam pengembangan dari sebagian penulisan source code dan referensi mekanisme robot agar lebih presisi, terutama logika input dari user dan revisi pada main.cpp serta planner untuk membuat program lebih sesuai dengan yang ditugaskan. AI juga digunakan untuk mencari referensi dan validasi dalam pembuatan class diagram. Program kemudian dijalankan, diuji, dan dipelajari oleh pengembang.
