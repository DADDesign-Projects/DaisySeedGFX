//------------------------------------------------------------------------
// Copyright(c) 2024 Dad Design.
//      Bibliothèque graphique
//
// Inspiré largement de :
//    Adafruit-GFX-Library : https://github.com/adafruit/Adafruit-GFX-Library
//    eSPI : https://github.com/Bodmer/TFT_eSPI
//------------------------------------------------------------------------
#pragma once
#include "Frame.h"
#define PROGMEM

constexpr float __PI = 3.14159265358979;
constexpr float __PI_2 = 1.57079632679489;

//***********************************************************************************
// CImage
// Gestion d'une image
//
enum class TypeImage{
    R8G8B8,
    B8G8R8,
    R8G8B8A8,
    B8G8R8A8
};

// cImage
class cImage {
public:
    // Constructeur
    cImage(uint16_t With, uint16_t Height, TypeImage Type, const uint8_t* pImage){
        m_With = With,
        m_Height = Height;
        m_Type = Type;
        m_pImage = pImage;
    }
    // Lecture de la largeur de l'image
    inline uint16_t getWith(){
        return m_With;
    } 
    // Lecture de la hauteur de l'image
    inline uint16_t getHeight(){
        return m_Height;
    } 
    // Lecture de l'adresse du premier pixel de la la ligne spécifiée
    inline const uint8_t* GetPtrLine(uint16_t Line){
        switch(m_Type){
            case TypeImage::R8G8B8 :
            case TypeImage::B8G8R8 :
                return m_pImage+(m_With*(Line)*3); 
            case TypeImage::R8G8B8A8 :
            case TypeImage::B8G8R8A8 :
                return m_pImage+(m_With*(Line)*4); 
        }
        return nullptr;
    }
    // Lecteur de la couleur du pixel
    inline cColor getColor(const uint8_t* pImage){
        switch(m_Type){
            case TypeImage::R8G8B8 :
                return cColor((*(pImage)),(*(pImage+1)),(*(pImage+2)));
            case TypeImage::B8G8R8 :
                return cColor((*(pImage+2)),(*(pImage+1)),(*(pImage)));
            case TypeImage::R8G8B8A8 :
                return cColor((*(pImage)),(*(pImage+1)),(*(pImage+2)),(*(pImage+3))); 
            case TypeImage::B8G8R8A8 :
                return cColor((*(pImage+2)),(*(pImage+1)),(*(pImage)),(*(pImage+3))); 
        }
        return cColor(0,0,0);
    }
    // Lecture de la taile d'un pixel
    inline uint8_t getPixelSize(){
        switch(m_Type){
            case TypeImage::R8G8B8 :
            case TypeImage::B8G8R8 :
                return 3;
            case TypeImage::R8G8B8A8 :
            case TypeImage::B8G8R8A8 :
                return 4;
        }
        return 0;        
    }
protected :
    uint16_t m_With;
    uint16_t m_Height;
    TypeImage m_Type;
    const uint8_t* m_pImage;
};

//***********************************************************************************
// CFont
// Gestion des polices de caratères
// Utilisation de la structuration des fonts de Adafruit-GFX-Library
// Ce qui permet de profiter des outils de conversion (ex: https://rop.nl/truetype2gfx/)

// Table ds descripteurs de caratères
typedef struct
{
    uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
    uint8_t width;         ///< Bitmap dimensions in pixels
    uint8_t height;        ///< Bitmap dimensions in pixels
    uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
    int8_t xOffset;        ///< X dist from cursor pos to UL corner
    int8_t yOffset;         ///< Y dist from cursor pos to UL corner
} GFXglyph;

