# generate C source to test posix options from POSIX option macro list.
BEGIN { cnt = 0 }
# get posix options from file.
{ elem[cnt++] = $1; }
# end; print them; generate C source code.
END {
	for (i=0; i<=cnt; i++)
	{
		if (i == 0) {
			print "#include <unistd.h>\n#include <stdio.h>\nint main()\n{\n"
		} else if (i == cnt) {
			print "\treturn 0;\n}"
		} else {
			printf("#if %s > 0L\n\tprintf(\"[O] %s\\n\");\n", elem[i], elem[i]);
			printf("#else\n\tprintf(\"[X] %s\\n\");\n", elem[i], elem[i]);
			printf("#endif\n");
		}
	}
}
