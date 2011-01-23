# Elfcrunch

I found this project in a CVS repo while cleaning out some old hard drives.  
At the time, I was teaching myself C and UNIX programming by building reverse engineering tools.  

I had a lot of fun with elfcrunch and learned a lot (apparently not how to use `static`, though).
A lot of the code and functionality was inspired by [elfsh](http://elfsh.asgardlabs.org/).
I figured I'd post it to github so people interested in ELF can skim it and so that I'd be constantly 
reminded of how dumb I've become over the years ;)

Elfcrunch is basically an `objdump` clone with some extra utilities that make
reverse engineering easier.  

`objdump` utility wouldn't print symbolic function names if the symbol table had been stripped; 
the displayed `call` operand was an address in the PLT (I think they've fixed this).  
After digging into the ELF spec, I realized that it's possible to find the symbol associated with a PLT address.
All of the graphs below were built from stripped binaries and their stdlib
functions are displayed by name.

`objdump` also crashed on obfuscated binaries (encrypted sections, non-standard entrypoints, etc...) and didn't
make pretty graphs.

## Gallery

 Here's some images generated with elfcrunch that I found laying around

##### Function call graph, showing a common util.

 ![callgraph](http://i.imgur.com/p0jQ1.png)

##### Assembly flow graph, showing symbolic function names where possible

 ![flowgraph](http://i.imgur.com/ECI0m.png)

##### Assembly of a key verification routine

 ![flowgraph](http://i.imgur.com/VYLU2.png)

