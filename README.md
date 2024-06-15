# DaisySeedGFX
TFT graphical display library for Daisy Seed. ST7789 and ST7735 driver

## Author
DAD Design

## Présentation
DaisySeedGFX est une bibliothèque graphique pour la plateforme Daisy Seed de Electrosmith.
La bibliothèque prend en charge pour le moment uniquement les contrôleurs ST7735 et ST7789. A priori la bibliothèque doit pouvoir être adaptée à n’importe quel contrôleur assez facilement.

La bibliothèque utilise un frame buffer intermédiaire ente les primitives graphiques et le contrôleur. Les modifications réalisées dans le frame buffer sont transmises au contrôleur utilisant les transferts SPI sous DMA. Afin de réduire les transferts le frame buffer est divisé en blocs -> seuls les blocs modifiés sont transférés vers l’écran.

Les primitives graphiques sont minimalistes, elles pourront être complétées en fonction des besoins.

## Exemples
Des exemples d'implemantation sont données dans les repository :
https://github.com/DADDesign-Projects/DEMO_DaisyGFX_ST7735
https://github.com/DADDesign-Projects/DEMO_DaisyGFX_ST7789

## Développement
Le code est modifiable et compilable sous VS Code dans l’environnement de développement du Daisy Seed (voir le https://github.com/electro-smith).

## Configurations
Le fichier ../UserConfig.h permet d’adapter la bibliothèque aux différentes implémentations matérielles.

Le fichiers Makefile c_cpp_properties.json sont à adapter en fonction de l’emplacement des librairies libDaisy et DaisySP: 
 Makefile : 
  LIBDAISY_DIR = ../../../Desktop/DaisyExamples/libDaisy/
  DAISYSP_DIR = ../../../Desktop/DaisyExamples/DaisySP/

.vscode/c_cpp_properties.json :
"includePath": [
  "${workspaceFolder}/**",
  "${workspaceFolder}/../../../Desktop/DaisyExamples/libDaisy//**",
  "${workspaceFolder}/../../../Desktop/DaisyExamples/DaisySP//**"

Pour créer des fonts utilisez l’outil https://rop.nl/truetype2gfx/. Chaque font est enregistrée un fichier xxx.h.
