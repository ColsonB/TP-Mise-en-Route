<?php
    require_once("session.php");
    require_once("class/GPS.php");
    require_once("class/user.php");

    $bateau = new GPS($BDD);
    $admin = new user($BDD);
    $admin->getuser($_SESSION['id']);
    
    if (!isset($_SESSION['id'])){
        header("Location: connexion.php");
    }
    if ($admin->getadmin() == 0){
        header("Location: accueil.php");
    }
?>
<!DOCTYPE html>
<html lang="fr">
    <head>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <meta name="description" content="">
        <meta name="author" content="Mark Otto, Jacob Thornton, and Bootstrap contributors">
        <meta name="generator" content="Hugo 0.88.1">
        <link rel="icon" href="image/logo_providence.png" />
        <title>Ajout</title>
        <link rel="canonical" href="https://getbootstrap.com/docs/5.1/examples/sign-in/">
        <!-- Bootstrap core CSS -->
        <link href="css/bootstrap.min.css" rel="stylesheet">
        <link href="css/modifcordonnees" rel="stylesheet">
        <!-- Custom styles for this template -->
        <link href="css/signin.css" rel="stylesheet">
    </head>
    <body class="text-center">
        <main class="form-signin">
        <form method="POST">
                <img class="mb-4" src="image/logo_providence.png" alt="" width="72" height="57">
                <h1 class="h3 mb-3 fw-normal">Ajouter des coordonnées</h1>
                <div class="form-floating">
                    <input name="name" type="text" class="form-control" id="floatingInput" placeholder="nom" required>
                    <label for="floatingInput">Nom</label>
                </div>
                <div class="form-floating">
                    <input name="lat" type="text" class="form-control" id="floatingInput" placeholder="latitude" required>
                    <label for="floatingInput">Latitude</label>
                </div>
                <div class="form-floating">
                    <input name="long" type="text" class="form-control" id="floatingInput" placeholder="longitude" required>
                    <label for="floatingInput">Longitude</label>
                </div>
                <div class="form-floating">
                    <input name="hour" type="text" class="form-control" id="floatingPassword" placeholder="heure" required>
                    <label for="floatingPassword">Heure</label>
                </div>
                <div class="form-floating">
                    <input name="alt" type="text" class="form-control" id="floatingPassword" placeholder="altitude" required>
                    <label for="floatingPassword">Altitude</label>
                </div>
                <button class="w-100 btn btn-lg btn-primary" name="envoi" type="submit">Ajouter</button>
            </form>
            <?php
                if (isset($_POST['envoiajout'])){
                    $name = strip_tags($_POST['name']);
                    $lat = strip_tags($_POST['lat']);
                    $long = strip_tags($_POST['long']);
                    $hour = strip_tags($_POST['hour']);
                    $alt = strip_tags($_POST['alt']);
                    $erreur = $bateau->ajoutgps($name, $lat, $long, $hour, $alt);
                    echo "<p style=color:#FF0000><b>".$erreur."</b></p>";
                }else{
                    echo "<p>&nbsp;</p>";
                }
            ?>
            <input type="button" value="Annuler" class="w-100 btn btn-lg btn-primary" onClick="window.location.href='admin.php'" />
        </main>
    </body>
</html>