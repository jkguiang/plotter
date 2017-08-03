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
    $.getJSON(jsonPath, function(data){
        set_sections(data);
        fill_sections(data);
    });
}

function set_sections(data){
    var container = $("#section_1");
    var i = 0;
    for (var key in data){
        var toappend = "";
        if (data.hasOwnProperty(key)){
            toappend += "<br>";
            //Link to toggle hidden of details_i
            toappend += ("<a href='#/' class='thick' onClick=\"$('#details_"+i+"').slideToggle(100)\">"+key+"</a>");
            toappend += "<div class='pbar' id='pbar_"+i+"'>";
            toappend +=      "<span id='pbartextleft_"+i+"' class='pbartextleft'></span>";
            toappend +=      "<span id='pbartextright_"+i+"' class='pbartextright'></span>";
            toappend += "</div>";
            toappend += "<div id='details_"+i+"' style='display:none;'></div>";

            container.append(toappend);
            i++;
        }
    }
}

function get_plots(data){
    var i = 0;
    for (var key in data){
        $("#details_"+i).append("<p id='plots_"+i+"'></p>");
        toappend = "";
        if (data.hasOwnProperty(key)){
            toappend+="<img src="+data[key]+" width=1200 height=900>";
            $("#plots_"+i).html(toappend);
            i++;
        }
    }
}

function fill_sections(data){
    get_plots(data);
}

function expandAll() {
    // do it this way because one guy may be reversed
    if(detailsVisible) {
        $("#toggle_all").text("show details");
        $("[id^=details_]").slideUp(100);
    } else {
        $("#toggle_all").text("hide details");
        $("[id^=details_]").slideDown(100);
    }
    detailsVisible = !detailsVisible;
}
