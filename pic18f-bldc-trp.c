/* 
 * File:   pic18f-bldc-trp.c
 * Author: jmgonet
 *
 * Created on March 29, 2015, 10:21 AM
 */
#include <htc.h>
#include <stdio.h>
#include <stdlib.h>

#include "test.h"

/**
 * Bits de configuration:
 */
#pragma config FOSC = INTIO67  // Oscillateur interne, ports A6 et A7 comme IO.
#pragma config IESO = OFF      // Pas d'embrouilles avec l'osc. au démarrage.
#pragma config FCMEN = OFF     // Pas de monitorage de l'oscillateur.

// Nécessaires pour ICSP / ICD:
#pragma config MCLRE = EXTMCLR // RE3 est actif comme master reset.
#pragma config WDTEN = OFF     // Watchdog inactif (pour ICSP /ICD)
#pragma config LVP = OFF       // Single Supply Enable bits off.

// Sortie CCP3 sur RC6:
#pragma config CCP3MX = PORTC6

#define ERROR 255
#define CYCLE 36

#define PUISSANCE_MAX 40
#define PUISSANCE_ARRET 0
#define PUISSANCE_DEPART 5

/* tableau de contanstes pour les vitesses
 * 65 niveaux et 18 angles de 0 à 170
 */
const unsigned char TAB_VITESSE[65][18] = {
{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1},
{1, 2, 3, 4, 6, 7, 7, 8, 8, 8, 8, 8, 7, 7, 6, 4, 3, 2},
{1, 3, 5, 6, 8, 10, 11, 12, 12, 12, 12, 12, 11, 10, 8, 6, 5, 3},
{1, 3, 6, 8, 11, 13, 14, 15, 16, 16, 16, 15, 14, 13, 11, 8, 6, 3},
{1, 4, 7, 10, 13, 16, 18, 19, 20, 20, 20, 19, 18, 16, 13, 10, 7, 4},
{1, 5, 9, 12, 16, 19, 21, 23, 24, 24, 24, 23, 21, 19, 16, 12, 9, 5},
{1, 5, 10, 14, 18, 22, 25, 27, 28, 28, 28, 27, 25, 22, 18, 14, 10, 5},
{1, 6, 11, 16, 21, 25, 28, 30, 32, 32, 32, 30, 28, 25, 21, 16, 11, 6},
{1, 7, 13, 18, 24, 28, 32, 34, 36, 36, 36, 34, 32, 28, 24, 18, 13, 7},
{1, 7, 14, 20, 26, 31, 35, 38, 40, 40, 40, 38, 35, 31, 26, 20, 14, 7},
{1, 8, 15, 22, 29, 34, 38, 42, 44, 44, 44, 42, 38, 34, 29, 22, 15, 8},
{1, 9, 17, 24, 31, 37, 42, 45, 48, 48, 48, 45, 42, 37, 31, 24, 17, 9},
{1, 10, 18, 26, 34, 40, 45, 49, 52, 52, 52, 49, 45, 40, 34, 26, 18, 10},
{1, 10, 20, 28, 36, 43, 49, 53, 55, 56, 55, 53, 49, 43, 36, 28, 20, 10},
{1, 11, 21, 30, 39, 46, 52, 57, 59, 60, 59, 57, 52, 46, 39, 30, 21, 11},
{1, 12, 22, 32, 41, 49, 56, 60, 63, 64, 63, 60, 56, 49, 41, 32, 22, 12},
{1, 12, 24, 34, 44, 52, 59, 64, 67, 68, 67, 64, 59, 52, 44, 34, 24, 12},
{1, 13, 25, 36, 47, 55, 63, 68, 71, 72, 71, 68, 63, 55, 47, 36, 25, 13},
{1, 14, 26, 38, 49, 58, 66, 72, 75, 76, 75, 72, 66, 58, 49, 38, 26, 14},
{1, 14, 28, 40, 52, 62, 70, 75, 79, 80, 79, 75, 70, 62, 52, 40, 28, 14},
{1, 15, 29, 42, 54, 65, 73, 79, 83, 84, 83, 79, 73, 65, 54, 42, 29, 15},
{1, 16, 30, 44, 57, 68, 76, 83, 87, 88, 87, 83, 76, 68, 57, 44, 30, 16},
{1, 16, 32, 46, 59, 71, 80, 87, 91, 92, 91, 87, 80, 71, 59, 46, 32, 16},
{1, 17, 33, 48, 62, 74, 83, 90, 95, 96, 95, 90, 83, 74, 62, 48, 33, 17},
{1, 18, 35, 50, 65, 77, 87, 94, 99, 100, 99, 94, 87, 77, 65, 50, 35, 18},
{1, 19, 36, 52, 67, 80, 90, 98, 103, 104, 103, 98, 90, 80, 67, 52, 36, 19},
{1, 19, 37, 54, 70, 83, 94, 102, 106, 108, 106, 102, 94, 83, 70, 54, 37, 19},
{1, 20, 39, 56, 72, 86, 97, 105, 110, 112, 110, 105, 97, 86, 72, 56, 39, 20},
{1, 21, 40, 58, 75, 89, 101, 109, 114, 116, 114, 109, 101, 89, 75, 58, 40, 21},
{1, 21, 41, 60, 77, 92, 104, 113, 118, 120, 118, 113, 104, 92, 77, 60, 41, 21},
{1, 22, 43, 62, 80, 95, 107, 117, 122, 124, 122, 117, 107, 95, 80, 62, 43, 22},
{1, 23, 44, 64, 82, 98, 111, 120, 126, 128, 126, 120, 111, 98, 82, 64, 44, 23},
{1, 23, 45, 66, 85, 101, 114, 124, 130, 132, 130, 124, 114, 101, 85, 66, 45, 23},
{1, 24, 47, 68, 88, 104, 118, 128, 134, 136, 134, 128, 118, 104, 88, 68, 47, 24},
{1, 25, 48, 70, 90, 107, 121, 132, 138, 140, 138, 132, 121, 107, 90, 70, 48, 25},
{1, 25, 50, 72, 93, 110, 125, 135, 142, 144, 142, 135, 125, 110, 93, 72, 50, 25},
{1, 26, 51, 74, 95, 113, 128, 139, 146, 148, 146, 139, 128, 113, 95, 74, 51, 26},
{1, 27, 52, 76, 98, 116, 132, 143, 150, 152, 150, 143, 132, 116, 98, 76, 52, 27},
{1, 28, 54, 78, 100, 120, 135, 147, 154, 156, 154, 147, 135, 120, 100, 78, 54, 28},
{1, 28, 55, 80, 103, 123, 139, 150, 157, 160, 157, 150, 139, 123, 103, 80, 55, 28},
{1, 29, 56, 82, 106, 126, 142, 154, 161, 164, 161, 154, 142, 126, 106, 82, 56, 29},
{1, 30, 58, 84, 108, 129, 145, 158, 165, 168, 165, 158, 145, 129, 108, 84, 58, 30},
{1, 30, 59, 86, 111, 132, 149, 162, 169, 172, 169, 162, 149, 132, 111, 86, 59, 30},
{1, 31, 60, 88, 113, 135, 152, 165, 173, 176, 173, 165, 152, 135, 113, 88, 60, 31},
{1, 32, 62, 90, 116, 138, 156, 169, 177, 180, 177, 169, 156, 138, 116, 90, 62, 32},
{1, 32, 63, 92, 118, 141, 159, 173, 181, 184, 181, 173, 159, 141, 118, 92, 63, 32},
{1, 33, 65, 94, 121, 144, 163, 177, 185, 188, 185, 177, 163, 144, 121, 94, 65, 33},
{1, 34, 66, 96, 123, 147, 166, 180, 189, 192, 189, 180, 166, 147, 123, 96, 66, 34},
{1, 34, 67, 98, 126, 150, 170, 184, 193, 196, 193, 184, 170, 150, 126, 98, 67, 34},
{1, 35, 69, 100, 129, 153, 173, 188, 197, 200, 197, 188, 173, 153, 129, 100, 69, 35},
{1, 36, 70, 102, 131, 156, 176, 192, 201, 204, 201, 192, 176, 156, 131, 102, 70, 36},
{1, 37, 71, 104, 134, 159, 180, 195, 205, 208, 205, 195, 180, 159, 134, 104, 71, 37},
{1, 37, 73, 106, 136, 162, 183, 199, 209, 212, 209, 199, 183, 162, 136, 106, 73, 37},
{1, 38, 74, 108, 139, 165, 187, 203, 212, 216, 212, 203, 187, 165, 139, 108, 74, 38},
{1, 39, 75, 110, 141, 168, 190, 206, 216, 220, 216, 206, 190, 168, 141, 110, 75, 39},
{1, 39, 77, 112, 144, 171, 194, 210, 220, 224, 220, 210, 194, 171, 144, 112, 77, 39},
{1, 40, 78, 114, 147, 174, 197, 214, 224, 228, 224, 214, 197, 174, 147, 114, 78, 40},
{1, 41, 80, 116, 149, 178, 201, 218, 228, 232, 228, 218, 201, 178, 149, 116, 80, 41},
{1, 41, 81, 118, 152, 181, 204, 221, 232, 236, 232, 221, 204, 181, 152, 118, 81, 41},
{1, 42, 82, 120, 154, 184, 208, 225, 236, 240, 236, 225, 208, 184, 154, 120, 82, 42},
{1, 43, 84, 122, 157, 187, 211, 229, 240, 244, 240, 229, 211, 187, 157, 122, 84, 43},
{1, 43, 85, 124, 159, 190, 214, 233, 244, 248, 244, 233, 214, 190, 159, 124, 85, 43},
{1, 44, 86, 126, 162, 193, 218, 236, 248, 252, 248, 236, 218, 193, 162, 126, 86, 44},
{1, 45, 88, 128, 164, 196, 221, 240, 252, 255, 252, 240, 221, 196, 164, 128, 88, 45}
};

