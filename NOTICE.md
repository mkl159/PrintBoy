# 📜 Attributions

PrintBoy s'appuie sur ces composants :

| Composant            | Source / Upstream                                                       | Licence | Usage |
|----------------------|-------------------------------------------------------------------------|---------|-------|
| Format App Onion     | [OnionUI/Onion](https://github.com/OnionUI/Onion)                       | MIT     | Schéma `App/<name>/config.json + launch.sh` |
| API Prusa-Link       | [prusa3d/Prusa-Link](https://github.com/prusa3d/Prusa-Link)             | GPL-3.0 | Documentation REST utilisée (pas de code repris) |
| SDL2                 | https://www.libsdl.org/                                                 | Zlib    | Rendu graphique (lié dynamiquement) |
| SDL2_ttf             | https://github.com/libsdl-org/SDL_ttf                                   | Zlib    | Rendu texte |
| SDL2_image           | https://github.com/libsdl-org/SDL_image                                 | Zlib    | Décodage JPEG (webcam) |
| libcurl              | https://curl.se/                                                        | curl    | HTTP/HTTPS client |
| cJSON                | https://github.com/DaveGamble/cJSON                                     | MIT     | Parsing JSON |
| DejaVu Sans          | https://dejavu-fonts.github.io/                                         | Bitstream Vera | Police par défaut |

PrintBoy contient du code original (C, environ 1500 lignes) sous MIT et
n'inclut **aucune partie du code Prusa-Link** ou OnionOS : il consomme
seulement leurs API/formats publics.

## Marques

"Prusa", "Original Prusa", "Prusa Mini" sont des marques de Prusa Research a.s.
PrintBoy n'est pas affilié à Prusa Research.

"Miyoo Mini" est une marque de Miyoo. PrintBoy n'est pas affilié à Miyoo.
