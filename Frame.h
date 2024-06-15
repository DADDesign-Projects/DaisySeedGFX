//------------------------------------------------------------------------
// Copyright(c) 2024 Dad Design.
//      Gestion d'une frame de pixel
//      Les pixels sont organisés en bloc de transmission
//      pour une mise à jour optimisé de l'écran
//------------------------------------------------------------------------
#pragma once
#include <stdint.h>
#include "TFT_SPI.h"
#include "Debug.h"

#define NB_BLOC (FRAME_GRILLE*FRAME_GRILLE)
#if TFT_COLOR == 16
    #define TAILLE_BLOC (TFT_WIDTH * TFT_HEIGHT * 2) / NB_BLOC
#else
    #define TAILLE_BLOC (TFT_WIDTH * TFT_HEIGHT * 3) / NB_BLOC
#endif

//***********************************************************************************
// Cmd_CASET
//   Commande SPI de séléction des colones  
//*********************************************************************************** 
class Cmd_CASET {
    public :
    friend class cRBG_Frame;
    // --------------------------------------------------------------------------
    // Constructeur
    Cmd_CASET(){
        m_Commande = TFT_CASET;
    }

    // --------------------------------------------------------------------------
    // Définition de l'abcisse de la fenetre
    void setData(uint16_t x, uint16_t dx){
        m_Data[0] = x >> 8;
        m_Data[1] = x & 0xFF;
        m_Data[2] = dx >> 8;
        m_Data[3] = dx & 0xFF;
    }
    
    // --------------------------------------------------------------------------
    // Données de la classe
    protected :
    uint8_t m_Commande;
    uint8_t m_Data[4];
};

//***********************************************************************************
// Cmd_RASET
//   Commande SPI de selection des rangées  
//*********************************************************************************** 
class Cmd_RASET {
    public :
    friend class cRBG_Frame;

    // --------------------------------------------------------------------------
    // Constructeur
    Cmd_RASET(){
        m_Commande = TFT_RASET;
    }
    
    // --------------------------------------------------------------------------
    // Définition de l'ordonnée de la fenètre
    void setData(uint16_t y, uint16_t dy){
        m_Data[0] = y >> 8;
        m_Data[1] = y & 0xFF;
        m_Data[2] = dy >> 8;
        m_Data[3] = dy & 0xFF;
    }

    // --------------------------------------------------------------------------
    // Données de la classe
    protected :
    uint8_t m_Commande;
    uint8_t m_Data[4];
};

//***********************************************************************************
// Cmd_RAMWR
//   Commande SPI d'ecriture des pixels 
//***********************************************************************************
class cRBG_Frame;
class Cmd_RAMWR {
    public :
    friend class cRBG_Frame;

    // --------------------------------------------------------------------------
    // Constructeur
    Cmd_RAMWR(){
        m_Commande = TFT_RAMWR;
    }

    // --------------------------------------------------------------------------
    // Définition des pixels de la frame à transférer
    void setData(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, cRBG_Frame *pFrame);

    // --------------------------------------------------------------------------
    // Données de la classe
    protected :
    uint8_t m_Commande;
    uint8_t m_Data[TAILLE_BLOC];
};

//***********************************************************************************
// cColor 
//*********************************************************************************** 
struct cColor { 
    // Constructeur
    cColor(uint8_t R, uint8_t G, uint8_t B, uint8_t Trans=255){
        m_R = R;
        m_G = G;
        m_B = B;
        m_Trans = Trans;
    }

    uint8_t m_R;
    uint8_t m_G;
    uint8_t m_B;
    uint8_t m_Trans;
};

//***********************************************************************************
// RGB
// Définition d'un pixel RGB 
//*********************************************************************************** 
struct RGB {
    friend class cRBG_Frame;
	// --------------------------------------------------------------------------
	// Mise à jour d'un pixel et indication que le bloc a changé d'état
    void inline set(cColor Color){
        R = Color.m_R;
        G = Color.m_G;
        B = Color.m_B;
        *m_pChange = true;
    }

    // --------------------------------------------------------------------------
	// Lecture de la composante Rouge 
    uint8_t inline getR(){
        return R;
    }
    // --------------------------------------------------------------------------
	// Lecture de la composante Verte 
    uint8_t inline getG(){
        return G;
    }

    // --------------------------------------------------------------------------
	// Lecture de la composante Bleue 
    uint8_t inline getB(){
        return B;
    }

    // --------------------------------------------------------------------------
    // Données de la classe
    protected :
    uint8_t R;          // Composante Rouge
    uint8_t G;          // Composante Verte
    uint8_t B;          // Composante Bleue
    bool *m_pChange;    // Indicateur de changement d'état (pointe sur un tableau de bloc) 
};

//***********************************************************************************
// cRBG_Frame
//  Gestion de la framme
//*********************************************************************************** 

//   Pour le fonctionnement du DMA cette structure doit être instanciée dans la
//   mémoire DMA_BUFFER_MEM_SECTION
struct FIFO_Data {
    Cmd_CASET m_CmdCASET[SIZE_FIFO];
    Cmd_RASET m_CmdRASET[SIZE_FIFO];
    Cmd_RAMWR m_CmdRAWWR[SIZE_FIFO];
};

// ================================
// cRBG_Frame
class cRBG_Frame  : protected TFT_SPI {
    public:
    // --------------------------------------------------------------------------
    // Constructeur 
    // FIFO_Data doit être instanciée dans la mémoire DMA_BUFFER_MEM_SECTION
    cRBG_Frame(){
        m_FIFO_in = 0;
        m_FIFO_out = 0;
        m_FIFO_NbElements = 0;
        m_Busy = false;
    }

