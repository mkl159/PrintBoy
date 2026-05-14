#!/bin/sh
# =============================================================
#  PrintBoy - launcher Onion
#  - charge la config printer.cfg (si absent, en cree un vide)
#  - verifie qu'on a du WiFi (Onion)
#  - lance le binaire
# =============================================================

APPDIR=/mnt/SDCARD/App/PrintBoy
SAVEDIR=/mnt/SDCARD/Saves/PrintBoy
LOG="$SAVEDIR/printboy.log"
CFG="$APPDIR/printer.cfg"

mkdir -p "$SAVEDIR" 2>/dev/null
printf '[%s] === PrintBoy start ===\n' "$(date +%Y-%m-%d_%H:%M:%S 2>/dev/null || echo '?')" >> "$LOG"

# Creer un printer.cfg minimal si absent (premier lancement)
if [ ! -f "$CFG" ]; then
    cat > "$CFG" <<'EOF'
# PrintBoy - config Prusa-Link
# Edite cette URL avec l'IP de ton imprimante (ou prusa.local).
# La cle API se trouve sur l'imprimante :
#   Settings > Network > Prusa Link > API key
url =
api_key =
poll_interval_s = 2
jog_step_mm = 10
EOF
    printf '[%s] config creee par defaut\n' "$(date +%H:%M:%S)" >> "$LOG"
fi

# Verifier que WiFi est active
if [ "$(/customer/app/jsonval wifi 2>/dev/null)" != "1" ]; then
    printf '[%s] WiFi OFF sur Onion - PrintBoy a besoin du WiFi\n' "$(date +%H:%M:%S)" >> "$LOG"
    # Afficher un message d'erreur (cf res/no_wifi.raw eventuel)
    if [ -f "$APPDIR/res/no_wifi.raw" ]; then
        dd if="$APPDIR/res/no_wifi.raw" of=/dev/fb0 bs=1024 count=600 2>/dev/null
        sleep 4
    fi
    # On continue quand meme : l'utilisateur peut vouloir consulter Settings
fi

BIN="$APPDIR/bin/printboy"
if [ ! -f "$BIN" ]; then
    printf '[%s] ERREUR : binaire %s absent. Cf BUILD.md\n' "$(date +%H:%M:%S)" "$BIN" >> "$LOG"
    exit 2
fi
chmod +x "$BIN" 2>/dev/null

export SDL_VIDEODRIVER=mmiyoo
export SDL_AUDIODRIVER=dummy
export LD_LIBRARY_PATH="$APPDIR/lib:/mnt/SDCARD/miyoo/lib:/lib:$LD_LIBRARY_PATH"

cd "$APPDIR"
"$BIN"
RC=$?
printf '[%s] binaire exit code=%d\n' "$(date +%H:%M:%S)" "$RC" >> "$LOG"
exit $RC
