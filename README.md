# PPII2 : Développement d'un système de recharge de véhicules électriques

## Description

Le projet **Développement d'un système de recharge de véhicules électriques** est une application codée en langage C qui permet de connaitre le chemin que doit emprunter un ou des véhicules de telle manière qui puisse le réaliser avec une certaine autonomie dépendante de chaque véhicule en un temps optimal. Il a été développé dans le cadre d'un Projet Pluridisciplinaire d’Informatique Intégrative permettant la validation de notre cursus de première année d'ingénieur.

## Fonctionnalités

- Calcul le chemin le plus court entre le départ et la destination tout en maintenant un seuil d'autonomie.
- Gère les files d'attente de chaque station.
- Recalcul le chemin à réaliser si les temps d'attente sont excessifs aux différentes stations.
- Affichage d'une carte des parcours des différentes voitures.
- Mode simulation avec un fichier input.txt en entrée pour lancer plusieurs véhicules.

## Installation

1. Clonez ce référentiel sur votre machine locale en utilisant la commande suivante :

```shell
git clone https://gitlab.telecomnancy.univ-lorraine.fr/ppii2_2223/ppii2-grp_30.git
```

2. Naviguez vers le répertoire du projet puis vers le dossier source :

```shell
cd nom-du-projet/src
```

3. Compilez le code source en utilisant un compilateur C et la commande make :

Pour compiler le simulateur :

```shell
make simu
```

Pour compiler la partie 1 :

```shell
make main
```

4. Exécutez le programme :

Pour exécuter le simulateur :

```shell
./simu
```

Pour exécuter la partie 1 :

```shell
./main 48.672894 6.1582773 50.3933812 3.6062753 "Tesla Model 3"
```

## Configuration

- Pour modifier les départs et arrivées, il suffit de modifier les coordonnées dans le fichier input.txt qui ressemble à ceci :
```txt
voiture_1 lattitude_départ longitude_départ
voiture_2 lattitude_arrivée longitude_arrivée
```
Chaque paire de lignes correspond à un véhicule.

Attention ! Les noms des véhicules ne doivent pas contenir d'espaces, tout espace est géré en tant que "_".

## Contributeurs

- [Lucas Laurent](https://gitlab.telecomnancy.univ-lorraine.fr/Lucas.Laurent)
- [Alexis Marcel](https://gitlab.telecomnancy.univ-lorraine.fr/Alexis.Marcel)
- [Noé Steiner](https://gitlab.telecomnancy.univ-lorraine.fr/Noe.Steiner)
- [Matthias Aurand-Augier](https://gitlab.telecomnancy.univ-lorraine.fr/Mathias.Aurand-Augier)

## License

[MIT License](https://opensource.org/licenses/MIT).