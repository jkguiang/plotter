# Set up autoplotter in public_html if it exists

setup(){

    # $1 : public_html directory

    if [[ -e ${1} ]] ; then
        mkdir ${1}/plots
        mkdir ${1}/plots/pdfs
        mkdir ${1}/plots/pngs
        cp index.php ${1}/plots
        
    else
        echo "Public html directory not found, please log in to your UAF account to complete set up."
        exit 0

    fi
}

move(){

    # $1 : public_html directory, $2 : type, $3 : directory

    if [[ -e ${1}/plots/${2}s ]] ; then
        
        for file in ${3}/*.${2} ; do
            cp ${file} ${1}/plots/${2}s
        done

        chmod -R 755 ${1}/plots
        exit 0

    else
        echo "Please run the setup command first."
        exit 0
    fi
}

html=/home/users/${USER}/public_html

if [ "${1}" == "setup" ] ; then
    setup ${html}
    exit 0
    
elif [ "${1}" == "move" ] ; then

    # $2 : type, $3 : directory

    if [ "${2}" != "pdf" ] && [ "${2}" != "png" ] ; then
        echo "Invalid type: ${2}"
        exit 0 
    fi

    if ! [[ -e ${3} ]] ; then
        echo "Directory ${3} does not exist"
        exit 0
    fi

    move ${html} ${2} ${3}
    exit 0

else
    echo "Invalid command: ${1}"
    exit 0
fi
