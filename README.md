# Mainan Robot — Striker Humanoid Soccer ICHIRO ITS (C++17, OOP)

Simulasi 2D berbasis terminal: satu robot **Striker** mencari bola, mendatanginya, mengatur posisi tembak,
dan menendang bola ke gawang lawan. Game loop berbasis tick (1 tick = 1 detik) dengan siklus
**Sense → Think → Act**.

## Status Level

| Level | Isi | Status |
|---|---|---|
| 1 | `Field`, `Ball`, `Simulator`, abstract `Robot`, `Striker` (inheritance, override `think()`), enkapsulasi, DRY | ✅ |
| 2 | Composition `Robot HAS-A Sensor`, exception handling (try-catch) untuk aksi tidak valid | ✅ |
| 3 | (1) File konfigurasi ✅ (2) State Pattern ✅ (3) Unit test ✅ | ✅ |

## Build & Run

Hanya Standard Library C++17.

```bash
# Opsi A - g++ langsung
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude src/*.cpp -o striker_sim

# Opsi B - CMake
cmake -S . -B build && cmake --build build
```

```bash
./striker_sim                                # skenario bawaan
./striker_sim config.txt                     # dari file konfigurasi
./striker_sim scenarios/02_ball_behind_robot.txt
./striker_sim config.txt --animate 150       # animasi, 150 ms per tick
./striker_sim config.txt --quiet             # hanya frame akhir + hasil
```

Exit code: `0` = gol, `1` = tidak gol (timeout / menyerah), `2` = konfigurasi salah.

### Unit test

```bash
g++ -std=c++17 -Wall -Wextra -O2 -Iinclude $(ls src/*.cpp | grep -v main.cpp) tests/*.cpp -o run_tests
./run_tests
# atau dengan CMake: cmake --build build && ./build/run_tests
```

## Format output

```
. . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . #
. . . . . . . . . . . . . . . . . . #
. . . . . . . . . @ . . . . . . . . #
. . . . . . . . @ @ . . . . . . . . #
. . . . . . . @ @ @ . . . . . . . . #
. . . . . . R @ @ @ . . . . . . . O #
. . . . . . . @ @ @ . . . . . . . .
. . . . . . . . @ @ . . . . . . . .
. . . . . . . . . @ . . . . . . . .
```

`R` robot · `O` bola · `@` area pandang kamera · `.` kosong · `#` gawang. Elemen dipisahkan spasi,
baris paling atas = y terbesar. Grid 18 × 12 petak (1 petak = 0.5 m).

## Asumsi desain (penting untuk dijelaskan saat review)

Spesifikasi tidak menentukan semuanya, jadi ini keputusan yang saya ambil:

1. **Robot bergerak di grid, heading kelipatan 90°.** Maju = tepat 1 petak (0.5 m, batas maksimum),
   putar maksimum 90°/tick. Gerak diagonal ditolak karena 0.707 m > batas 0.5 m/tick.
2. **Kamera = segitiga** dengan apex di robot, tinggi 1.5 m (3 petak), alas 3.5 m (7 petak) di ujung.
   Sebuah petak terlihat jika titik tengahnya ada di dalam segitiga → 3 + 5 + 7 = **15 petak**.
   Petak di depan robot (tengah segitiga) = petak tendang; 3 petak terdekat = 3 arah tendangan
   (lurus, kiri-diagonal, kanan-diagonal).
3. **Bola**: ditendang 3 m/tick lalu melambat 1 m/tick → jarak 3 + 2 + 1 = **6 m**, berhenti, lalu
   di-snap ke tengah petak. Menabrak dinding = berhenti. Melewati garis x = 4.5 dengan |y| ≤ 1.5 = **gol**.
4. **Sense tanpa curang**: `Robot::sense()` hanya memanggil `Sensor::capture()`. `Striker::think()` hanya
   menerima `Perception` (posisi relatif bola dari kamera). Posisi global bola dibangun robot sendiri dari
   pose + data kamera, dan disimpan sebagai *memori*.
5. **Robot tahu fisika tendangan** (model internal). `KickPlanner` memakai `Ball::predictKick()` untuk
   memprediksi ke mana bola berhenti. Ini bukan membaca Simulator; hanya model aturan yang sama.

## Alur kecerdasan Striker (State Pattern)

```
          bola terlihat                 sampai di posisi tembak
SearchState ───────────▶ ApproachState ─────────────────────▶ AlignState
    ▲   ◀─── bola hilang ───┘   ▲                                  │
    │                           └── bola pindah / rencana batal ───┤
    │                                                              ▼ heading benar &
    └───────── tunggu bola berhenti (2 tick) ────────────── KickState   bola di petak depan
```

| State | Aksi pada tahap Think | Keluar ke |
|---|---|---|
| **SearchState** (`SEARCH_BALL`) | Patroli ke 6 waypoint (jarak 7 petak → kamera menutup seluruh lapangan); di tiap waypoint putar 3 × 90° untuk memindai 4 arah | Approach saat bola masuk memori |
| **ApproachState** (`APPROACH_BALL`) | Minta rencana ke `KickPlanner`, lalu jalan (BFS, menghindari petak bola) ke petak tembak | Align saat tiba; Search jika bola hilang |
| **AlignState** (`ALIGN_TO_GOAL`) | Putar ke heading rencana; verifikasi lewat kamera bahwa bola persis di petak depan | Kick jika benar; Approach jika tidak |
| **KickState** (`KICK`) | Tendang, ingat prediksi tempat bola berhenti, tunggu 2 tick | Search (lalu langsung Approach ke bola) |

