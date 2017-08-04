import json
import os

def get_name(old_name):
    pref = ["ll", "lt", "ht", "jets", "met", "small"]
    suff = ["eta", "phi", "pt", "mass"]
    pref_transl = ["Loose Lepton", "Tight Lepton", "Hadronic Transverse Momentum", "Jets", "Missing Transverse Energy", "Small", "Z"]
    suff_transl = [" Eta", " Phi", " Transverse Momentum", " Invariant Mass"]

    new_name = ""
    old_name = old_name.split("_")

    for wrd in old_name:
        if wrd in pref:
            new_name += (pref_transl[pref.index(wrd)])
        elif wrd in suff:
            new_name += (suff_transl[suff.index(wrd)])
    #Check to make sure that new_name doesn't start with a space
    if new_name[0] == " ":
        new_name = new_name[1:]

    return new_name

def mkjson():
    try:
        pdf_plots = os.listdir(os.getcwd() + "/static/pdfs")
        png_plots = os.listdir(os.getcwd() + "/static/pngs")
    except OSError:
        print("Directories not set up.")
        return; 

    if len(pdf_plots)==0 or len(png_plots)==0:
        return;

    new_json = {"pdfs":{}, "pngs":{}}

    for plt in pdf_plots:
        name = plt.split(".pdf")[0]
        new_json["pdfs"][get_name(name)] = "static/pdfs/"+plt
    for plt in png_plots:
        name = plt.split(".png")[0]
        new_json["pngs"][get_name(name)] = "static/pngs/"+plt

    with open("plots.json", "w") as fhout:
        json.dump(new_json, fhout, sort_keys=True, indent=4, separators=(',',':'))

if __name__ == "__main__":
    mkjson()
