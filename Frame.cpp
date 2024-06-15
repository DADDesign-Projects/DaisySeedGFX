//------------------------------------------------------------------------
// Copyright(c) 2024 Dad Design.
//      Gestion d'une frame de pixel
//      Les pixels sont organisés en blocs de transmission.
//      Lorsqu'un Bloc est modififé il est placé dans un FIFO 
//      et transmis automatiquement à l'écran via SPI en DMA.
//------------------------------------------------------------------------
#include "Debug.h"
#include "daisy.h"
#include "Frame.h"

//***********************************************************************************
// cRBG_Frame
//  Gestion de la framme
//*********************************************************************************** 

// --------------------------------------------------------------------------
// Initialisation de la classe cRBG_Frame.
// Doit être appelée avant toute utilisation (aucune vérification réalisée) 
void cRBG_Frame::setFrame(RGB *pFrameBuff,  FIFO_Data *pFIFO_Data, uint16_t Width,  uint16_t Height){
    Init_TFT_SPI();
    m_pFrame = pFrameBuff;
    m_pFIFO = pFIFO_Data;
    m_Width = Width;
    m_InitWidth = Width;
    m_Height = Height;
    m_InitHeight = Height;
    m_BlocWidth = Width/FRAME_GRILLE;
    m_InitBlocWidth = Width/FRAME_GRILLE;
    m_BlocHeight = Height/FRAME_GRILLE;
    m_InitBlocHeight = Height/FRAME_GRILLE;

    InitBlocs();
}

// --------------------------------------------------------------------------
// Configuration l'orientation de la frame et de l'écran
void cRBG_Frame::setFrameRotation (Rotation r){
    while(m_Busy==true){
        Delay(1);
    }
    setTFTRotation(r);
    switch (r) {
    case Rotation::Degre_0 :   // Portrait
    case Rotation::Degre_180 : // Inverter portrait
        setPortrait();
        break;

    case Rotation::Degre_90 : // Landscape (Portrait + 90)
    case Rotation::Degre_270 : // Inverted landscape
        setLandscape();
        break;
    }
}
// ---------------------------------------------------------------------------
// Initialisation des bloc
//   configure pour chaque pixel l'adresse du bloc correspondant
//   Doit être appelé pour chaque changement de d'orientation (Rotation)
//   
void cRBG_Frame::InitBlocs(){  
    RGB *pFrame = m_pFrame;
    for(uint16_t y =0; y < m_Height ; y ++){
        for(uint16_t x = 0; x < m_Width ; x++){
            uint16_t Bloc = (x/m_BlocWidth)+((y/m_BlocHeight)*(FRAME_GRILLE));
            pFrame->m_pChange = &m_BlocChange[Bloc];
            pFrame->B = 0;
            pFrame->G = 0;
            pFrame->R = 0;
            pFrame++;
        }
    }
    for(uint8_t IndexBloc=0; IndexBloc < NB_BLOC; IndexBloc++){
        m_BlocChange[IndexBloc]= true;
    }
    FlushFrame();
}

// --------------------------------------------------------------------------
// Configuration de la frame en mode portrait
void cRBG_Frame::setPortrait(){
    m_Height = m_InitHeight;
    m_Width = m_InitWidth;
    m_BlocHeight = m_InitBlocHeight;
    m_BlocWidth = m_InitBlocWidth;
    InitBlocs();
}

// --------------------------------------------------------------------------
// Configuration de la frame en mode paysage     
void cRBG_Frame::setLandscape(){
    m_Height = m_InitWidth;
    m_Width = m_InitHeight;
    m_BlocHeight = m_InitBlocWidth;
    m_BlocWidth = m_InitBlocHeight;
    InitBlocs();
}

// --------------------------------------------------------------------------
// Transmission des modifications de la frame vers l'écran
void cRBG_Frame::FlushFrame(){
    uint16_t x;
    uint16_t y;
    
    for(uint16_t Bloc = 0; Bloc < NB_BLOC; Bloc++){
        if(getBlocChange(Bloc) == true){
            resetBlocChange(Bloc);
            x= (Bloc % FRAME_GRILLE) * m_BlocWidth;
            y= (Bloc / FRAME_GRILLE) * m_BlocHeight;
        
            while(AddBloc(x, y) == false){
                System::DelayTicks(1);
            }
            sendDMA();
        }
    }
}
// --------------------------------------------------------------------------
// Ajout d'un bloc dans le FIFO
bool  cRBG_Frame::AddBloc( uint16_t x, uint16_t y){
    __disable_irq();
    if(m_FIFO_NbElements >= SIZE_FIFO){
        __enable_irq();
        return false;
    }
    __enable_irq();
    uint16_t dx = x + m_BlocWidth-1;
    uint16_t dy = y + m_BlocHeight-1;

    m_pFIFO->m_CmdCASET[m_FIFO_in].setData(x, dx);
    m_pFIFO->m_CmdRASET[m_FIFO_in].setData(y, dy);
    m_pFIFO->m_CmdRAWWR[m_FIFO_in].setData(x, y, dx, dy, this);
    m_FIFO_in +=1;
    if(m_FIFO_in >= SIZE_FIFO){
        m_FIFO_in = 0;
    }
    m_FIFO_NbElements+=1;
    return true;
}

