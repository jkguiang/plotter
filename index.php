<html>
    <head>
        <title>AutoPlotter</title>
        <!-- Latest compiled and minified CSS -->
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>

        <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
        <script src="//code.jquery.com/ui/1.11.4/jquery-ui.js"></script>

 
        <!-- My Code -->
        
        <!-- CSS -->
        <style>
            img:hover {
                border: 1px solid #cecece;
            }

            .jumbotron-billboard .main {
                margin-bottom: 0px;
                opacity: 0.2;
                color: #fff;
                background-image: #fff url("") center center;
                background-color: #ffffff !important;
                background-size: 20% !important;

                -webkit-filter: blur(5px);
                -moz-filter: blur(5px);
                -o-filter: blur(5px);
                -ms-filter: blur(5px);
                filter: blur(5px);

                overflow: hidden;
                width: 100%;
                height: 100%;

                position:absolute;
                top:0;left:0;
                z-index:1;
            }
            .jumbotron {position:relative;padding:50px;overflow:hidden}
            .jumbotron .container {z-index:2;
                position:relative;
                z-index:2;
            }
        </style>

        <!-- PHP -->
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

            $png_path = ("static/pngs/");
            $pdf_path = ("static/pdfs/");

            $pngs = scandir($cwd . "/" . $png_path);
            $pdfs = scandir($cwd . "/" . $pdf_path);
            
            // Fill JSON
            for ($i = 0; $i < count($pngs); $i++) {
                if ($pngs[$i] == '.' || $pngs[$i] == '..') continue;
                newImage($png_path . $pngs[$i], $pdf_path . $pdfs[$i]);
            }

        ?>

        <!-- JQuery -->
        <script type="text/javascript">

            var php_out = <?php echo json_encode($images); ?>;

            $(function() {
                load_page(php_out);
                $('[id^=img_]').mouseenter(
                    function() {
                        $("#jumbo").css("background","#000 url('"+$(this).attr("src")+"') center center");
                    } 
                );
                console.log($('[id^=img_]'));
            });

            function load_page(php_out) {
                var data = make_json(php_out);
                filter(data);
                fill_sections(data);
            }

            function make_json(php_out) {
                var new_json = [];
                
                for (var i = 0; i < php_out.length; i++) {
                    var img_obj = php_out[i];
                    var png_path = img_obj["png_path"];
                    var pdf_path = img_obj["pdf_path"];
                    var name = png_path.split('/').reverse()[0].split('.')[0];

                    new_json.push({
                        "name": name,
                        "png_path": png_path,
                        "pdf_path": pdf_path,
                        "hidden": false,
                    });
                }                
                
                return new_json;
            }

            function refresh() {
                load_page(php_out);
            }

            function filter(data) {

                var input = document.getElementById('search');
                var search = input.value.toLowerCase();

                for (var i = 0; i < data.length; i++) {
                    if (data[i]["name"].toLowerCase().indexOf(search) < 0) {
                        console.log("hid "+data[i]["name"])
                        data[i]["hidden"] = true;
                    }
                }

                return data;
            }

            function set_grid(data) {
                var container = $("#section_1");
                var counter = 0;

                container.html("");

                for (var i = 0; i < data.length; i+=3) {
                    var toappend = "";

                    //Draw thumbnails
                    toappend += "<div class='row'>";
                    toappend += "   <div class='text-center'>"
                    for (var j = 0; j < 3; j++){
                        if (counter > data.length) return;
                        toappend +=     ("<div id=grid_" + (i + j) + " class='col-lg-4'></div>");
                        counter++;
                    }
                    toappend += "   </div>"
                    toappend += "</div>";
                    container.append(toappend);
                }
            }

            function fill_grid(data) {

                var counter = 0;

                for (var i = 0; i < data.length; i++) {
                    if (data[i]["hidden"]) {
                        continue;
                    }
                    $("#grid_" + counter).append("<h4>"+data[i]["name"]+"</h4><a href="+data[i]["pdf_path"]+"><img id=img_"+counter+" src="+data[i]["png_path"]+" width=200 height=240></a>");
                    $("#grid_" + counter).append("");
                    counter++;
                }
            }

            function fill_sections(data) {
                set_grid(data);
                fill_grid(data);
            }

        </script>
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
                            <input type="text" id="search" onkeyup="refresh()" placeholder="Search" class="form-control">
                        </div>
                        <button type="submit" id="search_button" class="btn btn-success">Search</button>
                    </form>
                </div><!--/.nav-collapse -->
            </div><!-- end navbar  -->

        </nav>

        <!-- Jumbotron -->
        <div class="jumbotron jumbotron-billboard">
            <div id="jumbo" class="main"></div>
            <p><br /><p>
            <div class="container">
                <h1>AutoPlotter</h1>
                <p>Click on any of the thumbnails below to view the full pdf of your plot or click the button below to view the source code for AutoPlotter.</p>
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