struct CCP {
    unsigned char ccpa;
    unsigned char ccpb;
    unsigned char ccpc;
};

enum DIRECTION {
    AVANT,
    ARRIERE
};

enum EVENEMENT {
    /** Fin de période du PWM. */
    TICTAC,

    /** Le moteur vient de changer de phase.*/
    PHASE,

    /** Il s'est ecoulé trop de temps depuis le dernier changement de phase. */
    BLOCAGE,
    
    /** La vitesse demandée a varié. */
    VITESSE
};

enum STATUS {
    /**
     * Le moteur est en arrêt.
     */
    ARRET,

    /**
     * Le moteur est en train de démarrer. Il n'est pas encore possible
     * de calculer sa vitesse.
     */
    DEMARRAGE,

    /**
     * Le moteur est en mouvement. Sa vitesse est stable ou
     * elle varie lentement.
     */
    EN_MOUVEMENT,

    /**
     * Le moteur est bloqué.
     */
    BLOQUE
};

/*
 * variables globales
 *
 *
 */

enum STATUS etat;
unsigned char phaseActuelle;
unsigned char puissanceActuelle;
unsigned char angleActuel;

/**
 * Rend les valeurs PWM para rapport à l'angle spécifié.
 * À appeler lorsque l'angle est connu, c'est à dire, lorsque le moteur
 * est en mouvement.
 * @param alpha Angle, entre 0 et 35.
 * @param puissance, entre 0 et 50.
 * @param ccp Structure pour les valeurs PWM.
 */
