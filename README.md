# nana.cygwin

# Build
cygwin with clang & c++11(gcc4.9+), extras: readline, xorg, openssl, etc..
cd build/makefile; make

# example
nana.cpp:
```c++
#include <nana/gui/wvl.hpp> 
#include <nana/gui/widgets/label.hpp> 
int main() 
{ 
  nana::gui::form form; 
  nana::gui::label label(form, nana::rectangle(0, 0, 100, 20)); 
  label.caption(STR("Hello Nana")); 
  form.show(); 
  nana::gui::exec(); 
 } 
```
Makefile:
```Makefile
GCC	= g++
#PATH to nana home folder
NANAPATH =../nana
BIN	= nana
SOURCES = nana.cpp

NANAINC	= $(NANAPATH)/include
NANALIB = $(NANAPATH)/build/bin

INCS	= -I$(NANAINC)
LIBS	= -L$(NANALIB) -lnana -lX11 -lpthread -lrt -lXft -lpng -lcygwin -lgdi32

LINKOBJ	= $(SOURCES:.cpp=.o)

$(BIN): $(LINKOBJ) $(NANALIB)/libnana.a
	$(GCC) $(LINKOBJ) $(INCS) $(LIBS) -o $(BIN) -std=c++11

.cpp.o:
	$(GCC) -g -c $< -o $@ $(INCS) -std=c++11

$(NANALIB):
	make -f $(NANAPATH)/build/makefile/makefile

clean:
	rm -f $(LINKOBJ)
```
