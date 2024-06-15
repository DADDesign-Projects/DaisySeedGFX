//------------------------------------------------------------------------
// Copyright(c) 2024 Dad Design.
//      Définir ici les paramètres de votre écran et de votre interface SPI
//------------------------------------------------------------------------
#pragma once

// Hauteur et largeur de l'écran en pixel
#define TFT_WIDTH       128
#define TFT_HEIGHT      160

// Type de contôleur
//#define TFT_CONTROLEUR_TFT  7789
#define TFT_CONTROLEUR_TFT  7735

// Pour info codage des couleurs
// 18 Codage 18 bits RGB666
// 16 Codage 16 bits RGB565

//#define TFT_COLOR 18
#define TFT_COLOR 16

// Configuration du SPI
#define TFT_SPI_PORT SPI_1
#define TFT_SPI_MODE Mode0
#define TFT_SPI_BaudPrescaler PS_4


// Configuration des GPIO utilisés par le SPI
#define TFT_MOSI D10
#define TFT_SCLK D8
#define TFT_DC   D15
#define TFT_RST  D16

// Définition de la taille des blocs utilisés pour une mise à jour partielle de l'écran
//   -> On me transmet à l'écran que les blocs qui ont été modifiés
// Ex : ecran 240x320 / FRAME_GRILLE 10 -> Bloc = 24x32
// Attention la hauteur et la largeur  doivent être divisibles par FRAME_GRILLE
#define FRAME_GRILLE 8

// Taille du FIFO pour la transmission SPI des blocs par le DMA
// Attention nombre limité car le FIFO est placé dans la SRAM D1 qui semble être utilisée aussi par un autre process
#define SIZE_FIFO 10