void calculeAmplitudesEnMouvement(unsigned char alpha, unsigned char puissance, struct CCP *ccp) {
    if (puissance > PUISSANCE_MAX) puissance = PUISSANCE_MAX;
    if (alpha < 12) {
        ccp->ccpa = TAB_VITESSE[puissance][alpha];
        ccp->ccpb = TAB_VITESSE[puissance][0];
        ccp->ccpc = TAB_VITESSE[puissance][alpha + 6];
    } else if (alpha < 24) {
        ccp->ccpa = TAB_VITESSE[puissance][alpha - 6];
        ccp->ccpb = TAB_VITESSE[puissance][alpha - 12];
        ccp->ccpc = TAB_VITESSE[puissance][0];
    } else {
        ccp->ccpa = TAB_VITESSE[puissance][0];
        ccp->ccpb = TAB_VITESSE[puissance][alpha - 18];
        ccp->ccpc = TAB_VITESSE[puissance][alpha - 24];
    }

};

/**
 * Rend les valeurs PWM para rapport à la phase spécifiée.
 * À appeler lorsque seule la phase est connue, c'est à dire lorsque le moteur
 * est à l'arret.
 * @param phase Phase, entre 0 et 5.
 * @param ccp Structure pour les valeurs PWM.
 */
void calculeAmplitudesArret(unsigned char phase, struct CCP *ccp) {
    unsigned char alpha;
    switch (phase)
    {
        case 1:
            alpha = 0;
            break;
        case 2:
            alpha = 6;
            break;
        case 3:
            alpha = 12;
            break;
        case 4:
            alpha = 18;
            break;
        case 5:
            alpha = 24;
            break;
        case 6:
            alpha = 30;
            break;
        default:
            alpha = 0;
            break;
    }
    calculeAmplitudesEnMouvement(alpha, PUISSANCE_ARRET, ccp);
}

