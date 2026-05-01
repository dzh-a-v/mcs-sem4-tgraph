latexmk -xelatex -outdir=out report.tex
if exist .\out\report.pdf (
    if exist .\report.pdf del .\report.pdf
    move .\out\report.pdf .\report.pdf
)