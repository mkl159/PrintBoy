# 🔒 Politique de sécurité — PrintBoy

## 🎯 Modèle de menace

PrintBoy s'exécute :
- En **root** sur la console Miyoo Mini Plus (firmware Onion)
- Sur le **réseau WiFi local** uniquement (le binaire ne contacte pas Internet — il ne parle qu'à Prusa-Link)
- À destination d'**adultes/makers** (gère une imprimante 3D)

### Surfaces d'attaque

| Vecteur | Risque | Mitigation |
|---|---|---|
| 🌐 Trafic HTTP en clair sur WiFi domestique | Faible (LAN privé) | Documenter : ne pas l'utiliser sur WiFi public |
| 📡 MITM sur WiFi public | Élevé si Prusa expose HTTP | Privilégier HTTPS si Prusa-Link le supporte |
| 🔑 Clé API dans `printer.cfg` (clair) | Modéré (accès physique à la SD) | `.gitignore` exclut le fichier, et bouton Déconnexion l'efface |
| 🧨 Commande malveillante (jog, target) | Faible (Prusa-Link valide) | L'API rejette les températures > limite firmware |
| 🌡️ Définir température extrême | Possible mais limité par firmware | Prusa-Link impose `0 ≤ buse ≤ 300 °C`, `0 ≤ plateau ≤ 120 °C` |
| 📋 Buffer overflow dans le parseur JSON | Faible (cJSON est mature) | Compilé avec `-fstack-protector-strong` |

## 🚨 Vulnérabilités connues

### Héritées

| Composant | Source | Impact sur PrintBoy |
|---|---|---|
| **libcurl** | Sans CVE actif à la date du repo, mais vérifier régulièrement | Toute CVE libcurl impacte directement |
| **cJSON** | Historique de buffer overflows (< 1.7.15) | Pin sur version ≥ 1.7.15 |
| **SDL2_image / libjpeg** | Plusieurs CVE historiques sur JPEG malformé | Une caméra Prusa compromise pourrait pousser un JPEG malicieux |

### Spécifiques PrintBoy

- 🟡 **Pas de validation TLS** : libcurl est appelée avec ses défauts (vérification CA système). Si Prusa-Link expose un certificat self-signed, l'utilisateur doit l'ajouter au store système ou ça échoue. **On ne désactive PAS `CURLOPT_SSL_VERIFYPEER`**.
- 🟡 **L'image JPEG webcam est entièrement chargée en mémoire** (jusqu'à 4 MiB). Sur Miyoo (128 MiB RAM) c'est ok, mais une caméra compromise pourrait DoS via taille (limité par `MAX_BODY_BYTES` dans `http.c`).
- ✅ **Redirections HTTP bornées** (depuis v0.3.0) : `MAXREDIRS=3` et protocoles restreints à `http`/`https` — un serveur compromis ne peut pas rediriger le client vers `file://` ou un autre schéma.
- ✅ **`CURLOPT_NOSIGNAL`** activé (depuis v0.3.0) : requis pour le polling threadé, évite que `SIGALRM` (timeout DNS) tue le process.

## 📢 Signaler une vuln

Pour les rapports de sécurité, NE PAS ouvrir d'issue publique.
Ouvrir un *Security advisory* via l'onglet **Security** du repo GitHub.

Réponse attendue sous **7 jours**.

## 🛡️ Bonnes pratiques

- 🔍 Vérifie le SHA-256 du zip avant install (commande dans INSTALL.md)
- 🔐 Ne partage pas ta clé API
- 🚫 N'utilise pas PrintBoy sur un WiFi public sans VPN
- 🆕 Mets à jour le firmware Prusa et Prusa-Link régulièrement
- 🔒 Active un mot de passe sur ton routeur WiFi
- 🚪 Utilise le bouton **Déconnexion** dans Settings si tu prêtes ta SD

## 📋 Checklist pré-release publique

- [ ] Pin de la toolchain Docker par SHA-256 (`BUILD.md`)
- [ ] Signature GPG/cosign des releases
- [ ] CI lint propre (shellcheck, pyflakes, clang-format)
- [ ] Test avec une caméra Prusa-Link réelle (JPEG malformé)
- [ ] Test de récupération après perte WiFi (timeout libcurl 10s OK)
- [ ] Test de déconnexion → reconnexion via Settings
