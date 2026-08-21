#include "metaalLaag.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <AppKit/AppKit.h>

void * maakMetaalLaag(GLFWwindow * glfwScherm)
{
	NSWindow 	* venster 		= glfwGetCocoaWindow(glfwScherm);
	NSView 		* inhoudsZicht 	= [venster contentView];

	id<MTLDevice> metaalApparaat = MTLCreateSystemDefaultDevice();

	CAMetalLayer * laag = [CAMetalLayer layer];
	laag.device 		= metaalApparaat;
	laag.pixelFormat 	= MTLPixelFormatBGRA8Unorm;
	laag.framebufferOnly = true;

	inhoudsZicht.wantsLayer 	= YES;
	inhoudsZicht.layer 			= laag;

	//Retina: de echte content-schaal (bv. 2.0) i.p.v. een verhouding uit bounds —
	//die klopt pas nadat het venster zijn uiteindelijke grootte heeft.
	float xs = 1.0f, ys = 1.0f;
	glfwGetWindowContentScale(glfwScherm, &xs, &ys);
	werkMetaalLaagBij((__bridge void *)laag,
	                  (int)(inhoudsZicht.bounds.size.width * xs),
	                  (int)(inhoudsZicht.bounds.size.height * ys),
	                  xs);

	return (__bridge void *)laag;
}

void werkMetaalLaagBij(void * laag, int breedte, int hoogte, float schaal)
{
	CAMetalLayer * metalenLaag = (__bridge CAMetalLayer *) laag;

	metalenLaag.contentsScale = (CGFloat)schaal;
	metalenLaag.drawableSize  = CGSizeMake(breedte, hoogte);
}