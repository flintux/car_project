#include <stdio.h>
#include <stdlib.h>

#define VRAI 1
#define FAUX 0

/* Saisir une valeur entiere */
int saisieEntier();


/************************************************************************
	
	Le programme demande le nb de tictac qu'à duré une phase puis
	calcul toutes valeures Y de 0 à 5 en fonction du nb de tictac
		0 = angle entre 00° à 10°
		1 = angle entre 10° à 20°
		2 = angle entre 20° à 30°
		3 = angle entre 30° à 40°
		4 = angle entre 40° à 50°
		5 = angle entre 50° à 60°

************************************************************************/
int main() {
	int cde, val, xi, xf, yi, yf, e;
	int dx;
	int dy;

	//initialiation de variables
	xi = 0;
	printf("duree de la derniere phase [en nb de TIC TAC]: ");
	xf = saisieEntier();
	yi = 0;
	yf = 12; // 6 = 60° en une phase
	dx = xf - xi;
	dy = yf - yi;
	dy *= 2;
	dx *= 2;
	e = dx;

	while (xi < xf)
	{
		xi++;
		e -= dy;
		if (e < 0)
		{
			yi++;
			e += dx*2;
		}
		printf("y = %d -> TICTAC No %d\n", yi, xi);//
	}
	system("Pause");

}

/* Saisie une valeur entiere au clavier */
int saisieEntier() {
	int n, val;
	char ligne[100];

	fgets(ligne, 100, stdin);
	n = sscanf(ligne, "%d", &val);
	while (n != 1) {
		printf("Entrer une valeur entiere : ");
		fgets(ligne, 100, stdin);
		n = sscanf(ligne, "%d", &val);
	}
	return (val);
}