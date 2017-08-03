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

os.chdir("/home/users/jguiang/public_html/zpeak/static");
plots = os.listdir(os.curdir)

new_json = {}

for plt in plots:
    name = plt.split(".png")[0]
    new_json[get_name(name)] = "static/"+plt

with open("/home/users/jguiang/public_html/zpeak/plots.json", "w") as fhout:
    json.dump(new_json, fhout, sort_keys=True, indent=4, separators=(',',':'))