/**
 * Determine la phase en cours d'après les senseurs hall.
 * @param hall La valeur des senseurs hall: 0xb*****yzx
 * @return Le numéro de phase, entre 1 et 6.
 */
unsigned char phaseSelonHall(unsigned char hall) {
    unsigned char c;
    unsigned char phase;

    c = hall & 0x07;
    switch(c)
    {
        case 0b001:
            phase = 1;
            break;
        case 0b011:
            phase = 2;
            break;
        case 0b010:
            phase = 3;
            break;
        case 0b110:
            phase = 4;
            break;
        case 0b100:
            phase = 5;
            break;
        case 0b101:
            phase = 6;
            break;
        default:
            phase = ERROR;
            break;
    }
    return phase;
}

/**
 * Calcule la phase en cours à partir de la lecture des senseurs hall.
 * Effectue également un contrôle de la lecture, pour vérifier si elle est
 * possible. Ceci sert à éviter de compter des rebondissements ou du bruit
 * qui affecte la lecture des senseurs.
 * @param hall La valeur des senseurs hall: 0xb*****yzx
 * @param direction Direction actuelle.
 * @return La phase (de 0 à 5) ou un code d'erreur.
 */
unsigned char phaseSelonHallEtDirection(unsigned char hall, enum DIRECTION direction) {
    unsigned char  phase;
    unsigned char lastPhase = ERROR;

    phase = phaseSelonHall(hall);
    
    // if last_phase == error alors c'est la première fois
    if (ERROR == lastPhase) {
        lastPhase = phase;
    } else {
        if (AVANT == direction)
        {
            switch (phase)
            {
                case 1:
                    if (6 != lastPhase) phase = ERROR;
                    break;
                case 2:
                    if (1 != lastPhase) phase = ERROR;
                    break;
                case 3:
                    if (2 != lastPhase) phase = ERROR;
                    break;
                case 4:
                    if (3 != lastPhase) phase = ERROR;
                    break;
                case 5:
                    if (4 != lastPhase) phase = ERROR;
                    break;
                case 6:
                    if (5 != lastPhase) phase = ERROR;
                    break;
            }
        } else if (ARRIERE == direction) {
            switch (phase)
            {
                case 1:
                    if (2 != lastPhase) phase = ERROR;
                    break;
                case 2:
                    if (3 != lastPhase) phase = ERROR;
                    break;
                case 3:
                    if (4 != lastPhase) phase = ERROR;
                    break;
                case 4:
                    if (5 != lastPhase) phase = ERROR;
                    break;
                case 5:
                    if (6 != lastPhase) phase = ERROR;
                    break;
                case 6:
                    if (1 != lastPhase) phase = ERROR;
                    break;
            }
        } else {
            phase = ERROR;
        }
    }

    lastPhase = phase;

    return phase;
}