// Descripteur de la Font
typedef struct
{
    uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;  ///< Glyph array
    uint16_t first;   ///< ASCII extents (first char)
    uint16_t last;    ///< ASCII extents (last char)
    uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;

// cFont
class cFont
{
public:
    // --------------------------------------------------------------------------
    // Constructeur
    cFont(const GFXfont *pFont);

    // --------------------------------------------------------------------------
    // Lecture le la largueur du caratère c
    uint8_t getCharWidth(char c)
    {
        return ((m_pFont->glyph) + (c - m_pFont->first))->xAdvance;
    }

    // --------------------------------------------------------------------------
    // Lecture le la largueur de la chaine de caratère.
    uint16_t getTextWidth(const char *Text)
    {
        const char *pText = Text;
        uint16_t result = 0;
        while (*pText != '\0')
        {
            result += getCharWidth(*pText++);
        }
        return result;
    }

    // --------------------------------------------------------------------------
    // Lecture de la hauteur max de la font
    inline uint8_t getHeight()
    {
        return m_NegHeight - m_PosHeight;
    }

    // --------------------------------------------------------------------------
    // Lecture de la hauteur max de la font au dessus la ligne du curseur
    inline uint8_t getPosHeight()
    {
        return -m_PosHeight;
    }

    // --------------------------------------------------------------------------
    // Lecture de la hauteur max de la font sous la ligne du curseur
    inline uint8_t getNegHeight()
    {
        return m_NegHeight;
    }

    // --------------------------------------------------------------------------
    // Lecture de l'adresse du descripteur de font
    inline const GFXfont *getGFXfont() { return m_pFont; }

    // --------------------------------------------------------------------------
    // Lecture de l'adresse de la table des decripteurs de caratères
    inline const GFXglyph *getGFXglyph() { return m_pTable; }

    // --------------------------------------------------------------------------
    // Lecture de l'adresse du descripteur du caratère c
    inline const GFXglyph *getGFXglyph(char c)
    {
        return m_pTable + (c - m_pFont->first);
    }

    // --------------------------------------------------------------------------
    // Lecteur de l'adresse du bitmap représentant c
    inline const uint8_t *getBitmap(char c)
    {
        return &m_pFont->bitmap[m_pTable[c - m_pFont->first].bitmapOffset];
    }

    // --------------------------------------------------------------------------
    // Données de la classe
protected:
    const GFXfont *m_pFont; // Descripteur de la font
    GFXglyph *m_pTable;     // Table des descripteurs de caratères

    int8_t m_PosHeight; // Hauteur au dessus de la ligne du curseur
    int8_t m_NegHeight; // Hauteur sous la ligne du curseur
};

//***********************************************************************************
// cGFX
//   Bibliothèque Graphique
class cGFX : protected cRBG_Frame
{
public:
    // --------------------------------------------------------------------------
    // Constructeur
    cGFX() {}

    // --------------------------------------------------------------------------
    // Initialisation de la classe
    // Doit être appelée avant toute utilisation (aucune vérification réalisée) 
    //   pFrameBuff pointe sur la mémoire de frame à instancier de préférence dans la SDRAM  
    //   pFIFO_Data pointe sur la mémoire du FIFO DMA utilisé pour les transfers SPI 
    //              doit etre obligatoirement instancié dans la SDRAM D1 (DMA_BUFFER_MEM_SECTION)
    //   Width, Height taille de l'écran
    //
    void Init(RGB *pFrameBuff, FIFO_Data *pFIFO_Data, uint16_t Width, uint16_t Height)
    {
        setFrame(pFrameBuff, pFIFO_Data, Width, Height);
    }
    
    // --------------------------------------------------------------------------
    // Changer l'orientation de l'écran
    void setRotation(Rotation r)
    {
        setFrameRotation(r);
    }
    
    // --------------------------------------------------------------------------
    // Transmette les modifications de la frame à l'écran
    inline void FlushFrame() { cRBG_Frame::FlushFrame();}

    // ==========================================================================
    // Dessiner des formes
    // ==========================================================================
    // Tracer un rectange vide 
    void drawRect(uint16_t x, uint16_t y, int16_t dx, int16_t dy, cColor Color);
    // Tracer un rectangle plein
    void drawFillRect(uint16_t x, uint16_t y, int16_t dx, int16_t dy, cColor Color);
    // Trace une ligne
    void drawLine(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, cColor Color);
    // Tracer un cercle vide
    void drawCircle(uint16_t centerX, uint16_t centerY, uint16_t radius, cColor Color);
    // Tracer un arc de cercle vide
    void drawArc(uint16_t centerX, uint16_t centerY, uint16_t radius, uint16_t AlphaIn, uint16_t AlphaOut, cColor Color);
    // Tracer un cercle plein
    void drawFillCircle(uint16_t centerX, uint16_t centerY, uint16_t radius, cColor Color);
    // Tracer une image 8bits par couleurs (depreciated)
    void drawR8G8B8Image(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, const uint8_t *pImg);
    // Tracer une image
    void drawImage(uint16_t x, uint16_t y, cImage &Image);

    // ==========================================================================
    // Dessiner du texte
    // ==========================================================================
    // Positionnement du cuseur
    inline void setCursor(uint16_t x, uint16_t y){
        m_xCursor = x;
        m_yCursor = y;
    };
    
    // Configuration de la fonte
    inline void setFont(cFont *pFont){ m_pFont = pFont; };

    // Configuration de la couleur du texte
    inline void setTextFrontColor(cColor Color) { m_TextFrontColor = Color;}
    
    // Configuration de la couleur de l'arrière plan du texte
    inline void setTextBackColor(cColor Color) { m_TextBackColor = Color; }
    
    // Dessiner le caractère c
    void drawChar(const char c, bool Erase = false);
    
    // Dessiner le texte sans couleur d'arriere plan
    void drawTransText(const char *Text, bool Erase = false);
    
    // Dessiner le texte
    void drawText(const char *Text, bool Erase = false);

    // Lire la position du curseur en X
    inline uint8_t getXCursor() { return m_xCursor; }

    // Lire la position du curseur en Y
    inline uint8_t getYCursor() { return m_xCursor; }

    // Lire la fonte courante
    inline cFont *getFont() { return m_pFont; }
    
    // Lire la hauteur de la frame
    inline uint16_t getWidth() { return cRBG_Frame::getWidth(); }
    
    // Lire la largeur de la frame
    inline uint16_t getHeight() { return cRBG_Frame::getHeight(); }

    // Lecture le la largueur de la chaine de caratère.
    inline uint16_t getTextWidth(const char *Text){return m_pFont->getTextWidth(Text);}

    // Lecture le la hauteur de la chaine de caratère.
    inline uint8_t getTextHeight(){return m_pFont->getHeight();}

    // --------------------------------------------------------------------------
    // Données de la classe
 protected:
    uint16_t m_xCursor = 0;
    uint16_t m_yCursor = 0;
    cFont *m_pFont = nullptr;
    cColor m_TextFrontColor = cColor(255, 255, 255);
    cColor m_TextBackColor = cColor(0, 0, 0);
};