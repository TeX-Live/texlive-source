#!/bin/bash
etex pdforigin.tex && etex pdforigin.tex
dvips pdforigin
ps2pdf pdforigin.ps && mv pdforigin.pdf pdforigin_etex_dvips.pdf
dvipdfmx pdforigin && mv pdforigin.pdf pdforigin_etex_dvipdfmx.pdf
eptex pdforigin.tex && eptex pdforigin.tex
dvipdfmx pdforigin && mv pdforigin.pdf pdforigin_eptex_dvipdfmx.pdf
pdftex pdforigin.tex && pdftex pdforigin.tex && mv pdforigin.pdf pdforigin_pdftex.pdf
xetex pdforigin.tex && xetex pdforigin.tex && mv pdforigin.pdf pdforigin_xetex.pdf

