latexmk -xelatex -outdir=out report.tex
del .\report.pdf
move .\out\report.pdf .\report.pdf