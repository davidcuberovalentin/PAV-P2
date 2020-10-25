CMD=bin/vad  #write here the name and path of your program

#optimizar N_init
for MAYBE_VOICE_MAX in 9 10 11 12 13; do
    echo "Maybe Voice Max = $MAYBE_VOICE_MAX"
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

        $CMD -i $filewav -o $filevad --Maybevoicemax=$MAYBE_VOICE_MAX || exit 1

    # Alternatively, uncomment to create output wave files
    #    filewavOut=${filewav/.wav/.vad.wav}
    #    $CMD $filewav $filevad $filewavOut || exit 1
    done
    scripts/vad_evaluation.pl db.v4/*/*lab | grep 'TOTAL'
done

exit 0