/**
 * Calcule l'angle correspondant à la phase et à la direction actuelle
 * de rotation.
 * @param phase Phase actuelle.
 * @param direction Direction actuelle.
 * @return L'angle correspondant entre 0 et 35.
 */
unsigned char angleSelonPhaseEtDirection(unsigned char phase, enum DIRECTION direction) {
    unsigned char angle;
    switch (phase)
    {
        case 1:
            if (AVANT == direction) {
                angle = 0;
            } else {
                angle = 6;
            }
            break;
        case 2:
            if (AVANT == direction) {
                angle = 6;
            } else {
                angle = 12;
            }
            break;
        case 3:
            if (AVANT == direction) {
                angle = 12;
            } else {
                angle = 18;
            }
            break;
        case 4:
            if (AVANT == direction) {
                angle = 18;
            } else {
                angle = 24;
            }
            break;
        case 5:
            if (AVANT == direction) {
                angle = 24;
            } else {
                angle = 30;
            }
            break;
        case 6:
            if (AVANT == direction) {
                angle = 30;
            } else {
                angle = 0;
            }
            break;
    }
    return angle;
}

/**
 * Cette fonction est appelée en réponse à un changement de phase. À
 * cet instant on connait la valeur exacte des deux paramètres.
 * @param angle Angle exact.
 * @param dureeDePhase Durée de la dernière phase.
 */
void corrigeAngleEtVitesse(unsigned char angle, int dureeDePhase) {

}

/**
 * Cette fonction est appelée à chaque cycle de PWM pour calculer (estimer)
 * l'angle actuel.
 * Le calcul se fait sur la base du dernier angle connu avec précision et
 * de la durée de la dernière phase. Ces valeurs ont été établies par l'appel
 * à 'corrigeAngleEtVitesse'.
 * @return L'angle actuel estimé.
 */
unsigned char calculeAngle() {
    unsigned char angle;
    return 0;
}

/**
 * Etablit la puissance de départ.
 * @param p La puissance de départ.
 */
void etablitPuissance(unsigned char p) {
    puissanceActuelle = p;
}

/**
 * Calcule le facteur de puissance nécessaire selon la vitesse demandée.
 * @param dureeDePhase Durée de phase actuelle.
 * @param vitesse Vitesse demandée.
 * @return Puissance à appliquer.
 */
unsigned char calculePuissance(int dureeDePhase, unsigned char vitesse) {
    return 0;
}

