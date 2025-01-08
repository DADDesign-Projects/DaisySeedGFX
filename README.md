# Notice: 
**A new version of the library is now available at** https://github.com/DADDesign-Projects/DaisySeedGFX2.

This new version introduces layer management and other enhancements.

The current version is now obsolete and will no longer be updated.

# DaisySeedGFX
TFT graphical display library for Daisy Seed. ST7789 and ST7735 driver

## Author
DAD Design

## Présentation
DaisySeedGFX est une bibliothèque graphique pour la plateforme Daisy Seed de Electrosmith.
La bibliothèque prend en charge pour le moment uniquement les contrôleurs ST7735 et ST7789. A priori la bibliothèque doit pouvoir être adaptée à n’importe quel contrôleur assez facilement.

La bibliothèque utilise un frame buffer intermédiaire ente les primitives graphiques et le contrôleur. Les modifications réalisées dans le frame buffer sont transmises au contrôleur utilisant les transferts SPI sous DMA. Afin de réduire les transferts le frame buffer est divisé en blocs -> seuls les blocs modifiés sont transférés vers l’écran.

Les primitives graphiques sont minimalistes, elles pourront être complétées en fonction des besoins.


## Mise en oeuvre
Le code est modifiable et compilable sous VS Code dans l’environnement de développement du Daisy Seed (voir le https://github.com/electro-smith).

### Configuration
1. Creez un projet avec l'outil helper.py (ou autre...).
2. Clonez la librairie dans le dossier DaisySeedGFX à l'interieur de votre dossier projet.
3. Editez le fichier Makefile et ajoutez DaisySeedGFX/Frame.cpp DaisySeedGFX/GFX.cpp DaisySeedGFX/TFT_SPI.cpp dans la ligne CPP_SOURCES.
4. Copiez le fichier UserConfig.h dans voire dossier projet et configurez le en fonction de votre écran et des pins utilisées. 

### Fonts
Pour créer des fonts utilisez l’outil https://rop.nl/truetype2gfx/. Chaque font est enregistrée un fichier xxx.h.

### Exemples
Des exemples d'implemantation sont données dans les repository :
1. https://github.com/DADDesign-Projects/DEMO_DaisyGFX_ST7735
2. https://github.com/DADDesign-Projects/DEMO_DaisyGFX_ST7789
