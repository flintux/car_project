#ifdef TEST

#include <htc.h>
#include <stdio.h>

/**
 * Fonction qui transmet un caract�re � la EUSART.
 * Il s'agit de l'impl�mentation d'une fonction syst�me qui est
 * appel�e par <code>printf</code>.
 * Si un terminal est connect� aux sorties RX / TX, il affichera du texte.
 * @param data Le code ascii du caract�re � afficher.
 */
void putch(char data) {
    while( ! TX1IF);
    TXREG1 = data;
}

/**
 * Configuration de la EUSART comme sortie asynchrone � 1200 bauds.
 * On assume que le PIC18 fonctionne � Fosc = 1MHz.
 */
void EUSART_Initialize()
{
    // Pour que la EUSART marche correctement sur Proteus, il
    // faut d�sactiver les convertisseurs A/D.
    ANSELA = 0x00;
    ANSELB = 0x00;
    ANSELC = 0x00;

    // Pour une fr�quence de 16MHz, ceci donne 19200 bauds:
    SPBRG1 = 12;
    SPBRGH1 = 0;

    // Configure RC6 et RC7 comme entr�es digitales, pour que
    // la EUSART puisse en prendre le contr�le:
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;

    // Configure la EUSART:
    RCSTA1bits.SPEN = 1;  // Active la EUSART.
    TXSTA1bits.SYNC = 0;  // Mode asynchrone.
    TXSTA1bits.TXEN = 1;  // Active l'�metteur.
}


/**
 * V�rifie si <param>value</param et <param>expectedValue</param> sont
 * identiques. Si elles ne le sont pas, affiche le test en erreur.
 * @param value Valeur obtenue.
 * @param expectedValue Valeur attendue.
 * @param testId Identifiant du test.
 */
unsigned char assertEqualsInt(int value, int expectedValue, const char *testId) {

    if (value != expectedValue) {
        printf("Test %s a echoue: attendu [%d], mais [%d]\r\n",
                testId, expectedValue, value);
        return 1;
    }
    return 0;
}

/**
 * V�rifie si <param>value</param> se trouve entre les deux
 * limites.
 * @param value Valeur obtenue.
 * @param min La valeur obtenue ne doit pas �tre inf�rieure � min.
 * @param max La valeur obtenue ne doit pas �tre sup�rieure � max.
 * @param testId Identifiant du test.
 */
unsigned char assertMinMaxInt(int value, int min, int max, const char *testId) {

    if ( (value < min) || (value > max) ) {
        printf("Test %s a echoue: attendu entre [%d] et [%d], mais [%d]\r\n",
                testId, min, max, value);
        return 1;
    }
    return 0;
}


/**
 * V�rifie si <param>value</param et <param>expectedValue</param> sont
 * identiques. Si elles ne le sont pas, affiche le test en erreur.
 * @param value Valeur obtenue.
 * @param expectedValue Valeur attendue.
 * @param testId Identifiant du test.
 */
unsigned char assertEqualsChar(char value, char expectedValue, const char *testId) {
    if (value != expectedValue) {
        printf("Test %s a echoue: attendu [%d], mais [%d]\r\n",
                testId, expectedValue, value);
        return 1;
    }
    return 0;
}

/**
 * V�rifie si <param>value</param n'est pas z�ro.
 * @param value Valeur obtenue.
 * @param testId Identifiant du test.
 */
unsigned char assertNotZeroChar(char value, const char *testId) {
    if (value == 0) {
        printf("Test %s a echoue: attendu [0], mais [%d]\r\n",
                testId, value);
        return 1;
    }
    return 0;
}

#endif