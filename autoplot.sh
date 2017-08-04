#Make target directory if it doesn't already exist, dump webpage files to target directory
setup(){
    
    # $1=target_dir
    
    mkdir ~/public_html/dump/$1

}

#Takes all .pdf's from pdf_dir and converts them to .png's at png_dir
pdf_to_png(){
    
    # $1=pdf_dir, $2=png_dir, $3=png_qual

    for file in ${1}/*.pdf ; do

        convert -density ${3} -fuzz 1% $file ${2}/${file##*/}.png
    
    done

}

if [[ $# == 0 ]] ; then
    echo 'Error: No arguments provided. Argument format: <pdf directory> <png directory> <png density>'
    exit 1
elif [[ $# == 1 ]] ; then
    pdf_dir=$1
    png_dir=$1
    png_qual=150
elif [[ $# == 2 ]] ; then
    pdf_dir=$1
    png_dir=$2
    png_qual=150
elif [[ $#==3 ]] ; then
    pdf_dir=$1
    png_dir=$2
    png_qual=$3
fi

pdf_to_png ${pdf_dir} ${png_dir} ${png_qual}
