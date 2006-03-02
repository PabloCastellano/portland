noinst_PROGRAMS = gen
gen_SOURCES = gen.cpp
gen_LDADD = $(LIB_QT)
gen_LDFLAGS = $(all_libraries)
INCLUDES = $(all_includes)
CLEANFILES = qtkde_functions.cpp module_functions.cpp module_functions.h generated

generate: generated

generated: gen gen.txt
	./gen || exit 1
	cat qt/in/qtkdeintegration_x11_0.cpp qt/in/qtkdeintegration_x11_1.cpp qtkdeintegration_x11.cpp.gen qt/in/qtkdeintegration_x11_2.cpp >qt/qtkdeintegration_x11.cpp
	cat qt/in/qtkdeintegration_x11_p_1.h qtkdeintegration_x11_p.h.gen qt/in/qtkdeintegration_x11_p_2.h >qt/qtkdeintegration_x11_p.h
	rm -f qtkdeintegration_x11.cpp.gen qtkdeintegration_x11_p.h.gen
	cp -f qt/in/qt.patch qt/
	touch generated

qtkde_functions.cpp module_functions.cpp module_functions.h : generated
