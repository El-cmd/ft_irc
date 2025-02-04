# ft_irc

<h1 align="center">Serveur IRC aux petits oignons 🧅</h1>

## Description
Ce projet consiste en la création d'un serveur IRC (Internet Relay Chat) en C++98. Il implémente le protocole IRC selon la RFC 2812, permettant aux utilisateurs de communiquer en temps réel.

## Compilation et Exécution

### Prérequis
- Compilateur C++ compatible C++98
- Make

### Lancement du serveur
Le serveur peut être démarré de deux façons :

```bash
./ircserv <port> <password>
```
ou
```bash
./ircserv [host:port_network:password_network] <port> <password>
```

Le port par défaut est 6667 (6697 pour TLS).

## Spécifications Techniques

### Fonctionnalités Principales
- Authentification via commande `PASS`
- Gestion des utilisateurs (`NICK`, `USER`)
- Communication entre clients
- Support IPv6 (testable avec `irssi -6`)

### Points Techniques Importants
1. **Authentification**
   - `PASS` doit être envoyé avant tout autre paquet
   - L'enregistrement complet nécessite l'envoi des RPL 1 à 4

2. **Gestion des Messages**
   - Utilisation d'une classe `Message` pour gérer les communications
   - Implémentation des commandes via une map (commande -> fonction)
   - Buffering des sorties (un seul `send()` par `select()`)

3. **Sécurité et Stabilité**
   - Utilisation de `MSG_NOSIGNAL` comme 4ème argument pour `send()`
   - Implémentation des RPL et ERR en tant que macro
   - Gestion des commandes `CAP` (à ignorer sans erreur)


## Outils de Développement
Pour le débogage et les tests :
- Wireshark
- Proxy personnalisé
- Client IRC (ex: irssi)

## Références
- RFC 2812 (protocole client-serveur)
- [IRC God](https://ircgod.com/)
