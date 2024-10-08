# ft_irc
<h1 align="center"> Serveur IRC aux petits oignons</h1>

<h2>Quelques etapes: </h2>


- **Créez une classe `Message` avec expéditeur, destinataire, charge utile, commande appelée, etc. Cela sera utile.**
- **`PASS` doit être envoyé avant tout autre paquet. Criez un message seulement si l'utilisateur est enregistré (le nickname et le mot de passe ont été complétés avec succès) et refusez la connexion à ce moment (vous pouvez le faire directement sur un mauvais `PASS` si vous le souhaitez).**

- **`USER` peut être appelé avant `NICK`.**

- **Pour compléter un enregistrement, vous devez envoyer RPL 1 à 4.**

- **Si un client envoie une commande `CAP`, ignorez-la, ne renvoyez pas d'erreur.**
- **Pour déterminer quelle commande appeler, utilisez une map avec une chaîne comme clé (nom de la commande) et un pointeur vers une fonction (c'est cool).**
- **Pour tester ipv6, vous pouvez utiliser `irssi` et ajouter `-6` pendant le `/connect`.**
- **Le port par défaut IRC est 6667 (6697 pour TLS). Lors de la soumission de votre travail, veuillez les utiliser, c'est ennuyeux de devoir les spécifier lors des tests en défense (par exemple, spécifier 6667 à chaque fois avec `irssi` est ennuyeux).**
- **Si vous avez le temps, faites `REHASH`, c'est vraiment cool de le voir fonctionner.**
- **Si vous avez le temps et que vous avez bien commencé le projet, faites `STATS`.**
- **RFC 1459 est obsolète, utilisez 2812, 2813 est pour multiserveur.**

- **Le sujet est changé, vous devriez démarrer votre exécutable ainsi :**
**`./ircserv <port> <password>`**
**mais gérez également l'autre forme :**
**`./ircserv [host:port_network:password_network] <port> <password>`**

**sinon certaines personnes peuvent se fâcher sans raison.**

- **Tamponnez chaque sortie, vous n'êtes autorisé à faire qu'un seul `send()` par `select()`.**

- **Ajoutez `MSG_NOSIGNAL` comme 4ème argument pour `send`, cela empêchera votre programme de planter sous certaines conditions.**

- **Implémentez RPL au fur et à mesure, vous n'aurez pas besoin de tous.**

- **Implémentez RPL et ERR en tant que macro.**

- **Vous pouvez coder en dur l'opérateur.**

- **Utilisez Wireshark / un proxy personnalisé etc… pour inspecter la communication entre votre serveur de référence (ou votre serveur) et votre client.**


- **Source: `https://ircgod.com/`**




