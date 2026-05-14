# 🚀 Installation de PrintBoy

> ⏱️ ~5 minutes. Pas de reformatage de SD, pas de re-flash. WiFi obligatoire.

## 1. ✅ Vérifications

- 📱 Tu as bien une Miyoo Mini **Plus** (avec WiFi) — pas la version standard
- 🧅 OnionOS v4.3.x installé et fonctionnel
- 🌐 WiFi configuré dans Onion : **Menu → Settings → WiFi → ON**
- 🖨️ Prusa avec Prusa-Link activé sur le **même réseau** que la Miyoo
- 🔑 Note ta clé API : **Imprimante → Settings → Network → Prusa Link → API key**

## 2. 📥 Récupère le ZIP

### Option A — Release pré-compilée (recommandé)

1. Va dans l'onglet **Releases** du repo
2. Télécharge `PrintBoy_vX.Y.Z.zip`
3. Vérifie le SHA-256 (publié à côté du zip) :
   ```
   certutil -hashfile PrintBoy_v0.1.0.zip SHA256   # Windows
   shasum -a 256 PrintBoy_v0.1.0.zip               # macOS/Linux
   ```
4. Décompresse sur ton bureau

### Option B — Build local

Voir [BUILD.md](BUILD.md). Docker requis.

## 3. 🎨 Génère les icônes si pas dans le zip

```bash
python scripts/make-icon.py
```

Stdlib Python uniquement, pas besoin de Pillow.

## 4. 📂 Copie sur la SD

Branche la SD du Miyoo sur le PC (par exemple `E:\`). Le zip décompressé contient :

```
PrintBoy_vX.Y.Z/
├── App/PrintBoy/            <-- vers E:\App\PrintBoy\
├── Icons/Default/app/       <-- vers E:\Icons\Default\app\
├── scripts/
├── preview/
└── (doc + audits)
```

À copier :

1. `App\PrintBoy\` → `E:\App\PrintBoy\` (fusionne, ne touche pas aux autres apps)
2. `Icons\Default\app\printboy.png` → `E:\Icons\Default\app\printboy.png`

## 5. ⚙️ Configure ton imprimante

Édite `E:\App\PrintBoy\printer.cfg` :

```ini
# PrintBoy - config Prusa-Link
url = http://192.168.1.42         # remplace par l'IP de ta Prusa
api_key = ZZZ-collé-depuis-l'imprimante
poll_interval_s = 2
jog_step_mm = 10
```

🔑 **Comment trouver l'IP de la Prusa ?**
- Sur la MK4 / Mini / XL : **Settings → Network → IP**
- Sinon : `192.168.1.???` ou utilise un scanner réseau (Fing, nmap, le routeur)

🔑 **Comment trouver la clé API ?**
- **Settings → Network → Prusa Link → API key**

⚠️ Garde cette clé pour toi : elle donne le contrôle total de ton imprimante.

## 6. ⏏️ Éjecter, brancher, lancer

1. **Éjecte proprement** la SD (clic droit → Éjecter sur Windows)
2. Insère dans le Miyoo, allume
3. Sur Onion : **Apps → PrintBoy → A**
4. 🎉 Tu dois voir le Dashboard avec l'état de l'imprimante

## 🐛 Résolution de problèmes

| Symptôme | Cause | Solution |
|---|---|---|
| 🚫 "PAS DE CONNEXION" sur le Dashboard | WiFi off, mauvais URL ou clé | Onglet Settings → "Tester (A)" — vérifie le message |
| 🌫️ Onion ne voit pas l'icône | `config.json` mal placé | Vérifie `E:\App\PrintBoy\config.json` |
| 💥 Crash au lancement / écran noir | Binaire `printboy` absent | Voir [BUILD.md](BUILD.md), copie `printboy` dans `App\PrintBoy\bin\` |
| 📷 Webcam : "aucune caméra détectée" | Pas de cam connectée à Prusa-Link | Normal sur MK3 sans Pi, ou Mini sans add-on |
| 🔥 "Erreur de pause" | Pas de job en cours | Normal si l'imprimante est IDLE |
| 🐌 Réponses lentes | WiFi 2.4 GHz faible | Augmente `poll_interval_s` à 5 ou 10 dans Settings |

📋 Logs disponibles dans `E:\Saves\PrintBoy\printboy.log` (visible une fois la SD rebranchée au PC).
