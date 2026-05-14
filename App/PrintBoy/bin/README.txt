Ce dossier recevra le binaire 'printboy' compile pour Miyoo Mini Plus.

Le binaire n'est PAS commit dans le repo (cf .gitignore). Il est genere :
  - par la CI Github Actions (.github/workflows/build.yml) a chaque tag v*
  - ou localement via 'make with-toolchain' (cf BUILD.md)

Format attendu :
  Nom        : printboy
  Taille     : ~200-400 KB
  Arch       : ARM EABI5 32-bit (armv7-a hard-float)
  Libs liees : libcurl, libSDL2, libSDL2_ttf, libSDL2_image, libcjson

Apres copie : chmod +x printboy
