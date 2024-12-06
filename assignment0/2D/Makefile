sources = \
	main_3d.cpp \
	mesh.cpp \
	util.cpp \
	imgui/imgui_draw.cpp \
	imgui/imgui_impl_glfw.cpp \
	imgui/imgui_impl_opengl3.cpp \
	imgui/imgui_tables.cpp \
	imgui/imgui_widgets.cpp \
	imgui/imgui.cpp \
	gl_core_3_3.c
libs = \
	-lGL \
	-lglut \
	-ldl \
	-lglfw
outname = assignment0

all:
	g++ -std=c++11 $(sources) $(libs) -o $(outname)
clean:
	rm $(outname)
