/*
Auteur(s):
Date:

Ce module contient les implementations des fonctions d'analyse de traces GPS

*/

#define _CRT_SECURE_NO_WARNINGS

// Librairies standard
#include <stdio.h>
#include <math.h>

#include "config.h"
#include "matrac_lib.h"

int lire_donnees_gps(char nom_fichier[],
	                 double tab_latitudes[],
	                 double tab_longitudes[],
	                 double tab_altitudes[],
	                 int taille_max) {
	int nb_point = 0;						            // nombre de points trouves
	char ligne[LONGUEUR_MAX_LIGNE];			            // chaine de caractere contenant la balise
	FILE* fp;								            // pointeur au fichier a lire
	int no, an, mois, jour, heure, minute, seconde;		// informations lues du fichier
	double latitude, longitude, altitude;

	// ouverture du fichier et verification du succes de l'ouverture
	fp = fopen(nom_fichier, "r"); // en mode lecture seulement

	if (fp == NULL) {
		nb_point = -1;
	} else {
		// sauter l'en-tete
		if (fgets(ligne, LONGUEUR_MAX_LIGNE - 1, fp) == NULL) {
			nb_point = -1;

			// continuer la lecture si fichier n'est pas vide
		} else {

			// boucle de lecture principale de chaque point (un par ligne)
			while ((fgets(ligne, LONGUEUR_MAX_LIGNE - 1, fp) != NULL) && (nb_point < taille_max)) {
				// lecture d'un point et identification des 10 champs attendus:
				// No,Latitude,Longitude,Altitude,Date(a/m/j),Time(h:m:s)
				if (sscanf(ligne, "%d,%lf,%lf,%lf,%d/%d/%d,%d:%d:%d", &no,
				                                                      &latitude,
				                                                      &longitude,
				                                                      &altitude,
				                                                      &an,
				                                                      &mois,
				                                                      &jour,
				                                                      &heure,
				                                                      &minute,
				                                                      &seconde) == 10){

					//si decodage correct,mise en memoire de la latitude, longitude,altitude
					tab_latitudes[nb_point] = latitude;
					tab_longitudes[nb_point] = longitude;
					tab_altitudes[nb_point] = altitude;

					// incrementer le nombre de points correctement lus
					nb_point++;
				}
			}
		}
		// fermeture du fichier
		fclose(fp);
	}

	if (nb_point >= 0) {
		printf("\n...%d points lus\n", nb_point);
	}else {
		printf("\n...probleme de lecture du fichier\n");
	}
	//renvoi du nombre de points trouves
	return nb_point;

}

// implementations des fonctions de matrac_lib (c.f. prototype et doc dans matrac_lib.h)

double convertir_en_radian(double angle)
{
        double angle_radian;

        angle_radian = angle * PI / 180.0;

        return angle_radian;
}

double distance_entre_2_points(double latitude1,
                                   double longitude1,
                                   double latitude2,
                                   double longitude2,
                                   double altidute1,
                                   double altidute2)
{
        double distance_surface;
        double delta_latitude;
        double delta_longitude;
        double a;
        double c;
        double delta_altitude;
        double distance_totale_points;

        delta_latitude = convertir_en_radian(latitude2 - latitude1);
        delta_longitude = convertir_en_radian(longitude2 - longitude1);

        latitude1 = convertir_en_radian(latitude1);
        latitude2 = convertir_en_radian(latitude2);

        a = pow(sin(delta_latitude / 2.0), 2.0) + cos(latitude1) * cos(latitude2) * pow(sin(delta_longitude / 2.0), 2.0);
        c = 2.0 * asin(sqrt(a));
        distance_surface = RAYON_TERRE * c;

        delta_altitude = (altidute2 - altidute1) / 1000.0;
        distance_totale_points = sqrt(pow(distance_surface, 2.0) + pow(delta_altitude, 2.0));

        return distance_totale_points;
}

void afficher_tableau(double tableau[], int nb_valeurs)
{
        int indice;

        indice = 0;

        if (nb_valeurs > 0) {
                while (indice < nb_valeurs) {
                        printf("%.2f\n", tableau[indice]);
                        indice = indice + 1;
                }
        }
}

int charger(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille_max)
{
        char nom_fichier[LONGUEUR_MAX_NOM_FICHIER];
        int nb_points;

        printf("Nom du fichier : ");
        fflush(stdout);
        scanf("%s", nom_fichier);

        nb_points = lire_donnees_gps(nom_fichier, tab_latitudes, tab_longitudes, tab_altitudes, taille_max);

        return nb_points;
}

