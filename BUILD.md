# 🔨 Compilation depuis les sources

> Pour la plupart des utilisateurs : prends la release pré-compilée.
> Cette doc est pour les développeurs.

## Prérequis

- 🐧 Linux ou macOS (Windows : WSL2)
- 🐳 Docker
- 📦 Git
- ~3 Go d'espace disque

## Dépendances dans la toolchain

Le binaire a besoin de :

- 📚 **libcurl** (HTTP/HTTPS)
- 🧩 **cJSON** (parsing)
- 🎨 **SDL2 + SDL2_ttf + SDL2_image** (UI + JPEG)

L'image `aemiii91/miyoomini-toolchain:latest` les contient déjà. Sinon, à ajouter via `apt-get install` dans un Dockerfile dérivé.

## Build classique

```bash
git clone <ce-repo> PrintBoy
cd PrintBoy
make with-toolchain
```

Le binaire apparaît dans `build/printboy`. Copie-le dans `App/PrintBoy/bin/printboy` puis donne le bit `+x`.

## Build pour développement (host x86)

Pour tester rapidement sur un PC :

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libcurl4-openssl-dev libcjson-dev
make
./build/printboy
```

Avec un printer.cfg local à `./printer.cfg`. Note : SDL2 utilisera ton serveur X/Wayland normalement.

## Build reproductible (recommandé pour les releases)

Pin la toolchain par digest SHA-256 :

```bash
docker pull aemiii91/miyoomini-toolchain:latest
docker inspect --format='{{index .RepoDigests 0}}' aemiii91/miyoomini-toolchain:latest
# -> aemiii91/miyoomini-toolchain@sha256:xxxxx
```

Édite `Makefile` :

```makefile
DOCKER_IMAGE := aemiii91/miyoomini-toolchain@sha256:xxxxx
```

## CI GitHub Actions

Voir `.github/workflows/build.yml`. Trigger sur :
- `push` d'un tag `v*` → build + zip + SHA-256 + GitHub Release
- `workflow_dispatch` manuel

## Vérifications avant release

- [ ] `make clean && make with-toolchain` réussit
- [ ] `file build/printboy` indique bien `ARM, EABI5`
- [ ] `ldd build/printboy` (dans le Docker) montre les libs attendues
- [ ] Test sur Miyoo réel avec une vraie Prusa accessible
- [ ] SHA-256 publié dans la description de release
- [ ] Tag annoté + signé GPG (idéalement)

## Notes de portabilité

- 🐧 Le firmware Miyoo est en `arm-linux-gnueabihf` (32-bit hard-float).
- 📚 Les libs dynamiques manquantes sont à mettre dans `App/PrintBoy/lib/`.
- 🎬 Pour la webcam : Prusa-Link renvoie du JPEG, donc `SDL2_image` avec support `libjpeg` est requis.
