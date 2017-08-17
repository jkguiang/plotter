<html>
    <head>
        <title>AutoPlotter</title>
        <!-- Latest compiled and minified CSS -->
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>

        <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
        <script src="//code.jquery.com/ui/1.11.4/jquery-ui.js"></script>

 
        <!-- My Code -->

        <!--PHP-->
        <?php

            $images = array();

            function newImage($new_png, $new_pdf) {
                global $images;
                $new_image = array(
                    "png_path" => $new_png,
                    "pdf_path" => $new_pdf,
                );

                $images[] = $new_image;
            
            }

            $cwd = getcwd();

            $png_path = ("/static/pngs/");
            $pdf_path = ("/static/pdfs/");

            $pngs = scandir($cwd . $png_path);
            $pdfs = scandir($cwd . $pdf_path);
            
            // Fill JSON
            for ($i = 0; $i < count($pngs); $i++) {
                newImage($png_path . $pngs[$i], $pdf_path . $pdfs[$i]);
            }

        ?>

        <!--JQuery-->
        <script type="text/javascript">

            var php_out = <?php echo json_encode($images); ?>;
            /* console.log(<?php print_r(dirname(__FILE__)); ?>); */

            $(function() {
                load_page(php_out);
            });

            function load_page(php_out) {
                var data = make_json(php_out);
                fill_sections(data);
            }

            function make_json(php_out) {
                var new_json = [];
                
                for (var i = 0; i < php_out.length; i++) {
                    var img_obj = php_out[i];
                    var png_path = img_obj["png_path"];
                    var pdf_path = img_obj["pdf_path"];
                    var name = "Image" + i;

                    new_json.push({
                        "name": name,
                        "png_path": png_path,
                        "pdf_path": pdf_path,
                    });
                }                
                
                return new_json;
            }

            function set_grid(data) {
                var container = $("#section_1");

                for (var i = 0; i < data.length; i+=3) {
                    var toappend = "";

                    //Draw thumbnails
                    toappend += "<div class='row'>";
                    toappend +=     ("<div id=grid_" + i + " class='col-lg-4'></div>");
                    toappend +=     ("<div id=grid_" + (i + 1) + " class='col-lg-4'></div>");
                    toappend +=     ("<div id=grid_" + (i + 2) + " class='col-lg-4'></div>");
                    toappend += "</div>";

                    container.append(toappend);
                }
            }

            function fill_grid(data) {

                for (var i = 0; i < data.length; i++) {
                    $("#grid_" + i).append("<a href="+data[i]["pdf_path"]+"><img src="+data[i]["png_path"]+" width=200 height=240></a>");
                    i++;
                }
            }

            function fill_sections(data) {
                set_grid(data);
                fill_grid(data);
            }

        </script>
        <!--<script src="plotter_main.js"></script>-->
    </head>

    <body>

        <!-- Navbar -->
        <nav class="navbar navbar-inverse navbar-fixed-top">
            <div class="container">
                <div class="navbar-header">
                    <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
                        <span class="sr-only">Toggle navigation</span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                    </button>
                    <a class="navbar-brand" href="#">AutoPlotter</a>
                </div>
                <div id="navbar" class="collapse navbar-collapse">
                    <form class="navbar-form navbar-right">
                        <div class="form-group">
                            <input type="text" id="search" placeholder="Search" class="form-control">
                        </div>
                        <button type="submit" class="btn btn-success">Search</button>
                    </form>
                </div><!--/.nav-collapse -->
            </div><!-- end navbar  -->

        </nav>

        <!-- Jumbotron -->
        <div class="jumbotron">
            <div class="container">
                <h1>AutoPlotter</h1>
                <p>Click on any of the thumbnails below to view the full pdf of your plot or click the button below to view the Github page for AutoPlotter.</p>
                <p><a class="btn btn-primary btn-lg" href="http://github.com/jkguiang/AutoPlotter" role="button">Github &raquo;</a></p>
            </div>
        </div>

        <!-- Images -->
        <div id="section_1" class="container">
        </div><!-- /.container -->

        <!-- Footer -->
        <div class="container">
            <hr> <!-- thin, grey horizontal line -->
            <footer>
                <p>Made by Jonathan Guiang 2017</p>
            </footer>
        </div>
    </body>
</html>