void machine(enum EVENEMENT evenement, unsigned char x, struct CCP *ccp) {

    static enum STATUS status = ARRET;
    static char phase  = 0;  // compteur de phase pour le DEMARRAGE.
    unsigned char angle;        // Utillisé dans DEMARRAGE
    char duree_phase = 0;
    char nbr_phase = 0;
    char nbr_blocage = 0;

    switch (status) {
        case ARRET: // Le moteur est en arrêt.
            switch (evenement) {
                case TICTAC: // Fin de période du PWM.
                    /* Répond aux événements TICTAC avec un CCP minimum. */
                    ccp -> ccpa = 1;    // Pour la simulation !
                    ccp -> ccpb = 1;
                    ccp -> ccpc = 1;
                    break;
                case PHASE: // Le moteur vient de changer de phase.
                    /* Ne fait rien */
                    break;
                case BLOCAGE: // Il s'est ecoulé trop de temps depuis le dernier changement de phase.
                    /* Ne fait rien */
                    break;
                case VITESSE: // La vitesse demandée a varié.
                    /* Si une vitesse supérieure à [X] est demandée, elle passe à l'état DEMARRAGE. */
                    if (x > 10){
                        status = DEMARRAGE;
                    }
                    break;
            }
            break;
        case DEMARRAGE: // Le moteur est en train de démarrer. Il n'est pas encore possible de calculer sa vitesse.
            switch (evenement) {
                case TICTAC: // Fin de période du PWM.
                    phase = phaseSelonHall(x);  // x correspond à la valeur des capteurs Hall.
                    calculeAmplitudesArret(phase, ccp);
                    duree_phase++;
                    break;
                case PHASE: // Le moteur vient de changer de phase.
                    phase = phaseSelonHallEtDirection(x, AVANT); // x correspond à la valeur des capteurs Hall.

                    if(phase != ERROR){
                        nbr_phase++;
                    }

                    if(nbr_phase > 5){
                        angle = angleSelonPhaseEtDirection(phase, AVANT);
                        corrigeAngleEtVitesse(angle, duree_phase);

                        status = EN_MOUVEMENT;
                    }
                    break;
                case BLOCAGE: // Il s'est ecoulé trop de temps depuis le dernier changement de phase.
                    nbr_phase = 0;
                    nbr_blocage++;
                    if(nbr_blocage > 9){
                        status = BLOQUE;
                    }
                    break;
                case VITESSE: // La vitesse demandée a varié.
                    /* Ne fait rien */
                    break;
            }
            break;
        case EN_MOUVEMENT: // Le moteur est en mouvement. Sa vitesse est stable ou elle varie lentement.
            switch (evenement) {
                case TICTAC: // Fin de période du PWM.
                    break;
                case PHASE: // Le moteur vient de changer de phase.
                    break;
                case BLOCAGE: // Il s'est ecoulé trop de temps depuis le dernier changement de phase.
                    break;
                case VITESSE: // La vitesse demandée a varié.
                    break;
            }
            break;
        case BLOQUE: // Le moteur est bloqué.
            switch (evenement) {
                case TICTAC: // Fin de période du PWM.
                    ccp -> ccpa = 1;    // Pour la simulation !
                    ccp -> ccpb = 1;
                    ccp -> ccpc = 1;
                    break;
                case PHASE: // Le moteur vient de changer de phase.
                    /* Ne fait rien */
                    break;
                case BLOCAGE: // Il s'est ecoulé trop de temps depuis le dernier changement de phase.
                    /* Ne fait rien */
                    break;
                case VITESSE: // La vitesse demandée a varié.
                    /* Ne fait rien */
                    break;
            }
            break;
    }
}

#ifndef TEST
/**
 * Interruptions de haute priorité.
 * - Lecture de la télécommande.
 */
void interrupt interruptionsHP() {

}

/**
 * Interruptions de basse priorité.
 * - Met à jour les cycles de travail.
 * - Avertit des changements de phase.
 * - Avertit que le moteur est arrêté.
 */
void low_priority interrupt interruptionsBP() {

}

/**
 * Point d'entrée.
 */
void main() {
    
    ANSELA = 0x00; // Désactive les convertisseurs A/D.
    ANSELB = 0x00; // Désactive les convertisseurs A/D.
    ANSELC = 0x00; // Désactive les convertisseurs A/D.


    // initialisation des variables globales
    etat = DEMARRAGE;

    etablitPuissance(PUISSANCE_DEPART);

    //configurer et activer le timer pour la lecture de la telecommande

    //configurer le port pour lire les sensors hall et les interrupt
    // init la phase et l'angle selon le status des hall

    // configurer la sortie des PWM

    // configurer l'interrupt du PWM (evenement TICTAC)

    // configurer

    // Ne fait plus rien. Les interruptions s'en chargent.
    while(1);
}
#endif

#ifdef TEST

