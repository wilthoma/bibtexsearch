## What is BibTeXSearch ##

BibTeXSearch is a small tool to search BibTeX files in folders. It can be used as a lightweight alternative to conventional reference management systems.

Concretely:
  1. You point BibTeXSearch to some folder. It recursively searches the folder and all subfolders for BibTeX files. BibTeXSearch has a built in BibTeX parser, parses all .bib files found and displays a list of references found.
  1. You may filter the list. E.g., type "ko de", and BibTeXSearch will filter the list to references like "Kontsevich, M. and Soibelman, Y., Deformations of algebras over operads and Deligne's conjecture..."
  1. If you have found the entry you searched for, BibTeXSearch allows you to copy it to clipboard, open the .bib file at that location, or export the entry as \bibitem...

## Why use BibTeXSearch ##

  * Why not use a reference management system (RMS)? The main disadvantage is that you have to curate some database of references. If you have the discipline, do it. Otherwise read on...
  * Why not put all references into one single big .bib file? This is in principle a good solution, and you can surely use BibTeXSearch to search a single .bib file as well. For me the single .bib file solution also did not quite work out. I often forget which keys I assigned to papers. Also, in joint projects all collaborators have to share the necessary files, and at the end I often ended up with extra copies of .bib files. Sure, if you have the discipline to keep them in sync with your big .bib file, go ahead.
  * BibTeXSearch is very lightweight and operates on the data you already have, i.e., your .bib files.

## Screenshot ##

![https://bibtexsearch.googlecode.com/svn/wiki/scr_bibtexsearch.png](https://bibtexsearch.googlecode.com/svn/wiki/scr_bibtexsearch.png)