    // ==========================================================================
    // Configuration

    // --------------------------------------------------------------------------
    // Initialisation de la classe.
    // Doit être appelée avant toute utilisation (aucune vérification réalisée) 
    void setFrame(RGB *pFrameBuff,  FIFO_Data *pFIFO_Data, uint16_t Width,  uint16_t Height);
    
    // --------------------------------------------------------------------------
    // Configuration l'orientation de la frame et de l'écran
    void setFrameRotation (Rotation r);

    // ==========================================================================
    // Lecture / Ecriture

    // --------------------------------------------------------------------------
    // Retourne l'adresse d'un pixel à partir de ses coordonnées         
    inline RGB *getPtr(int16_t x, int16_t y){
        if(x<0) x=0;
        if(y<0) y=0;
        if(x>=m_Width) x = m_Width-1;
        if(y>=m_Height) y = m_Height-1;
        return &m_pFrame[x+(y*m_Width)];
    } 

    // --------------------------------------------------------------------------
    // Ecriture d'un pixel 
    inline void setPixel(int16_t x, int16_t y, cColor Color){
        if((x<m_Width) && (y<m_Height)){
            m_pFrame[x+(y*m_Width)].set(Color);
        }
    }
    
    // ==========================================================================
    // Hauteur / Largeur
    // --------------------------------------------------------------------------
    // Lecture de la largeur de la frame
    inline uint16_t getWidth() {
        return m_Width ;
    }

    // --------------------------------------------------------------------------
    // Lecture de la hauteur de la frame
    inline uint16_t getHeight() {
        return m_Height ;
    }

    // --------------------------------------------------------------------------
    // Transmission des modifications de la frame vers l'écran    
    void FlushFrame();
    
    // ==========================================================================
    // Gestion des blocs de transmission

    
    protected:    
    // --------------------------------------------------------------------------
    // Test si le FIFO est plein
    inline bool isFull(){
        // protéger m_FIFO_NbElements ?
        if(m_FIFO_NbElements >= SIZE_FIFO){
            return false;
        }
        return true;
    }

    // --------------------------------------------------------------------------
    // Test si une transmission est en cours
    inline bool isBusy(){
        return m_Busy;
    }

    // --------------------------------------------------------------------------
    // Lecture de la largeur d'un bloc
    inline uint16_t getBlocWidth() {
        return m_BlocWidth ;
    }

    // --------------------------------------------------------------------------
    // Lecture de la hauteur de la frame
    inline uint16_t getBlocHeight() {
        return m_BlocHeight ;
    }

    // --------------------------------------------------------------------------
    // Test si un pixel du bloc a changé d'état
    inline bool getBlocChange(uint8_t Bloc){
        return m_BlocChange[Bloc];
    }

    // --------------------------------------------------------------------------
    // Remise à zéro de l'indicateur de changement d'état
    inline void resetBlocChange(uint8_t Bloc){
        m_BlocChange[Bloc] = false;
    }

    // --------------------------------------------------------------------------
    // Ajout d'un bloc dans le FIFO
    bool AddBloc(uint16_t x, uint16_t y);

    // --------------------------------------------------------------------------
    // Transmission des blocs contenus dans le FIFO
    bool sendDMA();
    
    // Callbacks en fin de transmission DMA
    static void sendCASETDMAData(void* context, daisy::SpiHandle::Result result);
    static void sendRASETDMACmd(void* context, daisy::SpiHandle::Result result);
    static void sendRASETDMAData(void* context, daisy::SpiHandle::Result result);
    static void sendRAWWRDMACmd(void* context, daisy::SpiHandle::Result result);  
    static void sendRAWWRDMAData(void* context, daisy::SpiHandle::Result result);
    static void endDMA(void* context, daisy::SpiHandle::Result result);
    
    // --------------------------------------------------------------------------
    // Configuration de la frame en mode portrait
    inline void setPortrait();

    // --------------------------------------------------------------------------
    // Configuration de la frame en mode paysage     
    inline void setLandscape();

    // Initialisation des blocs de tansmission
    void InitBlocs();

    RGB         *m_pFrame = nullptr;        // Pointe sur la mémoire de frame
    uint16_t    m_InitWidth = 0;            // Largeur de l'écran (Rotation 0)
    uint16_t    m_InitHeight = 0;           // Hauteur de l'écran (Rotation 0)
    uint16_t    m_Width = 0;                // Largeur de la frame
    uint16_t    m_Height = 0;               // Hauteur de la frame
    uint16_t    m_InitBlocWidth = 0;        // Largeur d'un bloc écran (Rotation 0)
    uint16_t    m_InitBlocHeight = 0;       // Hauteur d'un bloc écran (Rotation 0)
    uint16_t    m_BlocWidth = 0;            // Largeur d'un bloc
    uint16_t    m_BlocHeight = 0;           // Hauteur d'un bloc
    bool        m_BlocChange[NB_BLOC];      // Indicateurs de changment d'état des blocs
    
    // FIFO
    FIFO_Data   *m_pFIFO = nullptr;         // Pointe sur le FIFO de transmission bloc
    uint16_t    m_FIFO_in = 0;              // Index premier bloc libre du Bloc (entree du FIFO)
    uint16_t    m_FIFO_out = 0;             // Index du prochain bloc à transmette (Sortie du FIFO)
    uint16_t    m_FIFO_NbElements = 0;      // Nombre d'éléments dans le FIFO
    bool        m_Busy = false;             // Indicateur transmission en cours 
};
