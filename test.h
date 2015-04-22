/* 
 * File:   test.h
 * Author: jmgonet
 *
 * Created on 31. janvier 2013, 06:56
 */

#ifndef TEST_H
#define	TEST_H

#ifdef TEST

/**
 * Configuration de la EUSART comme sortie asynchrone � 1200 bauds.
 * On assume que le PIC18 fonctionne � Fosc = 1MHz.
 */
void EUSART_Initialize();

/**
 * Affiche le code du test qui a echoue.
 * @param failedTestId L'identifiant du test qui a echou�.
 */
void displayError(unsigned int failedTestId);

/**
 * V�rifie si <param>value</param et <param>expectedValue</param> sont
 * identiques. Si elles ne le sont pas, affiche le test en erreur.
 * @param value Valeur obtenue.
 * @param expectedValue Valeur attendue.
 * @param testId Identifiant du test.
 */
unsigned char assertEqualsInt(int value,
        int expectedValue, const char *testId);

/**
 * V�rifie si <param>value</param> se trouve entre les deux
 * limites.
 * @param value Valeur obtenue.
 * @param min La valeur obtenue ne doit pas �tre inf�rieure � min.
 * @param max La valeur obtenue ne doit pas �tre sup�rieure � max.
 * @param testId Identifiant du test.
 */
unsigned char assertMinMaxInt(int value, int min, int max, const char *testId);

/**
 * V�rifie si <param>value</param et <param>expectedValue</param> sont
 * identiques. Si elles ne le sont pas, affiche le test en erreur.
 * @param value Valeur obtenue.
 * @param expectedValue Valeur attendue.
 * @param testId Identifiant du test.
 */
unsigned char assertEqualsChar(char value, 
        char expectedValue, const char *testId);

/**
 * V�rifie si <param>value</param n'est pas z�ro.
 * @param value Valeur obtenue.
 * @param testId Identifiant du test.
 */
unsigned char assertNotZeroChar(char value, const char *testId);

#endif
#endif	/* TEST_H */