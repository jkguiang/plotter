var jsonPath = "plots.json";
var refreshSecs = 10*60;

$(function() {
    load_json(jsonPath);
    setInterval(load_json, refreshSecs*1000);
});

$.ajaxSetup({
   type: 'POST',
   timeout: 15000,
});

function load_json(jsonPath){
    jQuery.getJSON(jsonPath, function(data){
        set_grid(data);
        fill_sections(data);
    });
}

function set_grid(data){
    var container = $("#section_1");
    var i = 0;
    var len = Object.keys(data["pngs"]).length;

    for (var key in data["pngs"]){
        var toappend = "";
        if (data["pngs"].hasOwnProperty(key)){
            
            if (i > len){
                return;
            }

            //Draw thumbnails
            toappend += "<div class='row'>";
            toappend +=     ("<div id=grid_" + i + " class='col-lg-4'></div>");
            toappend +=     ("<div id=grid_" + (i + 1) + " class='col-lg-4'></div>");
            toappend +=     ("<div id=grid_" + (i + 2) + " class='col-lg-4'></div>");
            toappend += "</div>";

            container.append(toappend);
            i+=3;
        }
    }
}

function fill_grid(data){
    var i = 0;
    for (var key in data["pngs"]){
        if (data["pngs"].hasOwnProperty(key)){
            $("#grid_" + i).append("<a href="+data["pdfs"][key]+"><img src="+data["pngs"][key]+" width=200 height=240></a>")
            i++;
        }
    }
}

function fill_sections(data){
    set_grid(data);
    fill_grid(data);
}
