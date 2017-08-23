<html>
    <head>
        <title>AutoPlotter</title>

        <!-- Latest compiled and minified Boostrap CSS and Javascript -->
        <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css">
        <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js"></script>

        <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
        <script src="//code.jquery.com/ui/1.11.4/jquery-ui.js"></script>

        <!-- Slider -->
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-slider/9.8.1/css/bootstrap-slider.min.css">
        <script src="https://cdnjs.cloudflare.com/ajax/libs/bootstrap-slider/9.8.1/bootstrap-slider.min.js"></script>
 
        <!-- My Code -->
        
        <!-- CSS -->
        <style>
            body {
                padding-top: 40px;
            }

            img:hover {
                border: 1px solid #cecece;
            }
            
            .well-image .bg {
                pointer-events: none;
                margin-bottom: 0px;
                opacity: 0.2;
                color: #fff;
                background: #fff url("") no-repeat center center;
                background-color: #ffffff !important;
                background-size: contain !important;

                -webkit-filter: blur(5px);
                -moz-filter: blur(5px);
                -o-filter: blur(5px);
                -ms-filter: blur(5px);
                filter: blur(5px);

                overflow: hidden;
                width: 115%;
                height: 100%;

                position: absolute;
                top:0;left:0;
                z-index:1;
            }
            
            
            .well {
                overflow: hidden;
                width: 115%;
            }

        </style>

        <!-- PHP -->
        <?php

            $images = array();

            function newImage($new_png, $new_pdf, $new_width, $new_height) {
                global $images;
                $new_image = array(
                    "png_path" => $new_png,
                    "pdf_path" => $new_pdf,
                    "width" => $new_width,
                    "height" => $new_height,
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
                list($width, $height) = getimagesize($cwd . "/" . $png_path . "/" . $pngs[$i]);
                newImage($png_path . $pngs[$i], $pdf_path . $pdfs[$i], $width, $height);
            }

        ?>

        <!-- JQuery -->
        <script type="text/javascript">

            var php_out = <?php echo json_encode($images); ?>;
            var initMap = {"thumbnails":{"width": 0, "height": 0}};

            $(function() {
                load_page(php_out);
            });

            function load_page(php_out) {
                var data = make_json(php_out);
                filter(data);
                fill_sections(data);
                $('[id^=img_]').mouseenter(
                    function() {
                        $("#well").css("background","#000 url('"+$(this).attr("src")+"') no-repeat center center");
                    } 
                );

                $("#slider").bootstrapSlider({
                        ticks: [0, 100, 200],
                        ticks_labels: ['0%', '100%', '200%'],
                        ticks_snap_bounds: 5,
                        tooltip_position: 'bottom'
                });
                $("#slider").on("slide", function(slideEvt) {
                    $("img").attr("width", initMap["thumbnails"]["width"]*(slideEvt.value/100));
                    $("img").attr("height", initMap["thumbnails"]["height"]*(slideEvt.value/100));
                });

                // Ensure that images are drawn with slideEvt value when page is loaded (so if page is refreshed by search funct, maintain size)
                var val = $("#slider").attr("value");
                $("img").attr("width", initMap["thumbnails"]["width"]*(val/100));
                $("img").attr("height", initMap["thumbnails"]["height"]*(val/100));

            }

            function make_json(php_out) {
                var new_json = [];
                
                for (var i = 0; i < php_out.length; i++) {
                    var img_obj = php_out[i];
                    var png_path = img_obj["png_path"];
                    var pdf_path = img_obj["pdf_path"];
                    var width = img_obj["width"];
                    var height = img_obj["height"];
                    var name = png_path.split('/').reverse()[0].split('.')[0];

                    // Get divisor for image dimensions
                    var div = get_div(Math.max(width, height));
                    initMap["thumbnails"]["width"] = width/div;
                    initMap["thumbnails"]["height"] = height/div;

                    new_json.push({
                        "name": name,
                        "png_path": png_path,
                        "pdf_path": pdf_path,
                        "width": initMap["thumbnails"]["width"],
                        "height": initMap["thumbnails"]["height"],
                        "hidden": false,
                    });
                }                
                
                return new_json;
            }

            function get_div(max_length) {
                var divisor = 1;
                while (true) {
                    if (max_length/divisor <= 250) {
                        return divisor;
                    }
                    divisor+=0.5;
                }
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

                    $("#grid_" + counter).append("<h4>"+data[i]["name"]+"</h4><a href="+data[i]["pdf_path"]+"><img id=img_"+counter+" src="+data[i]["png_path"]+
                        " width="+data[i]["width"]+" height="+data[i]["height"]+"></a>");
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
<!--        <nav class="navbar navbar-inverse navbar-fixed-top">
            <div class="container">
                <div class="navbar-header">
                    <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#navbar" aria-expanded="false" aria-controls="navbar">
                        <span class="sr-only">Toggle navigation</span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                        <span class="icon-bar"></span>
                    </button>
                </div>
                <div id="navbar" class="collapse navbar-collapse">
                    <form class="navbar-form navbar-left">
                        <div class="form-group">
                        </div>
                    </form>
                    <form class="navbar-form navbar-right">
                        <div class="form-group">
                        </div>
                    </form>
                </div>
            </div>

        </nav> -->

<!--        
        <div class="jumbotron jumbotron-billboard">
            <div id="jumbo" class="main"></div>
            <p><br /><p>
            <div class="container">
                <h1>AutoPlotter</h1>
                <p>Click on any of the thumbnails below to view the full pdf of your plot or click the button below to view the source code for AutoPlotter.</p>
                <p><a class="btn btn-primary btn-lg" href="http://github.com/jkguiang/AutoPlotter" role="button">Github &raquo;</a></p>
            </div>
        </div> -->

        <div class="container-fluid">
            <div class="row">
                <!-- Side Navbar -->
                <div class="col-md-3">
                    <div class="sidebar-nav-fixed sidebar-nav-fixed affix">
                        <div class="well well-image">
                            <div id="well" class="bg"></div>
                            <h4>AutoPlotter</h4>
                            <div class="text-center">
                                <form>
                                    <div class="form-group">
                                        <input type="text" id="search" onkeyup="refresh()" placeholder="Search" class="form-control">
                                    </div>
                                    <div class="form-group">
                                        <input id="slider" type="text" data-slider-ticks="[0, 100, 200]" data-slider-ticks-snap-bounds="5" data-slider-ticks-labels='["0%", "100%", "200%"]' data-slider-value="100"/>
                                    </div>
                                </form>
                                <a class="btn btn-primary btn-sm" href="http://github.com/jkguiang/AutoPlotter" role="button">Github &raquo;</a>
                            </div>
                        </div>
                    </div>
                </div>

                <div class="col-sm-9 col-sm-offset-3 col-md-10 col-md-offset-2 main">
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
                </div>
            </div>
        </div>
    </body>
</html>
