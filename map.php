<?php
    require_once("session.php");
    require_once("class/user.php");
    require_once("class/GPS.php");
    $bateau = new GPS($BDD);
    $User = new user($BDD);
    $User->getuser($_SESSION['id']);

    if (!isset($_SESSION['id'])){
        header("Location: connexion.php");
    }

    if (isset($_POST['deconnexion'])){
        $User->deconnexion();
    }
?>

<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <!-- Nous chargeons les fichiers CDN de Leaflet. Le CSS AVANT le JS -->
        <link rel="stylesheet" href="https://unpkg.com/leaflet@1.3.1/dist/leaflet.css" integrity="sha512-Rksm5RenBEKSKFjgI3a41vrjkw4EVPlJ3+OiI65vTjIdo9brlAacEuKOiQ5OFh7cOI1bkDwLqdLw3Zg0cRJAAQ==" crossorigin="" />


        <link rel="canonical" href="https://getbootstrap.com/docs/5.1/examples/headers/">
        <!-- Bootstrap core CSS -->
        <link href="css/assets/dist/css/bootstrap.min.css" rel="stylesheet">
        <link href="css/headers.css" rel="stylesheet">
        <link href="css/map.css" rel="stylesheet">
        <title>Carte</title>
        <link rel="icon" href="image/logo_providence.png" />
    </head>
    <body>
        <main>
            <div class="container">
                <header class="d-flex flex-wrap align-items-center justify-content-center justify-content-md-between py-3 mb-4 border-bottom">
                    <a href="/" class="d-flex align-items-center col-md-3 mb-2 mb-md-0 text-dark text-decoration-none"></a>
                    <ul class="nav col-12 col-md-auto mb-2 justify-content-center mb-md-0">
                        <li><a href="map.php" class="nav-link px-2 link-dark">Map</a></li>
                        <?php
                            if ($User->getadmin() == 1){
                                ?>
                                    <li><a href="admin.php" class="nav-link px-2 link-secondary">Administrateur</a></li>
                                <?php
                            }
                        ?>
                    </ul>
                    <div class="col-md-3 text-end">
                        <form method="POST" action="">
                            <input type="submit" class="btn btn-primary" name="deconnexion" value="D??connexion">
                        </form>
                    </div>
                </header>
            </div>
        </main>
        <div class="centrer">
            <div id="map">
                <!-- Ici s'affichera la carte -->
            </div>

            <!-- Fichiers Javascript -->
            <script src="https://unpkg.com/leaflet@1.3.1/dist/leaflet.js" integrity="sha512-/Nsx9X4HebavoBvEBuyp3I7od5tA0UzAxs+j83KgC8PU0kgB4XiK4Lfe4y4cgBtaRJQEIFCW+oC506aPT2L1zw==" crossorigin=""></script>
            <script type='text/javascript' src='https://unpkg.com/leaflet.markercluster@1.3.0/dist/leaflet.markercluster.js'></script>
            <script type="text/javascript">
                // On initialise la latitude et la longitude de Paris (centre de la carte)
                var lat = 49.894067;
                var lon = 2.295753;
                var macarte = null;
                var markerClusters; // Servira ?? stocker les groupes de marqueurs
                // Nous initialisons une liste de marqueurs
                var villes ={
                    <?php
                        $request = $BDD->query("SELECT gps.latitude, gps.longitude, bateau.nom FROM bateau, gps WHERE gps.id_bateau = bateau.id");
                        while ($tab = $request->fetch()){
                    ?>
                        " Nom : <?= $tab['nom'];?>
                         Latitude : <?= $tab['latitude'];?>
                         Longitude : <?= $tab['longitude'];?>":{
                            "lat" : <?= $tab['latitude']?>,
                            "lon" : <?= $tab['longitude']?>,
                        },
                    <?php } ?>
                };
                // Fonction d'initialisation de la carte
                function initMap(){
                    // Nous d??finissons le dossier qui contiendra les marqueurs
                    var iconBase = 'image/';
                    // Cr??er l'objet "macarte" et l'ins??rer dans l'??l??ment HTML qui a l'ID "map"
                    macarte = L.map('map').setView([lat, lon], 7);
                    markerClusters = L.markerClusterGroup(); // Nous initialisons les groupes de marqueurs
                    // Leaflet ne r??cup??re pas les cartes (tiles) sur un serveur par d??faut. Nous devons lui pr??ciser o?? nous souhaitons les r??cup??rer. Ici, openstreetmap.fr
                    L.tileLayer('https://{s}.tile.openstreetmap.fr/osmfr/{z}/{x}/{y}.png',{
                        // Il est toujours bien de laisser le lien vers la source des donn??es
                        attribution: 'donn??es ?? <a href="//osm.org/copyright">OpenStreetMap</a>/ODbL - rendu <a href="//openstreetmap.fr">OSM France</a>',
                        minZoom: 1,
                        maxZoom: 20
                    }).addTo(macarte);
                    // Nous parcourons la liste des villes
                    for (ville in villes){
                        // Nous d??finissons l'ic??ne ?? utiliser pour le marqueur, sa taille affich??e (iconSize), sa position (iconAnchor) et le d??calage de son ancrage (popupAnchor)
                        var myIcon = L.icon({
                            iconUrl: iconBase + "marker_bateau.png",
                            iconSize: [70, 70],
                            iconAnchor: [25, 50],
                            popupAnchor: [-3, -76],
                        });
                        var marker = L.marker([villes[ville].lat, villes[ville].lon],{
                            icon: myIcon
                        });
                        marker.bindPopup(ville);
                        markerClusters.addLayer(marker); // Nous ajoutons le marqueur aux groupes
                    }
                    macarte.addLayer(markerClusters);
                }
                window.onload = function(){
                    // Fonction d'initialisation qui s'ex??cute lorsque le DOM est charg??
                    initMap();
                };
            </script>
        </div>
    </body>
</html>