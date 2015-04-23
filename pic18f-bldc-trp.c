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
 * 64 niveaux et 18 angles de 0 à 170
 */
const unsigned char TAB_VITESSE[64][18] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 2, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 2, 2, 1, 1},
    {1, 2, 4, 5, 7, 9, 10, 11, 11, 11, 11, 11, 10, 9, 7, 5, 4, 2},
    {1, 2, 5, 7, 10, 12, 13, 14, 15, 15, 15, 14, 13, 12, 10, 7, 5, 2},
    {1, 3, 6, 9, 12, 15, 17, 18, 19, 19, 19, 18, 17, 15, 12, 9, 6, 3},
    {1, 4, 8, 11, 15, 18, 20, 22, 23, 23, 23, 22, 20, 18, 15, 11, 8, 4},
    {1, 4, 9, 13, 17, 21, 24, 26, 27, 27, 27, 26, 24, 21, 17, 13, 9, 4},
    {1, 5, 10, 15, 20, 24, 27, 29, 31, 31, 31, 29, 27, 24, 20, 15, 10, 5},
    {1, 6, 12, 17, 23, 27, 31, 33, 35, 35, 35, 33, 31, 27, 23, 17, 12, 6},
    {1, 6, 13, 19, 25, 30, 34, 37, 39, 39, 39, 37, 34, 30, 25, 19, 13, 6},
    {1, 7, 14, 21, 28, 33, 37, 41, 43, 43, 43, 41, 37, 33, 28, 21, 14, 7},
    {1, 8, 16, 23, 30, 36, 41, 44, 47, 47, 47, 44, 41, 36, 30, 23, 16, 8},
    {1, 8, 17, 25, 33, 39, 44, 48, 51, 51, 51, 48, 44, 39, 33, 25, 17, 8},
    {1, 9, 19, 27, 35, 42, 48, 52, 54, 55, 54, 52, 48, 42, 35, 27, 19, 9},
    {1, 10, 20, 29, 38, 45, 51, 56, 58, 59, 58, 56, 51, 45, 38, 29, 20, 10},
    {1, 11, 21, 31, 40, 48, 55, 59, 62, 63, 62, 59, 55, 48, 40, 31, 21, 11},
    {1, 11, 23, 33, 43, 51, 58, 63, 66, 67, 66, 63, 58, 51, 43, 33, 23, 11},
    {1, 12, 24, 35, 46, 54, 62, 67, 70, 71, 70, 67, 62, 54, 46, 35, 24, 12},
    {1, 13, 25, 37, 48, 57, 65, 71, 74, 75, 74, 71, 65, 57, 48, 37, 25, 13},
    {1, 13, 27, 39, 51, 61, 69, 74, 78, 79, 78, 74, 69, 61, 51, 39, 27, 13},
    {1, 14, 28, 41, 53, 64, 72, 78, 82, 83, 82, 78, 72, 64, 53, 41, 28, 14},
    {1, 15, 29, 43, 56, 67, 75, 82, 86, 87, 86, 82, 75, 67, 56, 43, 29, 15},
    {1, 15, 31, 45, 58, 70, 79, 86, 90, 91, 90, 86, 79, 70, 58, 45, 31, 15},
    {1, 16, 32, 47, 61, 73, 82, 89, 94, 95, 94, 89, 82, 73, 61, 47, 32, 16},
    {1, 17, 34, 49, 64, 76, 86, 93, 98, 99, 98, 93, 86, 76, 64, 49, 34, 17},
    {1, 17, 35, 51, 66, 79, 89, 97, 102, 103, 102, 97, 89, 79, 66, 51, 35, 17},
    {1, 18, 36, 53, 69, 82, 93, 101, 105, 107, 105, 101, 93, 82, 69, 53, 36, 18},
    {1, 19, 38, 55, 71, 85, 96, 104, 109, 111, 109, 104, 96, 85, 71, 55, 38, 19},
    {1, 20, 39, 57, 74, 88, 100, 108, 113, 115, 113, 108, 100, 88, 74, 57, 39, 20},
    {1, 20, 40, 59, 76, 91, 103, 112, 117, 119, 117, 112, 103, 91, 76, 59, 40, 20},
    {1, 21, 42, 61, 79, 94, 106, 116, 121, 123, 121, 116, 106, 94, 79, 61, 42, 21},
    {1, 22, 43, 63, 81, 97, 110, 119, 125, 127, 125, 119, 110, 97, 81, 63, 43, 22},
    {1, 22, 44, 65, 84, 100, 113, 123, 129, 131, 129, 123, 113, 100, 84, 65, 44, 22},
    {1, 23, 46, 67, 87, 103, 117, 127, 133, 135, 133, 127, 117, 103, 87, 67, 46, 23},
    {1, 24, 47, 69, 89, 106, 120, 131, 137, 139, 137, 131, 120, 106, 89, 69, 47, 24},
    {1, 24, 49, 71, 92, 109, 124, 134, 141, 143, 141, 134, 124, 109, 92, 71, 49, 24},
    {1, 25, 50, 73, 94, 112, 127, 138, 145, 147, 145, 138, 127, 112, 94, 73, 50, 25},
    {1, 26, 51, 75, 97, 115, 131, 142, 149, 151, 149, 142, 131, 115, 97, 75, 51, 26},
    {1, 26, 53, 77, 99, 119, 134, 146, 153, 155, 153, 146, 134, 119, 99, 77, 53, 26},
    {1, 27, 54, 79, 102, 122, 138, 149, 156, 159, 156, 149, 138, 122, 102, 79, 54, 27},
    {1, 28, 55, 81, 105, 125, 141, 153, 160, 163, 160, 153, 141, 125, 105, 81, 55, 28},
    {1, 29, 57, 83, 107, 128, 144, 157, 164, 167, 164, 157, 144, 128, 107, 83, 57, 29},
    {1, 29, 58, 85, 110, 131, 148, 160, 168, 171, 168, 160, 148, 131, 110, 85, 58, 29},
    {1, 30, 59, 87, 112, 134, 151, 164, 172, 175, 172, 164, 151, 134, 112, 87, 59, 30},
    {1, 31, 61, 89, 115, 137, 155, 168, 176, 179, 176, 168, 155, 137, 115, 89, 61, 31},
    {1, 31, 62, 91, 117, 140, 158, 172, 180, 183, 180, 172, 158, 140, 117, 91, 62, 31},
    {1, 32, 64, 93, 120, 143, 162, 175, 184, 187, 184, 175, 162, 143, 120, 93, 64, 32},
    {1, 33, 65, 95, 122, 146, 165, 179, 188, 191, 188, 179, 165, 146, 122, 95, 65, 33},
    {1, 33, 66, 97, 125, 149, 169, 183, 192, 195, 192, 183, 169, 149, 125, 97, 66, 33},
    {1, 34, 68, 99, 128, 152, 172, 187, 196, 199, 196, 187, 172, 152, 128, 99, 68, 34},
    {1, 35, 69, 101, 130, 155, 175, 190, 200, 203, 200, 190, 175, 155, 130, 101, 69, 35},
    {1, 35, 70, 103, 133, 158, 179, 194, 204, 207, 204, 194, 179, 158, 133, 103, 70, 35},
    {1, 36, 72, 105, 135, 161, 182, 198, 207, 211, 207, 198, 182, 161, 135, 105, 72, 36},
    {1, 37, 73, 107, 138, 164, 186, 202, 211, 215, 211, 202, 186, 164, 138, 107, 73, 37},
    {1, 38, 74, 109, 140, 167, 189, 205, 215, 219, 215, 205, 189, 167, 140, 109, 74, 38},
    {1, 38, 76, 111, 143, 170, 193, 209, 219, 223, 219, 209, 193, 170, 143, 111, 76, 38},
    {1, 39, 77, 113, 145, 173, 196, 213, 223, 227, 223, 213, 196, 173, 145, 113, 77, 39},
    {1, 40, 79, 115, 148, 177, 200, 217, 227, 231, 227, 217, 200, 177, 148, 115, 79, 40},
    {1, 40, 80, 117, 151, 180, 203, 220, 231, 235, 231, 220, 203, 180, 151, 117, 80, 40},
    {1, 41, 81, 119, 153, 183, 207, 224, 235, 239, 235, 224, 207, 183, 153, 119, 81, 41},
    {1, 42, 83, 121, 156, 186, 210, 228, 239, 243, 239, 228, 210, 186, 156, 121, 83, 42},
    {1, 42, 84, 123, 158, 189, 213, 232, 243, 247, 243, 232, 213, 189, 158, 123, 84, 42},
    {1, 43, 85, 125, 161, 192, 217, 235, 247, 251, 247, 235, 217, 192, 161, 125, 85, 43},
    {1, 44, 87, 127, 163, 195, 220, 239, 251, 255, 251, 239, 220, 195, 163, 127, 87, 44}};

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
        ccp->ccpb = TAB_VITESSE[puissance][alpha - 12];
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
    switch (etat)
    {
        case ARRET:
            switch (evenement)
            {
                case BLOCAGE:
                    break;
                case PHASE:
                    break;
                case TICTAC:
                    break;
                case VITESSE:
                    break;
            }
            break;
        case BLOQUE:
            switch (evenement)
            {
                case BLOCAGE:
                    break;
                case PHASE:
                    break;
                case TICTAC:
                    break;
                case VITESSE:
                    break;
            }
            break;
        case DEMARRAGE:
            switch (evenement)
            {
                case BLOCAGE:
                    break;
                case PHASE:
                    break;
                case TICTAC:
                    break;
                case VITESSE:
                    break;
            }
            break;
        case EN_MOUVEMENT:
            switch (evenement)
            {
                case BLOCAGE:
                    break;
                case PHASE:
                    break;
                case TICTAC:
                    break;
                case VITESSE:
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