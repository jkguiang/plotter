# $1={program select} $2={selected program options}
# $1=setup -> Check for required files, make directories, run {doAll, ooplt, mkjson}
# $1=plot -> run ooplt, check to see if any new pdf's made, if new pdfs => pdf_to_png()
# $1=scan -> run doAll

#Takes all .pdf's from pdf_dir and converts them to .png's at png_dir
pdf_to_png(){

    # $1=pdf_dir, $2=png_dir, $3=png_qual

    echo -n "Working: ["
    for file in ${1}/*.pdf ; do
        
        pdf_name=${file##*/}
        convert -density ${3} -fuzz 1% $file ${2}/${pdf_name%.pdf}.png
        echo -n "#"

    done

    echo -n "]"
    echo " "

}

# Check for required files, make directories, run {doAll, ooplt, mkjson}
setup(){

    # Target Directories
    main_dir=$1
    static_dir=${main_dir}/static
    pdf_dir=${main_dir}/static/pdfs
    png_dir=${main_dir}/static/pngs
    png_qual=$2

    # Check for required files
    if [[ -e CMS3.h && -e CMS3.cc ]] ; then

        # Check if directories already exist
        if ! [[ -e ${pdf_dir} || -e ${png_dir} ]] ; then
            echo 'Setting up directory tree...'
            mkdir ${main_dir}
            mkdir ${static_dir}
            mkdir ${pdf_dir} ${png_dir}

            # Copy files to proper directories and modify permissions
            cp index.php ${main_dir}
            chmod 755 ${main_dir}/index.php

            echo 'Scanning data...'
            root -b -q doAll.C
            echo 'Plotting...'
            root -b -q ooplt.C

            echo 'Updating web interface...'
            pdf_to_png ${pdf_dir} ${png_dir} ${png_qual} 
            chmod -R 755 ${static_dir}
            chmod -R 755 ${pdf_dir}
            chmod -R 755 ${png_dir}
            cd ${main_dir}
            echo 'Finished.'
            chmod 755 ${main_dir}
            exit 0
        
        else
            echo 'AutoPlotter already installed.'
            exit 0

        fi

    else
        echo 'CORE technology required'
        exit 1
    fi

}

# Run ooplt, check to see if any new pdf's made, if new pdfs => pdf_to_png()
plot(){

    # Target Directories
    pdf_dir=$1
    png_dir=$2
    png_qual=$3
    
    echo 'Plotting...'
    root -b -q ooplt.C

    echo 'Updating web interface...'
    rm -r ${png_dir}
    mkdir ${png_dir}
    pdf_to_png ${pdf_dir} ${png_dir} ${png_qual}
    chmod -R 755 ${png_dir}
    chmod -R 755 ${pdf_dir}
    echo 'Finished.'
    exit 0

    echo 'No new images created'
    exit 0

}

# Run doAll
scan(){
    echo 'Scanning data...'
    root -b -q doAll.C
    echo 'Finished.'
    exit 0
}

# Default
main_dir=/home/users/${USER}/public_html/AutoPlotter
pdf_dir=/home/users/${USER}/public_html/AutoPlotter/static/pdfs
png_dir=/home/users/${USER}/public_html/AutoPlotter/static/pngs
png_qual=50

if [ "$1" == "setup" ] ; then
    shift
    setup ${main_dir} ${png_qual}
    exit 0

elif [ "$1" == "plot" ] ; then
    shift
    plot ${pdf_dir} ${png_dir} ${png_qual}
    exit 0
elif [ "$1" == "scan" ] ; then
    shift
    scan
    exit 0
else
    echo 'Error: Invalid command'
    exit 1

fi
