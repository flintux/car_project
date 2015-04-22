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

typedef struct {
    unsigned char ccpa;
    unsigned char ccpb;
    unsigned char ccpc;
} ccp_t;

typedef enum {
    AVANT,
    ARRIERE
} direction_t;

typedef enum {
    PHASE_1,
    PHASE_2,
    PHASE_3,
    PHASE_4,
    PHASE_5,
    PHASE_6,
    PHASE_ERROR
} phase_t;

typedef enum {
    /** Fin de période du PWM. */
    TICTAC,

    /** Le moteur vient de changer de phase.*/
    PHASE,

    /** Il s'est ecoulé trop de temps depuis le dernier changement de phase. */
    BLOCAGE,
    
    /** La vitesse demandée a varié. */
    VITESSE
} evenement_t;

typedef enum {
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
} status_t;

/**
 * Rend les valeurs PWM para rapport à l'angle spécifié.
 * À appeler lorsque l'angle est connu, c'est à dire, lorsque le moteur
 * est en mouvement.
 * @param alpha Angle, entre 0 et 35.
 * @param puissance, entre 0 et 50.
 * @param ccp Structure pour les valeurs PWM.
 */
void calculeAmplitudesEnMouvement(unsigned char alpha, unsigned char puissance, ccp_t *ccp) {

};

/**
 * Rend les valeurs PWM para rapport à la phase spécifiée.
 * À appeler lorsque seule la phase est connue, c'est à dire lorsque le moteur
 * est à l'arret.
 * @param phase Phase, entre 0 et 5.
 * @param ccp Structure pour les valeurs PWM.
 */
void calculeAmplitudesArret(unsigned char phase, ccp_t *ccp) {

}

/**
 * Determine la phase en cours d'après les senseurs hall.
 * @param hall La valeur des senseurs hall: 0xb*****yzx
 * @return Le numéro de phase, entre 1 et 6.
 */
phase_t phaseSelonHall(unsigned char hall) {
    unsigned char c;
    phase_t phase;
    c = hall & 0x07;
    switch(c)
    {
        case 0b001:
            phase = PHASE_1;
            break;
        case 0b011:
            phase = PHASE_2;
            break;
        case 0b010:
            phase = PHASE_3;
            break;
        case 0b110:
            phase = PHASE_4;
            break;
        case 0b100:
            phase = PHASE_5;
            break;
        case 0b101:
            phase = PHASE_6;
            break;
        default:
            phase = PHASE_1;
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
phase_t phaseSelonHallEtDirection(unsigned char hall, direction_t direction) {
    phase_t phase;
    phase_t lastPhase = PHASE_ERROR;

    phase = phaseSelonHall(hall);
    
    // if last_phase == error then it is first time
    if (PHASE_ERROR == lastPhase)
    {
        lastPhase = phase;
    }
    else
    {
        if (AVANT == direction)
        {
            
        }
        else if (ARRIERE == direction)
        {

        }
        else
        {
            phase = PHASE_ERROR;
        }
    }

    return phase;
}

/**
 * Calcule l'angle correspondant à la phase et à la direction actuelle
 * de rotation.
 * @param phase Phase actuelle.
 * @param direction Direction actuelle.
 * @return L'angle correspondant.
 */
unsigned char angleSelonPhaseEtDirection(unsigned char phase, direction_t direction) {
    return 0;
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
    return 0;
}

/**
 * Etablit la puissance de départ.
 * @param p La puissance de départ.
 */
void etablitPuissance(unsigned char p) {

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

void machine(evenement_t evenement, unsigned char x, ccp_t *ccp) {

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