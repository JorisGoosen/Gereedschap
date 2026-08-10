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

	werkMetaalLaagBij((__bridge void *)laag, (int)inhoudsZicht.bounds.size.width, (int)inhoudsZicht.bounds.size.height);

	return (__bridge void *)laag;
}

void werkMetaalLaagBij(void * laag, int breedte, int hoogte)
{
	CAMetalLayer * metalenLaag = (__bridge CAMetalLayer *) laag;

	CGRect grenzen = metalenLaag.bounds;

	if(grenzen.size.width > 0 && grenzen.size.height > 0)
	{
		metalenLaag.contentsScale 	= (CGFloat)breedte / grenzen.size.width;
		metalenLaag.drawableSize 	= CGSizeMake(breedte, hoogte);
	}
	else
		metalenLaag.drawableSize 	= CGSizeMake(breedte, hoogte);
}