// --------------------------------------------------------------------------
// Transmission des blocs contenus dans le FIFO
bool cRBG_Frame::sendDMA(){
    // Le FIFO n'est-il vide ou en cours de transmission
    __disable_irq();
    if((m_FIFO_NbElements == 0) || (m_Busy == true)){
        __enable_irq();
        return false;
    }
    __enable_irq();

    // On lance la transmission du premier bloc à transférer
    m_Busy = true;
    SendDMACommand(&m_pFIFO->m_CmdCASET[m_FIFO_out].m_Commande, cRBG_Frame::sendCASETDMAData, this);
    return true;
}

void cRBG_Frame::sendCASETDMAData(void* context, daisy::SpiHandle::Result result){
    cRBG_Frame *pthis = (cRBG_Frame *)context;
    pthis->SendDMAData(pthis->m_pFIFO->m_CmdCASET[pthis->m_FIFO_out].m_Data, 4, cRBG_Frame::sendRASETDMACmd, context);
}
// Comande RASET
void cRBG_Frame::sendRASETDMACmd(void* context, daisy::SpiHandle::Result result){
    cRBG_Frame *pthis = (cRBG_Frame *)context;
    pthis->SendDMACommand(&pthis->m_pFIFO->m_CmdRASET[pthis->m_FIFO_out].m_Commande, cRBG_Frame::sendRASETDMAData, context);
}
void cRBG_Frame::sendRASETDMAData(void* context, daisy::SpiHandle::Result result){
    cRBG_Frame *pthis = (cRBG_Frame *)context;
    pthis->SendDMAData(pthis->m_pFIFO->m_CmdRASET[pthis->m_FIFO_out].m_Data, 4, cRBG_Frame::sendRAWWRDMACmd, context);
}
// Commande RAAWWR
void cRBG_Frame::sendRAWWRDMACmd(void* context, daisy::SpiHandle::Result result){
    cRBG_Frame *pthis = (cRBG_Frame *)context;
    pthis->SendDMACommand(&pthis->m_pFIFO->m_CmdRAWWR[pthis->m_FIFO_out].m_Commande, cRBG_Frame::sendRAWWRDMAData, context);
}
void cRBG_Frame::sendRAWWRDMAData(void* context, daisy::SpiHandle::Result result){
    cRBG_Frame *pthis = (cRBG_Frame *)context;
    pthis->SendDMAData(pthis->m_pFIFO->m_CmdRAWWR[pthis->m_FIFO_out].m_Data, TAILLE_BLOC, cRBG_Frame::endDMA, context);
}

// Fin de transmission du bloc
void cRBG_Frame::endDMA(void* context, daisy::SpiHandle::Result result){
    cRBG_Frame *pthis = (cRBG_Frame *)context;
    
    // Bloc suivant
    pthis->m_FIFO_out++;
    if(pthis->m_FIFO_out >= SIZE_FIFO){
        pthis->m_FIFO_out = 0;
    }
    pthis->m_FIFO_NbElements -= 1;

    // Si le FIFO n'est pas vide -> Transmission du bloc suivant
    if(pthis->m_FIFO_NbElements != 0){
        pthis->SendDMACommand(&pthis->m_pFIFO->m_CmdCASET[pthis->m_FIFO_out].m_Commande, cRBG_Frame::sendCASETDMAData, context);
    }else{
        pthis->m_Busy = false;
    }
};

//***********************************************************************************
// Cmd_RAMWR
//   Commande SPI d'ecriture des pixels 
//***********************************************************************************

// --------------------------------------------------------------------------
// Définition des pixels de la frame à transférer
void Cmd_RAMWR::setData(uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, cRBG_Frame *pFrame){
    RGB *pFrameCourant;
    RGB *pEndLigne;
    uint8_t *pBloc = m_Data;

    for (uint16_t PosY = y; PosY <= dy; PosY++){
        pFrameCourant = pFrame->getPtr(x, PosY);
        pEndLigne = pFrame->getPtr(dx, PosY);
        while (pFrameCourant <= pEndLigne){
#if TFT_COLOR == 16
                *pBloc++ = (pFrameCourant->getR() & 0xF8) | (pFrameCourant->getG() >> 5 );
                *pBloc++ = (pFrameCourant->getB() >> 3) | ((pFrameCourant->getG() << 3 )  & 0xE0);
#else
                *pBloc++ = pFrameCourant->getR();
                *pBloc++ = pFrameCourant->getG();
                *pBloc++ = pFrameCourant->getB();
#endif
            pFrameCourant++;
        }
    }
}


