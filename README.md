libRASCH Version 0.8.31
======================

Topics handled here:
* General
* License
* Documentation

For installation instructions, see file INSTALL.

LIBRASCH_HOME is the top most directory of the libRASCH sourcecode
directory.

-----------------------------------------------------------------------

General
-------

libRASCH is a library for accessing signals/recordings saved
in several file-formats. libRASCH provides a common interface to
the signals, regardless of the used format. Additionally it
provides support to process (e.g. detect beats in an ecg or calculate
heart rate variability parameters) and view the signals. At the
moment the main focus of libRASCH is to handle biological signals
(e.g. ECG, EEG, blood-pressure) but the design of the library 
allows to handle any kind of signal.

The library is available for
- Linux and
- Windows

libRASCH is written in C and provides interfaces for
- Perl
- Python
- Octave for Linux
- Matlab
- Scilab (at the moment only for Linux)

More information can be found at the libRASCH-website at
                     www.librasch.org

-----------------------------------------------------------------------

License
-------

libRASCH is published under the GNU Lesser General Public License
Version 2.1.

Note that plugins for libRASCH _don't_ fall under the category of
'derivative work'. It is allowed to publish plugins under a license
other than the LGPL as long as other plugins and libRASCH is not
affected. 

Also note that the only valid version of the LGPL as far as libRASCH
is concerned is version 2.1 of the LGPL (from February 1999), not
any later versions, unless explicitly otherwise stated.

The distribution of libRASCH on www.librasch.org will only contain
plugins, which use a free-software or open-source license or where
the source code is available.

-----------------------------------------------------------------------

DOCUMENTATION
-------------

The HTML version of the user manual can be found in the directory
'LIBRASCH_HOME/docs/manual'. The file 'book1.html' is the starting
point. Additionally the HTML version as one page can be found at
'LIBRASCH_HOME/docs/librasch_manual_one_page.html'.

The manual uses DocBook. The source of the manual can be found in
'LIBRASCH_HOME/docs/manual_src'.

On Windows, a link to the HTML one page version in the Startmenu entry
for libRASCH is added.
