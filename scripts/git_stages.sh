#!/usr/bin/env bash
# Membuat riwayat git bertahap (satu commit per tahap pengerjaan).
# Jalankan dari root proyek:   bash scripts/git_stages.sh
# Tips: jangan jalankan semuanya sekaligus kalau ingin riwayat yang jujur terhadap waktu.
# Jalankan satu tahap per sesi kerja:  bash scripts/git_stages.sh 3   (hanya tahap 3)
set -e
[ -d .git ] || git init -b main

commit_stage() {  # $1=nomor  $2=pesan  sisanya=file
  local n="$1" msg="$2"; shift 2
  if [ -n "$ONLY" ] && [ "$ONLY" != "$n" ]; then return; fi
  git add "$@"
  git commit -m "$msg" || true
  echo ">> tahap $n selesai"
}
ONLY="$1"

commit_stage 1 "feat: scaffold proyek, helper matematika (DRY), Field 9x6 m grid 0.5 m, test framework" \
  .gitignore CMakeLists.txt include/MathUtils.hpp include/Field.hpp src/Field.cpp \
  tests/test_framework.hpp tests/test_main.cpp tests/test_math.cpp tests/test_field.cpp

commit_stage 2 "feat: Ball dengan fisika 3-2-1 m/tick, deteksi gol, predictKick + unit test" \
  include/Ball.hpp src/Ball.cpp tests/test_ball.cpp

commit_stage 3 "feat: Sensor kamera segitiga 3.5 x 1.5 m (Perception, tanpa akses langsung ke bola)" \
  include/Sensor.hpp src/Sensor.cpp tests/test_sensor.cpp

commit_stage 4 "feat: abstract Robot (encapsulation, HAS-A Sensor), Action, hierarki exception, Navigator BFS" \
  include/Exceptions.hpp include/Action.hpp src/Action.cpp include/Robot.hpp src/Robot.cpp \
  include/Navigator.hpp src/Navigator.cpp tests/test_robot.cpp

commit_stage 5 "feat: KickPlanner - DP jumlah tendangan minimum ke gol + pemilihan posisi tembak" \
  include/KickPlanner.hpp src/KickPlanner.cpp tests/test_planner.cpp

commit_stage 6 "feat: Striker dengan State Pattern (Search, Approach, Align, Kick)" \
  include/StrikerState.hpp include/Striker.hpp include/StrikerStates.hpp src/Striker.cpp src/StrikerStates.cpp

commit_stage 7 "feat: file konfigurasi (ConfigLoader) + skenario uji" \
  include/SimulationConfig.hpp include/ConfigLoader.hpp src/ConfigLoader.cpp tests/test_config.cpp \
  config.txt scenarios

commit_stage 8 "feat: Simulator (game loop Sense-Think-Act, try-catch aksi ilegal), Renderer ASCII, main CLI" \
  include/Renderer.hpp src/Renderer.cpp include/Simulator.hpp src/Simulator.cpp src/main.cpp tests/test_simulator.cpp

commit_stage 9 "docs: README, class diagram, alur state, pernyataan penggunaan AI" \
  README.md docs scripts
