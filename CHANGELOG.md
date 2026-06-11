# 📝 Changelog

## [0.3.0] - 2026-06-11

### ✨ Ajouts

- 🧵 **Polling asynchrone** : le `GET /api/v1/status` tourne dans un thread SDL dédié. L'UI ne gèle plus jamais (avant : jusqu'à 10 s de freeze à chaque poll si l'imprimante ne répondait pas). Fallback synchrone si le thread ne démarre pas.
- 🛑 **Confirmation avant annulation** : « Annuler l'impression » demande un **double appui sur A** dans les 3 s (le bouton devient rouge). Fini l'impression de 8 h ruinée par un appui accidentel.
- 🔥 **Préchauffage PETG** (230/85) en plus du PLA
- 📷 **Webcam : ratio d'aspect préservé** (letterbox centré au lieu d'étirer l'image en 560×360)
- ⏱️ **Indicateur de fraîcheur** sur le Dashboard (« maj Ns ») — utile maintenant que le poll est asynchrone
- 🏷️ **Version affichée** dans l'écran Settings

### 🔒 Sécurité / robustesse

- 🧷 `CURLOPT_NOSIGNAL` activé : obligatoire en multi-thread (sans ça, les timeouts DNS de libcurl utilisent `SIGALRM` et peuvent tuer le process)
- 🔁 Redirections HTTP **bornées** (`MAXREDIRS=3`) et **limitées à http/https** (un serveur malveillant ne peut plus rediriger vers `file://` ou autre)
- 🔐 Les écritures de config (URL/clé) sont protégées par mutex (le thread de poll les lit)

### 🐛 Corrections

- 🕹️ L'écran Jog affichait littéralement `nan` pour X/Y quand Prusa-Link ne les rapporte pas (cas fréquent) → affiche `--`

## [0.2.0] - 2026-06-02

### ✨ Ajouts

- 🔥 Écran Controls : préchauffage **PLA** (215/60) et **Refroidir tout** (buse + plateau à 0 °C) en un appui
- 🌡️ Réglage du **plateau −5 °C** (symétrie avec buse ±5 et plateau +5)
- 📷 **Découverte automatique de la caméra** via `GET /api/v1/cameras` quand `/api/v1/status` n'expose pas de bloc `camera`
- 🕹️ Le pas par défaut du Jog est désormais repris depuis `jog_step_mm` de `printer.cfg`

### 🐛 Corrections

- 🛑 **Webcam : plus de tempête de requêtes**. En cas d'échec (pas de caméra, snap KO, décodage JPEG raté), l'horodatage n'était pas mis à jour : `refresh()` était rappelé à chaque frame (~30 FPS), enchaînant des requêtes HTTP bloquantes qui gelaient l'UI et matraquaient l'imprimante. L'essai est maintenant horodaté dès le début → throttle 5 s respecté même en échec.
- 🧹 **Lint CI propre** : suppression des avertissements `cppcheck` (`--enable=warning,style --error-exitcode=1`) qui faisaient échouer le job lint (variables `const`, condition morte, ternaire dupliqué).
- 🩹 `prusa_api.h` inclut `<stddef.h>` (utilisé pour `size_t`).

## [0.1.0] - 2026-05-14

### 🎉 Initial release

- 🖨️ Client Prusa-Link v1 complet : status, job (pause/resume/cancel), jog XYZ, home, températures buse + plateau, webcam snapshot
- 🎮 UI SDL2 5 écrans : Dashboard / Webcam / Controls / Jog / Settings
- 🔑 Auth `X-Api-Key` lue depuis `App/PrintBoy/printer.cfg`
- 🚪 Bouton Déconnexion qui efface URL + clé API et resave
- 📶 Détection WiFi off au lancement (avertissement)
- 🎨 Icône générée par script Python stdlib (`scripts/make-icon.py`)
- 🖼️ 5 mockups d'écran générés (`scripts/make-mockups.py`)
- 🐳 Build via Docker `aemiii91/miyoomini-toolchain` (CI GitHub Actions)
- 📚 Documentation : README (emoji-rich), INSTALL, BUILD, SECURITY, NOTICE
- 🔒 Compilé avec `-Wall -Wextra -Wformat-security -fstack-protector-strong`
- ⏱️ Timeouts libcurl agressifs (5s connect, 10s total) pour ne pas geler l'UI
- 💾 Sauvegarde atomique de la config (temp + rename)

### Connu

- 🟡 Binaire ARM à compiler (la CI le fait à chaque tag `v*`)
- 🟡 Pas de saisie clavier dans l'app pour l'URL/clé : il faut éditer `printer.cfg` depuis le PC
- 🟡 Pas de support multi-imprimante (1 seule à la fois)
- 🟡 Pas de support HTTPS avec certificat self-signed Prusa-Link
- 🟡 Tests sur console réelle non encore effectués
