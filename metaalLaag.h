#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

///Maakt een CAMetalLayer op het inhouds-zicht van het GLFW venster en geeft de laag terug
///zodat wgpu er een oppervlak van kan maken (alleen op macOS zinvol)
void * maakMetaalLaag(GLFWwindow * glfwScherm);

///Past de tekenformaat van de metalen laag aan zodat die klopt met het grootte van het GLFW kader-geheugen
void werkMetaalLaagBij(void * laag, int breedte, int hoogte);