void afficher(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille)
{
        int indice;

        indice = 0;

        printf("index \t latitude \t longitude \t altitude\n");

        if (taille > 0) {
                while (indice < taille) {
                        printf("%d \t %lf \t %lf \t %lf\n", indice, tab_latitudes[indice], tab_longitudes[indice], tab_altitudes[indice]);
                        indice = indice + 1;
                }
        }
}

double distance_totale(double tab_latitudes[],
                           double tab_longitudes[],
                           double tab_altitudes[],
                           int taille,
                           int debut,
                           int fin)
{
        double distance;
        int indice;

        distance = 0.0;
        indice = debut;

        if ((debut >= 0) && (fin < taille) && (debut < fin)) {
                while (indice < fin) {
                        distance = distance + distance_entre_2_points(tab_latitudes[indice], tab_longitudes[indice], tab_latitudes[indice + 1], tab_longitudes[indice + 1], tab_altitudes[indice], tab_altitudes[indice + 1]);
                        indice = indice + 1;
                }
        }

        return distance;
}

double ascension(double tab_altitudes[], int taille)
{
        double ascension_totale;
        int indice;

        ascension_totale = 0.0;
        indice = 0;

        if (taille > 0) {
                while (indice < taille - 1) {
                        if (tab_altitudes[indice + 1] > tab_altitudes[indice]) {
                                ascension_totale = ascension_totale + (tab_altitudes[indice + 1] - tab_altitudes[indice]);
                        }
                        indice = indice + 1;
                }
        }

        return ascension_totale;
}

int minimum_tableau(double tableau[], int taille, double* minimum)
{
        int valide;
        int indice;

        valide = 0;
        indice = 0;

        if (taille > 0) {
                *minimum = tableau[0];
                while (indice < taille) {
                        if (tableau[indice] < *minimum) {
                                *minimum = tableau[indice];
                        }
                        indice = indice + 1;
                }
                valide = 1;
        } else {
                *minimum = 0.0;
        }

        return valide;
}

int maximum_tableau(double tableau[], int taille, double* maximum)
{
        int valide;
        int indice;

        valide = 0;
        indice = 0;

        if (taille > 0) {
                *maximum = tableau[0];
                while (indice < taille) {
                        if (tableau[indice] > *maximum) {
                                *maximum = tableau[indice];
                        }
                        indice = indice + 1;
                }
                valide = 1;
        } else {
                *maximum = 0.0;
        }

        return valide;
}

double pente_maximale(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille, int taille_fenetre)
{
        double pente_max;
        int indice;
        int debut_fenetre;
        int fin_fenetre;
        double delta_altitude;
        double distance_fenetre;
        double pente_courante;

        pente_max = 0.0;
        indice = taille_fenetre;

        if ((taille_fenetre > 0) && (taille_fenetre < taille)) {
                while (indice < (taille - taille_fenetre)) {
                        debut_fenetre = indice - taille_fenetre;
                        fin_fenetre = indice + taille_fenetre;
                        delta_altitude = tab_altitudes[fin_fenetre] - tab_altitudes[debut_fenetre];
                        distance_fenetre = distance_totale(tab_latitudes, tab_longitudes, tab_altitudes, taille, debut_fenetre, fin_fenetre);

                        if (distance_fenetre == 0.0) {
                                pente_courante = 0.0;
                        } else {
                                pente_courante = fabs(delta_altitude / (distance_fenetre * 1000.0)) * 100.0;
                        }

                        if (pente_courante > pente_max) {
                                pente_max = pente_courante;
                        }

                        indice = indice + 1;
                }
        }

        return pente_max;
}

void analyser(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille)
{
        double altitude_min;
        double altitude_max;
        double ascension_totale;
        double distance_trace;

        minimum_tableau(tab_altitudes, taille, &altitude_min);
        maximum_tableau(tab_altitudes, taille, &altitude_max);
        ascension_totale = ascension(tab_altitudes, taille);
        distance_trace = distance_totale(tab_latitudes, tab_longitudes, tab_altitudes, taille, 0, taille - 1);

        printf("Altitude minimale : %lf\n", altitude_min);
        printf("Altitude maximale : %lf\n", altitude_max);
        printf("Ascension totale : %lf\n", ascension_totale);
        printf("Distance totale : %lf km\n", distance_trace);
}

void calculer(double tab_latitudes[], double tab_longitudes[], double tab_altitudes[], int taille)
{
        int taille_fenetre;
        double pente;

        printf("Taille de la fenetre : ");
        fflush(stdout);
        scanf("%d", &taille_fenetre);

        pente = pente_maximale(tab_latitudes, tab_longitudes, tab_altitudes, taille, taille_fenetre);

        printf("Pente maximale : %lf %%\n", pente);
}