unsigned char test_calculeAmplitudesEnMouvement() {
    unsigned char ft = 0;
    struct CCP ccp;

    calculeAmplitudesEnMouvement(0, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpa,  1, "CCP-00A");
    ft += assertEqualsChar(ccp.ccpb,  1, "CCP-00B");
    ft += assertEqualsChar(ccp.ccpc, 35, "CCP-00C");

    calculeAmplitudesEnMouvement(3, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpc, 40, "CCP-03C");

    calculeAmplitudesEnMouvement(9, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 40, "CCP-09A");

    calculeAmplitudesEnMouvement(12, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 35, "CCP-12A");
    ft += assertEqualsChar(ccp.ccpb,  1, "CCP-12B");
    ft += assertEqualsChar(ccp.ccpc,  1, "CCP-12C");

    calculeAmplitudesEnMouvement(15, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 40, "CCP-15A");

    calculeAmplitudesEnMouvement(21, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpb, 40, "CCP-21B");

    calculeAmplitudesEnMouvement(24, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpa,  1, "CCP-24A");
    ft += assertEqualsChar(ccp.ccpb, 35, "CCP-24B");
    ft += assertEqualsChar(ccp.ccpc,  1, "CCP-24C");

    calculeAmplitudesEnMouvement(27, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpb, 40, "CCP-27B");

    calculeAmplitudesEnMouvement(32, 10, &ccp);
    ft += assertEqualsChar(ccp.ccpc, 40, "CCP-32C");

    return ft;
}

unsigned char test_phaseSelonHall() {
    unsigned char ft = 0;

    ft += assertEqualsChar(ERROR, phaseSelonHall(0), "PSH-00");
    ft += assertEqualsChar(1, phaseSelonHall(0b001), "PSH-01");
    ft += assertEqualsChar(2, phaseSelonHall(0b011), "PSH-02");
    ft += assertEqualsChar(3, phaseSelonHall(0b010), "PSH-03");
    ft += assertEqualsChar(4, phaseSelonHall(0b110), "PSH-04");
    ft += assertEqualsChar(5, phaseSelonHall(0b100), "PSH-05");
    ft += assertEqualsChar(6, phaseSelonHall(0b101), "PSH-06");
    ft += assertEqualsChar(ERROR, phaseSelonHall(7), "PSH-07");

    return ft;
}

unsigned char test_calculeAmplitudesArret() {
    unsigned char ft = 0;
    struct CCP ccp;

    calculeAmplitudesArret(0, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 1, "CAA-0A");
    ft += assertEqualsChar(ccp.ccpb, 1, "CAA-0B");
    ft += assertEqualsChar(ccp.ccpc, 1, "CAA-0C");

    calculeAmplitudesArret(1, &ccp);
    ft += assertEqualsChar(ccp.ccpb, 1, "CAA-1B");
    ft += assertEqualsChar(2 * ccp.ccpa, ccp.ccpc, "CAA-1AC");

    calculeAmplitudesArret(2, &ccp);
    ft += assertEqualsChar(ccp.ccpb, 1, "CAA-2B");
    ft += assertEqualsChar(ccp.ccpa, 2 * ccp.ccpc, "CAA-2AC");

    calculeAmplitudesArret(3, &ccp);
    ft += assertEqualsChar(ccp.ccpc, 1, "CAA-3C");
    ft += assertEqualsChar(ccp.ccpa, 2 * ccp.ccpb, "CAA-3AB");

    calculeAmplitudesArret(4, &ccp);
    ft += assertEqualsChar(ccp.ccpc, 1, "CAA-4C");
    ft += assertEqualsChar(2 * ccp.ccpa, ccp.ccpb, "CAA-4AB");

    calculeAmplitudesArret(5, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 1, "CAA-5A");
    ft += assertEqualsChar(ccp.ccpb, 2 * ccp.ccpc, "CAA-5BC");

    calculeAmplitudesArret(6, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 1, "CAA-5A");
    ft += assertEqualsChar(2 * ccp.ccpb, ccp.ccpc, "CAA-5BC");

    calculeAmplitudesArret(7, &ccp);
    ft += assertEqualsChar(ccp.ccpa, 1, "CAA-7A");
    ft += assertEqualsChar(ccp.ccpb, 1, "CAA-7B");
    ft += assertEqualsChar(ccp.ccpc, 1, "CAA-7C");

    return ft;
}

