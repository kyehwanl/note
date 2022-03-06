# call with dump_breaks file.txt
define dump_breaks
    set logging file $arg0
    set logging redirect on
    set logging on
    info breakpoints
    set logging off
    set logging redirect off
end
