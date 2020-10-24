CMD=bin/vad  #write here the name and path of your program

#optimizar alpha1
for alpha2 in 8.0 8.5 9.0 9.5; do
    echo "alpha2 = $alpha2"
    if [[ ! -x $CMD ]]; then
    echo "Error. Program not found (edit this script)"
    echo "CMD: $CMD"
    exit 1
    fi

    if [[ $# == 0 ]]; then
        echo $0 file1.wav [file2.wav ...]
        exit 1
    fi

    for filewav in $*; do
    #    echo
    #    echo "**************** $filewav ****************"
        if [[ ! -f $filewav ]]; then 
            echo "Wav file not found: $filewav" >&2
            exit 1
        fi

        filevad=${filewav/.wav/.vad}

        $CMD -i $filewav -o $filevad -2 $alpha2 || exit 1

    # Alternatively, uncomment to create output wave files
    #    filewavOut=${filewav/.wav/.vad.wav}
    #    $CMD $filewav $filevad $filewavOut || exit 1
    done
    scripts/vad_evaluation.pl db.v4/*/*lab | grep 'TOTAL'
done

exit 0