Setiap `State::update()` boleh mengembalikan `nullopt` ("saya hanya pindah state"), sehingga transisi
tidak membuang 1 tick.

### Otak strategi: `KickPlanner`

Saat dibuat, planner menghitung untuk **setiap petak bola** jumlah tendangan minimum menuju gol (DP di
atas semua kombinasi posisi berdiri × arah tendangan, hasil tiap tendangan diprediksi oleh fisika bola).
Saat `plan()`, robot hanya memilih tendangan yang berada di *rantai optimal* (tiap tendangan mengurangi
sisa tendangan sebanyak 1), lalu memilih yang paling murah dijangkau (jalan + putar). Hasilnya: tidak
ada bolak-balik tanpa kemajuan.

## Penjelasan skenario uji

| File | Tujuan | Hasil |
|---|---|---|
| `config.txt` | Skenario bawaan | Gol ±tick 25 |
| `scenarios/01_ball_in_front.txt` | Bola sudah terlihat & segaris gawang | Gol tick 3 |
| `scenarios/02_ball_behind_robot.txt` | Bola di belakang robot → harus mencari | Gol |
| `scenarios/03_far_from_goal_multi_kick.txt` | Bola > 6 m dari gawang → tendangan berantai | Gol |
| `scenarios/04_ball_near_wall.txt` | Bola menempel dinding → tendangan diagonal | Gol |
| `scenarios/05_ball_stuck_in_corner.txt` | Kasus batas: bola di pojok kanan-atas | Striker menyerah (terdokumentasi) |
| `scenarios/06_invalid_config.txt` | Config salah (heading 45°) | Pesan error, exit code 2 |

Pengujian menyeluruh (unit test `striker_scores_from_every_interior_ball_cell`): bola di **semua 160 petak
interior** selalu berhasil dicetak menjadi gol. Pada uji sapuan tambahan (semua 216 petak bola × 6 posisi
awal robot = 1290 skenario): 1284 gol (maks 103 tick), 6 "menyerah" — semuanya bola di pojok kanan-atas.

### Kenapa pojok kanan-atas mustahil?
Dengan aturan "robot hanya menendang ke 3 petak di depannya" dan robot harus berdiri tepat di belakang bola,
bola di pojok itu tidak bisa didorong keluar: berdiri di barat/selatan hanya bisa menendang ke arah dinding.
Ini keterbatasan aturan, bukan bug; Striker mendeteksinya (`kicksToGoal == ∞`) dan berhenti.

## Exception handling (Level 2)

Hierarki di `Exceptions.hpp`: `InvalidActionException` ← `SpeedLimitException`, `TurnLimitException`,
`OutOfBoundsException`, `BlockedException`, `KickNotPossibleException`. `Simulator::executeAction()`
melempar; `Simulator::step()` menangkap, mencatat, lalu memanggil `Robot::onActionRejected()` agar Striker
membatalkan rencananya. `ConfigException` dipakai untuk file konfigurasi/posisi awal yang salah.

## Struktur proyek

```
include/   header (Field, Ball, Sensor, Robot, Striker, StrikerState(s), KickPlanner, Navigator, Simulator, ...)
src/       implementasi + main.cpp
tests/     unit test (framework mini sendiri, tanpa library eksternal)
scenarios/ skenario uji      config.txt  skenario bawaan
docs/class_diagram.md        Class Diagram (Mermaid)
```

Class diagram: lihat [`docs/class_diagram.md`](docs/class_diagram.md).

## Prinsip OOP yang dipakai

- **Encapsulation**: atribut `Robot` private, akses via getter/setter yang melempar exception jika tidak valid.
- **Inheritance & Polymorphism**: `Striker : Robot`, `think()` pure virtual; state lewat `StrikerState`.
- **Composition**: `Robot HAS-A Sensor`, `Simulator` memiliki `Field`, `Ball`, `Robot`.
- **DRY**: jarak, bearing, normalisasi sudut, transformasi frame lokal ada di `MathUtils.hpp` dan `Robot`
  (`distanceTo`, `bearingTo`, `stepToward`); fisika bola dipakai bersama oleh simulator dan prediksi planner
  (`Ball::update` ↔ `Ball::predictKick`).

## Pernyataan Penggunaan AI

> **ISI/EDIT BAGIAN INI SESUAI KENYATAAN SEBELUM DIKUMPULKAN.**
>
> Kerangka arsitektur, seluruh source code C++ (`include/`, `src/`), unit test (`tests/`), skenario uji,
> `docs/class_diagram.md`, dan draf README ini dibuat dengan bantuan **Claude (Anthropic)**, kemudian
> dijalankan, diuji, dan dipelajari oleh saya. Bagian yang saya ubah/tulis sendiri: _(sebutkan di sini)_.
