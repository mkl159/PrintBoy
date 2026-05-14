# 📝 Changelog

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
