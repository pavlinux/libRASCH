// This is the builder.sce 
// must be run from this directory 

ilib_name  = 'librasch_sci' 		// interface library name 

// objects files (but do not give mexfiles here)

files = [];

// other libs needed for linking (must be shared library names)

//libs  = [rasch] 				
libs  = [] 				

// table of (scilab_name,interface-name or mexfile-name, type) 

table =['ra_scilab','ra_scilab','cmex'];

ldflags = "-L../.. -lrasch";
cflags = "-I../../include";
fflags = "";

// do not modify below 
// ----------------------------------------------
ilib_mex_build(ilib_name,table,files,libs,'Makelib',ldflags,cflags,fflags)