unsigned char test_phaseSelonHallEtDirection() {
    unsigned char ft = 0;

    ft += assertEqualsChar(phaseSelonHallEtDirection(0b001, AVANT), 1, "PHD-10");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b011, AVANT), 2, "PHD-20");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b010, AVANT), 3, "PHD-30");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b110, AVANT), 4, "PHD-40");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b100, AVANT), 5, "PHD-50");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b101, AVANT), 6, "PHD-60");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b001, AVANT), 1, "PHD-11");

    ft += assertEqualsChar(phaseSelonHallEtDirection(0b001, AVANT), ERROR, "PHD-E0");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b011, AVANT), 2, "PHD-E1");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b011, AVANT), ERROR, "PHD-E1");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b001, AVANT), ERROR, "PHD-E3");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b010, AVANT), 3, "PHD-E4");
    ft += assertEqualsChar(phaseSelonHallEtDirection(0b011, ARRIERE), 2, "PHD-E1");

    return ft;
}

unsigned char test_angleSelonPhaseEtDirection() {
    unsigned char ft = 0;

    ft += assertEqualsChar(angleSelonPhaseEtDirection(0, AVANT), ERROR, "CA-001");
    ft += assertEqualsChar(angleSelonPhaseEtDirection(1, AVANT),     0, "CA-002");
    ft += assertEqualsChar(angleSelonPhaseEtDirection(1, ARRIERE),   6, "CA-003");
    ft += assertEqualsChar(angleSelonPhaseEtDirection(2, AVANT),     6, "CA-004");

    return ft;
}

unsigned char test_calculePuissance() {
    unsigned char ft = 0;
    unsigned char n;
    unsigned char puissance;

    int dureeDePhase;

    dureeDePhase = 1000;
    for (n = 0; n < 10; n++) {
        puissance = calculePuissance(dureeDePhase, 15);
        dureeDePhase = 200 - 5 * puissance;
    }
    ft += assertMinMaxInt(dureeDePhase, 75, 85, "PID-05A");

    dureeDePhase = 10;
    for (n = 0; n < 10; n++) {
        puissance = calculePuissance(dureeDePhase, 15);
        dureeDePhase = 200 - 5 * puissance;
    }
    ft += assertMinMaxInt(dureeDePhase, 75, 85, "PID-05B");

    dureeDePhase = 1000;
    for (n = 0; n < 10; n++) {
        puissance = calculePuissance(dureeDePhase, 15);
        dureeDePhase = 200 - 6 * puissance;
    }
    ft += assertMinMaxInt(dureeDePhase, 75, 85, "PID-06A");

    dureeDePhase = 10;
    for (n = 0; n < 10; n++) {
        puissance = calculePuissance(dureeDePhase, 15);
        dureeDePhase = 200 - 6 * puissance;
    }
    ft += assertMinMaxInt(dureeDePhase, 75, 85, "PID-06B");

    return ft;
}

unsigned char test_calculeAngle() {
    unsigned char ft = 0;
    unsigned char n;

    corrigeAngleEtVitesse(0, 40);
    for (n = 0; n < 120; n++) {
        calculeAngle();
    }
    assertEqualsChar(calculeAngle(), 18, "CA-18");

    corrigeAngleEtVitesse(0, 50);
    for (n = 0; n < 120; n++) {
        calculeAngle();
    }
    assertEqualsChar(calculeAngle(), 15, "CA-15");

    corrigeAngleEtVitesse(15, 50);
    for (n = 0; n < 120; n++) {
        calculeAngle();
    }
    assertEqualsChar(calculeAngle(), 30, "CA-15B");

    return ft;
}
/**
 * Point d'entrée pour les tests unitaires.
 */
void main() {
    unsigned char ft = 0;

    // Initialise la EUSART pour pouvoir écrire dans la console
    // Activez la UART1 dans les propriétés du projet.
    EUSART_Initialize();
    printf("Lancement des tests...\r\n");

    // Exécution des tests:
    ft += test_phaseSelonHall();
    ft += test_calculeAmplitudesArret();

    ft += test_phaseSelonHallEtDirection();
    ft += test_angleSelonPhaseEtDirection();
    ft += test_calculeAngle();
    ft += test_calculeAmplitudesEnMouvement();

    ft += test_calculePuissance();

    // Affiche le résultat des tests:
    printf("%u tests en erreur\r\n",ft);
    SLEEP();
}
#endif