#include "weergaveScherm.h"
#include "metaalLaag.h"
#include <iostream>

using namespace glm;

weergaveScherm::toetsVerwerkerFunc weergaveScherm::_eigenVerwerker = nullptr;

std::map<GLFWwindow *, weergaveScherm*>	weergaveScherm::_schermen;

///De buffers uit de rest van de bibliotheek (wrgvOpslag ed) delen dit apparaat en deze rij
static WGPUDevice		s_gedeeldApparaat = nullptr;
static WGPUQueue		s_gedeeldeRij	= nullptr;

WGPUDevice gedeeldApparaat() { return s_gedeeldApparaat; }
WGPUQueue  gedeeldeRij()	 { return s_gedeeldeRij; 	}

///Het scherm dat op dit moment een weergave-pass open heeft staan
static weergaveScherm * s_huidigScherm = nullptr;

///wgpu-native-extensie: de surface is niet zichtbaar maar levert wel een geldige teken-textuur
constexpr WGPUSurfaceGetCurrentTextureStatus WGPUSurfaceGetCurrentTextureStatus_Occluded = (WGPUSurfaceGetCurrentTextureStatus)0x00030001;


static std::string _wgpFoutNaam(WGPUErrorType type)
{
	switch(type)
	{
	case WGPUErrorType_NoError:		return "Geen fout";
	case WGPUErrorType_Validation:	return "Validatie";
	case WGPUErrorType_OutOfMemory:	return "Geheugen op";
	case WGPUErrorType_Internal:	return "Intern";
	default:						return "Onbekend";
	}
}

static void wgpFoutMelder(WGPUDevice const * /*apparaat*/, WGPUErrorType type, WGPUStringView boodschap, void * /*userdata1*/, void * /*userdata2*/)
{
	std::string tekst = boodschap.data ? std::string(boodschap.data, boodschap.length) : "";

	meldWgpFout(std::string("wgpu-apparaat (") + _wgpFoutNaam(type) + "): " + tekst);
}


static void adapterVerwerver(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void * userdata1, void * /*userdata2*/)
{
	WGPUAdapter * inlaadAdres = static_cast<WGPUAdapter*>(userdata1);

	if(status == WGPURequestAdapterStatus_Success && adapter)
	{
		*inlaadAdres = adapter;
		wgpuAdapterAddRef(adapter);
	}
	else
	{
		std::string tekst = message.data ? std::string(message.data, message.length) : "";
		std::cerr << "Er kon geen wgpu-adapter worden verkregen: " << tekst << std::endl;
	}
}

static void apparaatVerwerver(WGPURequestDeviceStatus status, WGPUDevice apparaat, WGPUStringView message, void * userdata1, void * /*userdata2*/)
{
	WGPUDevice * inlaadAdres = static_cast<WGPUDevice*>(userdata1);

	if(status == WGPURequestDeviceStatus_Success && apparaat)
	{
		*inlaadAdres = apparaat;
		wgpuDeviceAddRef(apparaat);
	}
	else
	{
		std::string tekst = message.data ? std::string(message.data, message.length) : "";
		std::cerr << "Er kon geen wgpu-apparaat worden verkregen: " << tekst << std::endl;
	}
}


weergaveScherm::weergaveScherm(std::string Naam, size_t W, size_t H, size_t samples, bool volledigScherm)
: _schermVerhouding(float(W) / float(H)), _naam(Naam)
{
	std::cout << "weergaveScherm " << _naam << " created!" << std::endl;

    if (_schermen.size() == 0 && !glfwInit())
		throw std::runtime_error("Failed to intialize glfw");

	//We maken geen OpenGL context meer maar een venster puur voor wgpu
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _glfwScherm = glfwCreateWindow(W, H, _naam.c_str(), volledigScherm ? glfwGetPrimaryMonitor() : nullptr, nullptr);

    if (!_glfwScherm)
    {
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwTerminate();
        throw std::runtime_error("Failed to create window!");
    }

	//De metalen laag waar wgpu zijn tekenoppervlak aan kan hangen (macOS)
	_metaalLaag = maakMetaalLaag(_glfwScherm);

	int breedte, hoogte;
	glfwGetFramebufferSize(_glfwScherm, &breedte, &hoogte);

	//------------ wgpu basis ------------
	_wgpInstantie = wgpuCreateInstance(nullptr);

	if(!_wgpInstantie)
		throw std::runtime_error("Het aanmaken van de wgpu-instantie is mislukt...");

	_wgpAdapter  	= _vraagAdapter();
	_wgpApparaat 	= _vraagApparaat();
	_wgpRij 		= wgpuDeviceGetQueue(_wgpApparaat);

	//Zodat de rest van de bibliotheek dit apparaat ook kan gebruiken
	s_gedeeldApparaat = _wgpApparaat;
	s_gedeeldeRij	  = _wgpRij;

	//------------ tekenoppervlak (surface) ------------
	WGPUSurfaceSourceMetalLayer metaalBron = WGPU_SURFACE_SOURCE_METAL_LAYER_INIT;
	metaalBron.layer = _metaalLaag;

	WGPUSurfaceDescriptor oppervlakBeschrijving = WGPU_SURFACE_DESCRIPTOR_INIT;
	oppervlakBeschrijving.label 		= { _naam.c_str(), _naam.size() };
	oppervlakBeschrijving.nextInChain 	= &metaalBron.chain;

	_wgpOppervlak = wgpuInstanceCreateSurface(_wgpInstantie, &oppervlakBeschrijving);

	if(!_wgpOppervlak)
		throw std::runtime_error("Het aanmaken van het wgpu-oppervlak is mislukt...");

	//------------ formaat bepalen ------------
	WGPUSurfaceCapabilities mogelijkheden = WGPU_SURFACE_CAPABILITIES_INIT;
	wgpuSurfaceGetCapabilities(_wgpOppervlak, _wgpAdapter, &mogelijkheden);

	if(mogelijkheden.formatCount > 0)
		_oppervlakFormaat = mogelijkheden.formats[0];
	else
		throw std::runtime_error("Het oppervlak ondersteunt geen enkel tekenformaat...");

	wgpuSurfaceCapabilitiesFreeMembers(mogelijkheden);

	//------------ uniform-buffers en de basis-bind-groep ------------
	//bind-groep 0: binding 0 = beeld, binding 1 = matrices, binding 2 = extra
	const size_t beeldGrootte 	= 16;		// 3 floats + opvulling
	const size_t matrixGrootte	= 3 * 64;	// 3x mat4
	const size_t extraGrootte	= 64;		// 16 floats

	WGPUBufferDescriptor bufferBeschrijving = WGPU_BUFFER_DESCRIPTOR_INIT;
	bufferBeschrijving.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;

	bufferBeschrijving.size = beeldGrootte;
	_beeldBuffer = wgpuDeviceCreateBuffer(_wgpApparaat, &bufferBeschrijving);

	bufferBeschrijving.size = matrixGrootte;
	_matrixBuffer = wgpuDeviceCreateBuffer(_wgpApparaat, &bufferBeschrijving);

	bufferBeschrijving.size = extraGrootte;
	_extraBuffer = wgpuDeviceCreateBuffer(_wgpApparaat, &bufferBeschrijving);

	WGPUBindGroupLayoutEntry layoutInvoeren[3] = { WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT };

	layoutInvoeren[0].binding = 0;
	layoutInvoeren[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	layoutInvoeren[0].buffer.type = WGPUBufferBindingType_Uniform;
	layoutInvoeren[0].buffer.minBindingSize = beeldGrootte;

	layoutInvoeren[1].binding = 1;
	layoutInvoeren[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	layoutInvoeren[1].buffer.type = WGPUBufferBindingType_Uniform;
	layoutInvoeren[1].buffer.minBindingSize = matrixGrootte;

	layoutInvoeren[2].binding = 2;
	layoutInvoeren[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	layoutInvoeren[2].buffer.type = WGPUBufferBindingType_Uniform;
	layoutInvoeren[2].buffer.minBindingSize = extraGrootte;

	WGPUBindGroupLayoutDescriptor layoutBeschrijving = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
	layoutBeschrijving.entryCount = 3;
	layoutBeschrijving.entries = layoutInvoeren;
	_basisBindGroepLayout = wgpuDeviceCreateBindGroupLayout(_wgpApparaat, &layoutBeschrijving);

	WGPUBindGroupEntry bindInvoeren[3] = { WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT };

	bindInvoeren[0].binding = 0;
	bindInvoeren[0].buffer = _beeldBuffer;
	bindInvoeren[0].size	 = beeldGrootte;

	bindInvoeren[1].binding = 1;
	bindInvoeren[1].buffer = _matrixBuffer;
	bindInvoeren[1].size	 = matrixGrootte;

	bindInvoeren[2].binding = 2;
	bindInvoeren[2].buffer = _extraBuffer;
	bindInvoeren[2].size	 = extraGrootte;

	WGPUBindGroupDescriptor bindGroepBeschrijving = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
	bindGroepBeschrijving.layout 		= _basisBindGroepLayout;
	bindGroepBeschrijving.entryCount 	= 3;
	bindGroepBeschrijving.entries 		= bindInvoeren;
	_basisBindGroep = wgpuDeviceCreateBindGroup(_wgpApparaat, &bindGroepBeschrijving);

	//------------ sampler en textuur-bind-groep (groep 1) ------------
	WGPUSamplerDescriptor samplerBeschrijving = WGPU_SAMPLER_DESCRIPTOR_INIT;
	samplerBeschrijving.addressModeU 	= WGPUAddressMode_ClampToEdge;
	samplerBeschrijving.addressModeV 	= WGPUAddressMode_ClampToEdge;
	samplerBeschrijving.magFilter		= WGPUFilterMode_Linear;
	samplerBeschrijving.minFilter		= WGPUFilterMode_Linear;
	samplerBeschrijving.mipmapFilter	= WGPUMipmapFilterMode_Linear;
	_sampler = wgpuDeviceCreateSampler(_wgpApparaat, &samplerBeschrijving);

	WGPUBindGroupLayoutEntry textuurInvoeren[2] = { WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT };

	textuurInvoeren[0].binding = 0;
	textuurInvoeren[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	textuurInvoeren[0].texture.sampleType		= WGPUTextureSampleType_Float;
	textuurInvoeren[0].texture.viewDimension	= WGPUTextureViewDimension_2D;

	textuurInvoeren[1].binding = 1;
	textuurInvoeren[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	textuurInvoeren[1].sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor textuurLayoutBeschrijving = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
	textuurLayoutBeschrijving.entryCount = 2;
	textuurLayoutBeschrijving.entries = textuurInvoeren;
	_textuurBindGroepLayout = wgpuDeviceCreateBindGroupLayout(_wgpApparaat, &textuurLayoutBeschrijving);

	//------------ een 1x1 wit hulpje zodat bind-groep 1 altijd gebonden kan worden ------------
	unsigned char wit[4] = { 255, 255, 255, 255 };

	WGPUExtent3D witteOmvang = { 1, 1, 1 };

	WGPUTextureDescriptor witteBeschrijving = WGPU_TEXTURE_DESCRIPTOR_INIT;
	witteBeschrijving.usage 		= WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
	witteBeschrijving.dimension 	= WGPUTextureDimension_2D;
	witteBeschrijving.format 		= WGPUTextureFormat_RGBA8Unorm;
	witteBeschrijving.size 			= witteOmvang;
	witteBeschrijving.mipLevelCount = 1;
	witteBeschrijving.sampleCount 	= 1;
	_witteTextuur = wgpuDeviceCreateTexture(_wgpApparaat, &witteBeschrijving);

	WGPUTexelCopyTextureInfo witteBestemming = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
	witteBestemming.texture = _witteTextuur;
	WGPUTexelCopyBufferLayout witteBron = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
	witteBron.bytesPerRow = 4;
	WGPUExtent3D witteEnkel = { 1, 1, 1 };
	wgpuQueueWriteTexture(_wgpRij, &witteBestemming, wit, 4, &witteBron, &witteEnkel);

	WGPUTextureView witteZicht = wgpuTextureCreateView(_witteTextuur, nullptr);

	WGPUBindGroupEntry witteInvoeren[2] = { WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT };
	witteInvoeren[0].binding = 0;
	witteInvoeren[0].textureView = witteZicht;
	witteInvoeren[1].binding = 1;
	witteInvoeren[1].sampler = _sampler;

	WGPUBindGroupDescriptor witteBindGroepBeschrijving = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
	witteBindGroepBeschrijving.layout 		= _textuurBindGroepLayout;
	witteBindGroepBeschrijving.entryCount 	= 2;
	witteBindGroepBeschrijving.entries 		= witteInvoeren;
	_witteBindGroep = wgpuDeviceCreateBindGroup(_wgpApparaat, &witteBindGroepBeschrijving);

	wgpuTextureViewRelease(witteZicht);

	_configureerOppervlak(breedte, hoogte);

	_schermen[_glfwScherm] = this;

	glfwSetKeyCallback(_glfwScherm, toetsVerwerkerCentraal);

	(void)samples; // multi-sampling is (nog) niet volledig overgezet naar WebGPU
}

WGPUAdapter weergaveScherm::_vraagAdapter()
{
	WGPURequestAdapterOptions opties = WGPU_REQUEST_ADAPTER_OPTIONS_INIT;
	opties.powerPreference = WGPUPowerPreference_HighPerformance;

	WGPUAdapter resultaat = nullptr;

	WGPURequestAdapterCallbackInfo verwerkerInfo = WGPU_REQUEST_ADAPTER_CALLBACK_INFO_INIT;
	verwerkerInfo.mode 		= WGPUCallbackMode_AllowSpontaneous;
	verwerkerInfo.callback 	= adapterVerwerver;
	verwerkerInfo.userdata1 = &resultaat;

	wgpuInstanceRequestAdapter(_wgpInstantie, &opties, verwerkerInfo);

	//wacht tot de verwerker is aangeroepen (wgpuInstanceWaitAny is in wgpu-native nog niet geïmplementeerd)
	while(!resultaat)
		wgpuInstanceProcessEvents(_wgpInstantie);

	if(!resultaat)
		throw std::runtime_error("Het verkrijgen van een wgpu-adapter is mislukt!");

	return resultaat;
}

WGPUDevice weergaveScherm::_vraagApparaat()
{
	WGPUDeviceDescriptor beschrijving = WGPU_DEVICE_DESCRIPTOR_INIT;
	beschrijving.label 							= { _naam.c_str(), _naam.size() };
	beschrijving.uncapturedErrorCallbackInfo.callback 	= wgpFoutMelder;
	beschrijving.uncapturedErrorCallbackInfo.userdata1 	= this;

	WGPUDevice resultaat = nullptr;

	WGPURequestDeviceCallbackInfo verwerkerInfo = WGPU_REQUEST_DEVICE_CALLBACK_INFO_INIT;
	verwerkerInfo.mode 		= WGPUCallbackMode_AllowSpontaneous;
	verwerkerInfo.callback 	= apparaatVerwerver;
	verwerkerInfo.userdata1 = &resultaat;

	wgpuAdapterRequestDevice(_wgpAdapter, &beschrijving, verwerkerInfo);

	//wacht tot de verwerker is aangeroepen
	while(!resultaat)
		wgpuInstanceProcessEvents(_wgpInstantie);

	if(!resultaat)
		throw std::runtime_error("Het verkrijgen van een wgpu-apparaat is mislukt!");

	return resultaat;
}

void weergaveScherm::_configureerOppervlak(uint32_t breedte, uint32_t hoogte)
{
	if(breedte == 0) breedte = 1;
	if(hoogte == 0) hoogte = 1;

	WGPUSurfaceConfiguration configuratie = WGPU_SURFACE_CONFIGURATION_INIT;
	configuratie.device 		= _wgpApparaat;
	configuratie.format 		= _oppervlakFormaat;
	configuratie.usage 			= WGPUTextureUsage_RenderAttachment;
	configuratie.width 			= breedte;
	configuratie.height 		= hoogte;
	configuratie.presentMode 	= WGPUPresentMode_Fifo;
	configuratie.alphaMode 		= WGPUCompositeAlphaMode_Auto;

	wgpuSurfaceConfigure(_wgpOppervlak, &configuratie);

	_oppervlakBreedte = breedte;
	_oppervlakHoogte  = hoogte;
}

void weergaveScherm::toetsVerwerkerCentraal(GLFWwindow * scherm, int key, int scancode, int action, int mods)
{
	if(_eigenVerwerker)
		_eigenVerwerker(key, scancode, action, mods);

	if(_schermen.count(scherm) > 0)
		_schermen[scherm]->toetsVerwerker(key, scancode, action, mods);
}

void weergaveScherm::toetsVerwerker(int key, int , int action, int )
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}

weergaveScherm::~weergaveScherm()
{
	_schermen.erase(_glfwScherm);

	for(WGPUBindGroup bindGroep : _gevormdeBindGroepen)
		wgpuBindGroupRelease(bindGroep);
	for(auto & [naam, bindGroep] : _textuurBindGroepen)
		wgpuBindGroupRelease(bindGroep);
	for(WGPUBindGroup bindGroep : _rekenBindGroepen)
		wgpuBindGroupRelease(bindGroep);

	if(_basisBindGroep) 		wgpuBindGroupRelease(_basisBindGroep);
	if(_basisBindGroepLayout) 	wgpuBindGroupLayoutRelease(_basisBindGroepLayout);
	if(_textuurBindGroep) 		wgpuBindGroupRelease(_textuurBindGroep);
	if(_textuurBindGroepLayout)	wgpuBindGroupLayoutRelease(_textuurBindGroepLayout);
	if(_witteBindGroep) 		wgpuBindGroupRelease(_witteBindGroep);
	if(_witteTextuur) 			wgpuTextureRelease(_witteTextuur);

	if(_sampler) 		wgpuSamplerRelease(_sampler);
	if(_rekenBindGroepLayout) wgpuBindGroupLayoutRelease(_rekenBindGroepLayout);
	if(_leegRekenBuffer) 	  wgpuBufferRelease(_leegRekenBuffer);

	if(_beeldBuffer) 	wgpuBufferRelease(_beeldBuffer);
	if(_matrixBuffer) 	wgpuBufferRelease(_matrixBuffer);
	if(_extraBuffer) 	wgpuBufferRelease(_extraBuffer);

	for(auto & [naam, programma] : _shaderProgrammas)
		if(programma) wgpuRenderPipelineRelease(programma);
	for(auto & [naam, programma] : _rekenProgrammas)
		if(programma) wgpuComputePipelineRelease(programma);
	for(auto & [naam, modules] : _shaderModules)
	{
		wgpuShaderModuleRelease(modules.first);
		wgpuShaderModuleRelease(modules.second);
	}
	for(auto & [naam, textuur] : _texturen)
		if(textuur) wgpuTextureRelease(textuur);

	if(_diepteZicht) 	wgpuTextureViewRelease(_diepteZicht);
	if(_diepteTextuur) 	wgpuTextureRelease(_diepteTextuur);

	if(_wgpOppervlak) 	wgpuSurfaceRelease(_wgpOppervlak);
	if(_wgpAdapter) 	wgpuAdapterRelease(_wgpAdapter);
	if(_wgpApparaat) 	wgpuDeviceRelease(_wgpApparaat);
	if(_wgpInstantie) 	wgpuInstanceRelease(_wgpInstantie);

   	glfwDestroyWindow(_glfwScherm);

	if(_schermen.size() == 0)
		glfwTerminate();
}

void weergaveScherm::bereidWeergevenVoor(const std::string & shader, bool wisScherm)
{
	laadOmgeving();

	int breedte, hoogte;

	if(_doelTextuur)
	{
		//off-screen doel: de grootte van de doel-textuur telt
		breedte = _doelGrootte.x;
		hoogte  = _doelGrootte.y;
	}
	else
	{
		glfwGetFramebufferSize(_glfwScherm, &breedte, &hoogte);

		//Als het venstergrootte veranderd is moet het tekenoppervlak opnieuw geconfigureerd worden
		if((uint32_t)breedte != _oppervlakBreedte || (uint32_t)hoogte != _oppervlakHoogte)
			_configureerOppervlak(breedte, hoogte);

		werkMetaalLaagBij(_metaalLaag, breedte, hoogte);
	}

	_bereidWeergevenVoor(shader, wisScherm, breedte, hoogte);
}

void weergaveScherm::_bereidWeergevenVoor(const std::string & shader, bool wisScherm, int breedte, int hoogte)
{
	_schermVerhouding = breedte / (float) hoogte;

	static int vorigeH = -1, vorigeB = -1;

	if(vorigeH != hoogte || vorigeB != breedte)
		std::cout << "Schermgrootte: " << breedte << "x" << hoogte << std::endl;

	vorigeH = hoogte, vorigeB = breedte;

	//Welk programma moet er gebruikt worden?
	if		(shader == "" && _shaderModules.size() == 1)			_huidigProgrammaNaam = _shaderModules.begin()->first;
	else if	(shader != "" && _shaderModules.count(shader) > 0)		_huidigProgrammaNaam = shader;
	else if	(shader != "")											std::cerr << "bereidWeergevenVoor kon geen shader genaamd \"" << shader << "\" vinden..." << std::endl;

_huidigProgramma = _shaderProgrammas.count(_huidigProgrammaNaam) > 0 ? _shaderProgrammas[_huidigProgrammaNaam] : nullptr;

	//Een "apparaat" tekstuur als doel (off-screen) of het tekenfragment van het wgpu-oppervlak
	if(_doelTextuur)
	{
		_oppervlakTextuur = _doelTextuur;
		_oppervlakZicht   = wgpuTextureCreateView(_oppervlakTextuur, nullptr);
	}
	else
	{
		//Haal het tekenfragment (textuur) van het wgpu-oppervlak
		WGPUSurfaceTexture oppervlakTextuur = WGPU_SURFACE_TEXTURE_INIT;
		wgpuSurfaceGetCurrentTexture(_wgpOppervlak, &oppervlakTextuur);

		if(oppervlakTextuur.status == WGPUSurfaceGetCurrentTextureStatus_Occluded || !oppervlakTextuur.texture)
		{
			//extensie "Occluded": het venster is (even) niet zichtbaar en levert geen tekstuur,
			//dus wordt deze frame overgeslagen (rondWeergevenAf doet dan niets)
			static bool occludedGemeld = false;

			if(!occludedGemeld)
			{
				std::cout << "wgpu-oppervlak is (even) niet zichtbaar: frames worden overgeslagen..." << std::endl;
				occludedGemeld = true;
			}

			_oppervlakTextuur = nullptr;
			_oppervlakZicht   = nullptr;
			return;
		}

		if(oppervlakTextuur.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal &&
		   oppervlakTextuur.status != WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal)
			throw std::runtime_error("Er kon geen tekenfragment worden verkregen van het wgpu-oppervlak... (status " + std::to_string((int)oppervlakTextuur.status) + ")");

		_oppervlakTextuur = oppervlakTextuur.texture;
		_oppervlakZicht   = wgpuTextureCreateView(_oppervlakTextuur, nullptr);
	}

	_gebondenTextuur.clear();

	//De beeldparameters moeten vóór de teken-pass geüpload worden!
	_beeldenUniformen(breedte, hoogte);
	extraVoorbereidingen(_huidigProgramma);

	_zorgDiepteTextuur(breedte, hoogte);

	_commandEncoder = wgpuDeviceCreateCommandEncoder(_wgpApparaat, nullptr);

	WGPURenderPassColorAttachment kleurHechting = WGPU_RENDER_PASS_COLOR_ATTACHMENT_INIT;
	kleurHechting.view 		= _oppervlakZicht;
	kleurHechting.loadOp 	= wisScherm ? WGPULoadOp_Clear : WGPULoadOp_Load;
	kleurHechting.storeOp 	= WGPUStoreOp_Store;
	kleurHechting.clearValue = { _weergaveKleur[0], _weergaveKleur[1], _weergaveKleur[2], _weergaveKleur[3] };

	//de z-buffer wordt elke frame schoongemaakt
	WGPURenderPassDepthStencilAttachment diepteHechting = WGPU_RENDER_PASS_DEPTH_STENCIL_ATTACHMENT_INIT;
	diepteHechting.view 			= _diepteZicht;
	diepteHechting.depthLoadOp 		= WGPULoadOp_Clear;
	diepteHechting.depthStoreOp 	= WGPUStoreOp_Discard;
	diepteHechting.depthClearValue 	= 1.0;
	diepteHechting.stencilLoadOp 	= WGPULoadOp_Clear;
	diepteHechting.stencilStoreOp 	= WGPUStoreOp_Discard;

	WGPURenderPassDescriptor passBeschrijving = WGPU_RENDER_PASS_DESCRIPTOR_INIT;
	passBeschrijving.label 				= { _huidigProgrammaNaam.c_str(), _huidigProgrammaNaam.size() };
	passBeschrijving.colorAttachmentCount 	= 1;
	passBeschrijving.colorAttachments 		= &kleurHechting;
	passBeschrijving.depthStencilAttachment = &diepteHechting;

	_weergavePass = wgpuCommandEncoderBeginRenderPass(_commandEncoder, &passBeschrijving);

	s_huidigScherm = this;

	wgpuRenderPassEncoderSetViewport(_weergavePass, 0.0f, 0.0f, (float)breedte, (float)hoogte, 0.0f, 1.0f);
}

///Schrijft de schermparameters (breedte/hoogte/verhouding) naar de beeld-buffer
void weergaveScherm::_beeldenUniformen(int breedte, int hoogte)
{
	struct beeldParameters
	{
		float schermBreedte;
		float schermHoogte;
		float schermVerhouding;
		float _opvulling;
	};

	beeldParameters parameters;
	parameters.schermBreedte 	= (float)breedte;
	parameters.schermHoogte 	= (float)hoogte;
	parameters.schermVerhouding = _schermVerhouding;
	parameters._opvulling 		= 0.0f;

	wgpuQueueWriteBuffer(_wgpRij, _beeldBuffer, 0, &parameters, sizeof(beeldParameters));
}

void weergaveScherm::zetMatrices(const glm::mat4 & projectie, const glm::mat4 & modelZicht, const glm::mat4 & transInvMV)
{
	glm::mat4 matrices[3] = { projectie, modelZicht, transInvMV };

	wgpuQueueWriteBuffer(_wgpRij, _matrixBuffer, 0, glm::value_ptr(matrices[0]), sizeof(matrices));

	wgpFoutControle("weergaveScherm::zetMatrices(): ");
}

void weergaveScherm::zetExtraFloat(int plek, float waarde)
{
	float data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	data[plek & 3] = waarde;

	wgpuQueueWriteBuffer(_wgpRij, _extraBuffer, (plek >> 2) * 16, data, 16);

	wgpFoutControle("weergaveScherm::zetExtraFloat(): ");
}

void weergaveScherm::zetExtraFloats(const float * waarden, int aantal)
{
	if(aantal <= 0)
		return;

	wgpuQueueWriteBuffer(_wgpRij, _extraBuffer, 0, waarden, (uint64_t)std::min(aantal, 16) * sizeof(float));

	wgpFoutControle("weergaveScherm::zetExtraFloats(): ");
}

void weergaveScherm::zetWeergaveDoel(WGPUTexture textuur, glm::uvec2 grootte)
{
	_doelTextuur 	= textuur;
	_doelFormaat 	= WGPUTextureFormat_RGBA8Unorm;
	_doelGrootte 	= grootte;
}

void weergaveScherm::verbindRekenBuffer(uint32_t binding, WGPUBuffer buffer)
{
	if(binding >= _rekenBufferBinden.size())
		throw std::runtime_error("verbindRekenBuffer: alleen bindings 0..3 zijn ondersteund!");

	_rekenBufferBinden[binding] = buffer;
}

///Vraagt (of maakt) de bind-groep voor een textuur (groep 1)
WGPUBindGroup weergaveScherm::_bindgroepVoorTextuur(const std::string & textuurNaam)
{
	auto gevonden = _textuurBindGroepen.find(textuurNaam);

	if(gevonden != _textuurBindGroepen.end())
		return gevonden->second;

	if(!_texturen.count(textuurNaam) || !_textuurBindGroepLayout)
		return nullptr;

	WGPUTextureView zicht = wgpuTextureCreateView(_texturen.at(textuurNaam), nullptr);

	WGPUBindGroupEntry invoeren[2] = { WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT };

	invoeren[0].binding 	 = 0;
	invoeren[0].textureView  = zicht;

	invoeren[1].binding 	= 1;
	invoeren[1].sampler 	= _sampler;

	WGPUBindGroupDescriptor beschrijving = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
	beschrijving.layout 	 = _textuurBindGroepLayout;
	beschrijving.entryCount = 2;
	beschrijving.entries 	 = invoeren;

	WGPUBindGroup bindGroep = wgpuDeviceCreateBindGroup(_wgpApparaat, &beschrijving);

	wgpuTextureViewRelease(zicht);

	if(bindGroep)
		_textuurBindGroepen[textuurNaam] = bindGroep;

	return bindGroep;
}

///Bindt de actief gebonden textuur aan bind-groep 1 van de pass (of het witte hulpje)
void weergaveScherm::_bindTextuurAanPass(WGPURenderPassEncoder pass)
{
	WGPUBindGroup bindGroep = _witteBindGroep;

	if(!_gebondenTextuur.empty())
		bindGroep = _bindgroepVoorTextuur(_gebondenTextuur);

	if(bindGroep)
		wgpuRenderPassEncoderSetBindGroup(pass, 1, bindGroep, 0, nullptr);
}

WGPURenderPipeline weergaveScherm::_maakPipeline(const std::string & programmaNaam, const std::string & moduleNaam, const std::vector<WGPUVertexBufferLayout> & vertexLayouts, WGPUPrimitiveTopology topologie, WGPUIndexFormat stripFormaat)
{
	if(!_shaderModules.count(moduleNaam))
		throw std::runtime_error("_maakPipeline: er zijn geen shader modules voor \"" + moduleNaam + "\"...");

	auto [vertModule, fragModule] = _shaderModules[moduleNaam];

	WGPUBindGroupLayout groepen[2] = { _basisBindGroepLayout, _textuurBindGroepLayout };

	WGPUPipelineLayoutDescriptor layoutBeschrijving = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
	layoutBeschrijving.label 				= { programmaNaam.c_str(), programmaNaam.size() };
	layoutBeschrijving.bindGroupLayoutCount 	= 2;
	layoutBeschrijving.bindGroupLayouts 		= groepen;

	WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(_wgpApparaat, &layoutBeschrijving);

	WGPUVertexState vertexStaat = WGPU_VERTEX_STATE_INIT;
	vertexStaat.module 		= vertModule;
	vertexStaat.entryPoint 	= { "main", 4 };
	vertexStaat.bufferCount = vertexLayouts.size();
	vertexStaat.buffers 	= vertexStaat.bufferCount > 0 ? vertexLayouts.data() : nullptr;

	WGPUColorTargetState kleurDoel = WGPU_COLOR_TARGET_STATE_INIT;
	kleurDoel.format 	= _doelTextuur ? _doelFormaat : _oppervlakFormaat;
	kleurDoel.writeMask = WGPUColorWriteMask_All;

	WGPUFragmentState fragmentStaat = WGPU_FRAGMENT_STATE_INIT;
	fragmentStaat.module 		= fragModule;
	fragmentStaat.entryPoint 	= { "main", 4 };
	fragmentStaat.targetCount 	= 1;
	fragmentStaat.targets 		= &kleurDoel;

	WGPUPrimitiveState primitief = WGPU_PRIMITIVE_STATE_INIT;
	primitief.topology 	= topologie;
	primitief.frontFace = WGPUFrontFace_CCW;
	primitief.cullMode 	= WGPUCullMode_None;
	if(topologie == WGPUPrimitiveTopology_TriangleStrip)
		primitief.stripIndexFormat = stripFormaat;

	WGPUMultisampleState multi = WGPU_MULTISAMPLE_STATE_INIT;
	multi.count = 1;
	multi.mask 	= 0xFFFFFFFF;

	WGPUDepthStencilState diepteStaat = WGPU_DEPTH_STENCIL_STATE_INIT;
	diepteStaat.format 			= WGPUTextureFormat_Depth32Float;
	diepteStaat.depthWriteEnabled 	= WGPUOptionalBool_True;
	diepteStaat.depthCompare 		= WGPUCompareFunction_Less;
	diepteStaat.stencilReadMask 		= 0xFFFFFFFF;
	diepteStaat.stencilWriteMask 		= 0xFFFFFFFF;

	WGPURenderPipelineDescriptor beschrijving = WGPU_RENDER_PIPELINE_DESCRIPTOR_INIT;
	beschrijving.label 		= { programmaNaam.c_str(), programmaNaam.size() };
	beschrijving.layout 	= layout;
	beschrijving.vertex 	= vertexStaat;
	beschrijving.primitive 	= primitief;
	beschrijving.multisample = multi;
	beschrijving.depthStencil = &diepteStaat;
	beschrijving.fragment 	= &fragmentStaat;

	WGPURenderPipeline programma = wgpuDeviceCreateRenderPipeline(_wgpApparaat, &beschrijving);

	wgpuPipelineLayoutRelease(layout);

	if(!programma)
		throw std::runtime_error("Het maken van de weergave-pipeline \"" + programmaNaam + "\" is mislukt!");

	return programma;
}

WGPURenderPipeline weergaveScherm::zorgVoorPuntenProgramma()
{
	const std::string sleutel = _huidigProgrammaNaam + ":punten";

	if(_shaderProgrammas.count(sleutel))
		return _shaderProgrammas[sleutel];

	//een punt heeft maar één eigenschap: een vec4 (PlaatsKleur)
	WGPUVertexAttribute puntAttribuut = {};
	puntAttribuut.format 			= WGPUVertexFormat_Float32x4;
	puntAttribuut.offset 			= 0;
	puntAttribuut.shaderLocation 	= 0;

	WGPUVertexBufferLayout puntLayout = {};
	puntLayout.arrayStride 		= 16;
	puntLayout.stepMode 		= WGPUVertexStepMode_Vertex;
	puntLayout.attributeCount 	= 1;
	puntLayout.attributes 		= &puntAttribuut;

	std::vector<WGPUVertexBufferLayout> layouts = { puntLayout };

	WGPURenderPipeline programma = _maakPipeline(sleutel, _huidigProgrammaNaam, layouts, WGPUPrimitiveTopology_PointList, WGPUIndexFormat_Undefined);

	slaShaderOp(sleutel, programma);

	return programma;
}

void weergaveScherm::tekenKadertjes(WGPUBuffer vierkantje, uint32_t vierkantPunten, WGPUBuffer plekken, uint32_t aantalPlekken)
{
	if(!_weergavePass)
		return;

	const std::string sleutel = _huidigProgrammaNaam + ":kadertjes";

	if(!_shaderProgrammas.count(sleutel))
	{
		WGPUVertexAttribute attributen[2] = {};
		attributen[0].format = WGPUVertexFormat_Float32x2;
		attributen[0].offset = 0;
		attributen[0].shaderLocation = 0;
		attributen[1].format = WGPUVertexFormat_Float32x4;
		attributen[1].offset = 0;
		attributen[1].shaderLocation = 1;

		WGPUVertexBufferLayout layouts[2] = {};
		layouts[0].arrayStride = 8;
		layouts[0].stepMode = WGPUVertexStepMode_Vertex;
		layouts[0].attributeCount = 1;
		layouts[0].attributes = &attributen[0];
		layouts[1].arrayStride = 16;
		layouts[1].stepMode = WGPUVertexStepMode_Instance;
		layouts[1].attributeCount = 1;
		layouts[1].attributes = &attributen[1];

		std::vector<WGPUVertexBufferLayout> vecLayouts = { layouts[0], layouts[1] };

		WGPURenderPipeline programma = _maakPipeline(sleutel, _huidigProgrammaNaam, vecLayouts, WGPUPrimitiveTopology_TriangleList, WGPUIndexFormat_Undefined);

		slaShaderOp(sleutel, programma);
	}

	wgpuRenderPassEncoderSetPipeline(_weergavePass, _shaderProgrammas[sleutel]);
	wgpuRenderPassEncoderSetBindGroup(_weergavePass, 0, _basisBindGroep, 0, nullptr);
	_bindTextuurAanPass(_weergavePass);

	wgpuRenderPassEncoderSetVertexBuffer(_weergavePass, 0, vierkantje, 0, wgpuBufferGetSize(vierkantje));
	wgpuRenderPassEncoderSetVertexBuffer(_weergavePass, 1, plekken, 0, wgpuBufferGetSize(plekken));
	wgpuRenderPassEncoderDraw(_weergavePass, vierkantPunten, aantalPlekken, 0, 0);

	wgpFoutControle("weergaveScherm::tekenKadertjes(): ");
}

void weergaveScherm::tekenAlsPunten(WGPUBuffer buffer, uint32_t puntAantal)
{
	if(!_weergavePass)
		return;

	WGPURenderPipeline programma = zorgVoorPuntenProgramma();

	wgpuRenderPassEncoderSetPipeline(_weergavePass, programma);
	wgpuRenderPassEncoderSetBindGroup(_weergavePass, 0, _basisBindGroep, 0, nullptr);
	_bindTextuurAanPass(_weergavePass);

	wgpuRenderPassEncoderSetVertexBuffer(_weergavePass, 0, buffer, 0, wgpuBufferGetSize(buffer));
	wgpuRenderPassEncoderDraw(_weergavePass, puntAantal, 1, 0, 0);

	wgpFoutControle("weergaveScherm::tekenAlsPunten(): ");
}

void weergaveScherm::_zorgDiepteTextuur(uint32_t breedte, uint32_t hoogte)
{
	if(breedte == 0 || hoogte == 0)
		return;

	if(_diepteTextuur && _diepteBreedte == breedte && _diepteHoogte == hoogte)
		return;

	if(_diepteZicht) 		{ wgpuTextureViewRelease(_diepteZicht); 	_diepteZicht = nullptr; }
	if(_diepteTextuur) 	{ wgpuTextureRelease(_diepteTextuur);	_diepteTextuur = nullptr; }

	_diepteBreedte = breedte;
	_diepteHoogte  = hoogte;

	WGPUExtent3D omvang = { breedte, hoogte, 1 };

	WGPUTextureDescriptor beschrijving = WGPU_TEXTURE_DESCRIPTOR_INIT;
	beschrijving.usage 		= WGPUTextureUsage_RenderAttachment;
	beschrijving.dimension 	= WGPUTextureDimension_2D;
	beschrijving.format 	= WGPUTextureFormat_Depth32Float;
	beschrijving.size 		= omvang;
	beschrijving.mipLevelCount = 1;
	beschrijving.sampleCount 	= 1;

	_diepteTextuur = wgpuDeviceCreateTexture(_wgpApparaat, &beschrijving);
	_diepteZicht   = wgpuTextureCreateView(_diepteTextuur, nullptr);
}

void weergaveScherm::laadOmgeving()
{
	glfwMakeContextCurrent(_glfwScherm);
}

void wrgvOpslag::tekenGeïndexeerd()
{
	weergaveScherm * scherm = s_huidigScherm;

	if(!scherm || !scherm->weergavePass())
		return;

	WGPURenderPipeline programma = scherm->zorgVoorProgramma(this);
	WGPURenderPassEncoder pass  = scherm->weergavePass();

	wgpuRenderPassEncoderSetPipeline(pass, programma);
	wgpuRenderPassEncoderSetBindGroup(pass, 0, scherm->basisBindGroep(), 0, nullptr);

	bindVoorWeergave(pass);
	scherm->_bindTextuurAanPass(pass);

	if(aantalIndices() > 0)
		wgpuRenderPassEncoderDrawIndexed(pass, (uint32_t)aantalIndices(), 1, 0, 0, 0);
	else if(aantalVertices() > 0)
		wgpuRenderPassEncoderDraw(pass, (uint32_t)aantalVertices(), 1, 0, 0);

	wgpFoutControle("wrgvOpslag::tekenGeïndexeerd(): ");
}

void weergaveScherm::doeRekenVerwerker(const std::string & verwerker, glm::uvec3 groepGroottes, std::function<void()> renderVoorbereiding)
{
	WGPUComputePipeline programma = _rekenProgrammas.count(verwerker) > 0 ? _rekenProgrammas[verwerker] : nullptr;

	if(!programma)
		throw std::runtime_error("doeRekenVerwerker kon het reken-programma \"" + verwerker + "\" niet vinden...");

	WGPUCommandEncoder encoder 		= wgpuDeviceCreateCommandEncoder(_wgpApparaat, nullptr);
	WGPUComputePassEncoder rekenPass = wgpuCommandEncoderBeginComputePass(encoder, nullptr);

	wgpuComputePassEncoderSetPipeline(rekenPass, programma);

	renderVoorbereiding(); //o.a. verbindRekenBuffer

	//bouw de bind-groep met de groepsbuffers (en opvul-buffers voor ongebruikte bindings)
	WGPUBindGroupEntry invoeren[4] = { WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT, WGPU_BIND_GROUP_ENTRY_INIT };

	for(int i = 0; i < 4; i++)
	{
		invoeren[i].binding = i;
		invoeren[i].buffer 	= _rekenBufferBinden[i] ? _rekenBufferBinden[i] : _leegRekenBuffer;
		invoeren[i].size 	= WGPU_WHOLE_SIZE;
	}

	WGPUBindGroupDescriptor bindGroepBeschrijving = WGPU_BIND_GROUP_DESCRIPTOR_INIT;
	bindGroepBeschrijving.layout 		= _rekenBindGroepLayout;
	bindGroepBeschrijving.entryCount 	= 4;
	bindGroepBeschrijving.entries 		= invoeren;

	WGPUBindGroup bindGroep = wgpuDeviceCreateBindGroup(_wgpApparaat, &bindGroepBeschrijving);

	wgpuComputePassEncoderSetBindGroup(rekenPass, 0, bindGroep, 0, nullptr);

	wgpuComputePassEncoderDispatchWorkgroups(rekenPass, groepGroottes.x, groepGroottes.y, groepGroottes.z);

	wgpuComputePassEncoderEnd(rekenPass);

	WGPUCommandBuffer commando = wgpuCommandEncoderFinish(encoder, nullptr);
	wgpuQueueSubmit(_wgpRij, 1, &commando);

	wgpuBindGroupRelease(bindGroep);
	wgpuCommandBufferRelease(commando);
	wgpuCommandEncoderRelease(encoder);

	wgpFoutControle("doeRekenVerwerker('" + verwerker + "'): ");
}

void weergaveScherm::rondWeergevenAf()
{
	if(!_weergavePass)
	{
		glfwPollEvents();
		return;
	}

	wgpuRenderPassEncoderEnd(_weergavePass);

	WGPUCommandBuffer commando = wgpuCommandEncoderFinish(_commandEncoder, nullptr);

	wgpuQueueSubmit(_wgpRij, 1, &commando);

	//Bij een off-screen doel (nepScherm) hoeft er niet gepresenteerd te worden
	if(!_doelTextuur)
		wgpuSurfacePresent(_wgpOppervlak);

	wgpuCommandBufferRelease(commando);
	wgpuCommandEncoderRelease(_commandEncoder);

	wgpuTextureViewRelease(_oppervlakZicht);
	if(!_doelTextuur)
		wgpuTextureRelease(_oppervlakTextuur);

	_weergavePass		= nullptr;
	_commandEncoder 	= nullptr;
	_oppervlakZicht 	= nullptr;
	_oppervlakTextuur 	= nullptr;
	_doelTextuur	 	= nullptr;

	if(_diepteZicht) 	{ wgpuTextureViewRelease(_diepteZicht); 	_diepteZicht = nullptr; }
	if(_diepteTextuur) 	{ wgpuTextureRelease(_diepteTextuur);	_diepteTextuur = nullptr; }

	glfwPollEvents();
	wgpFoutControle("weergaveScherm::rondWeergevenAf(): ");
}

///WebGPU kent geen vlak-verdelings shaders (dus geen tessellation)
[[noreturn]] void weergaveScherm::maakVlakVerdelingsShader(const std::string & shaderNaam, const std::string &, const std::string &, const std::string &, const std::string &)
{
	werpOnondersteund("Vlak-verdelings shader '" + shaderNaam + "' (WebGPU kent geen tessellation)");
}

///WebGPU kent geen geometry shaders
[[noreturn]] void weergaveScherm::maakGeometrieShader(const std::string & shaderNaam, const std::string &, const std::string &, const std::string &)
{
	werpOnondersteund("Geometry shader '" + shaderNaam + "' (WebGPU kent geen geometry-shader)");
}

WGPURenderPipeline weergaveScherm::maakShader(const std::string & shaderNaam, const std::string & vertshaderbestand, const std::string & fragshaderbestand)
{
	WGPUShaderModule vertModule = _maakShaderModule(vertshaderbestand, _wgpApparaat);
	WGPUShaderModule fragModule = _maakShaderModule(fragshaderbestand, _wgpApparaat);

	_shaderModules[shaderNaam] = { vertModule, fragModule };

	//De pipeline wordt pas bij de eerste weergave gemaakt (dan pas weten we de vertex-indeling)
	return nullptr;
}

WGPUComputePipeline weergaveScherm::maakRekenShader(const std::string & shaderNaam, const std::string & shaderbestand)
{
	WGPUShaderModule module = _maakShaderModule(shaderbestand, _wgpApparaat);

	//de reken-layout: vier opslag-buffers op binding 0..3
	if(!_rekenBindGroepLayout)
	{
		WGPUBindGroupLayoutEntry rekenInvoeren[4] = { WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT, WGPU_BIND_GROUP_LAYOUT_ENTRY_INIT };

		for(int i = 0; i < 4; i++)
		{
			rekenInvoeren[i].binding 	= i;
			rekenInvoeren[i].visibility = WGPUShaderStage_Compute;
			rekenInvoeren[i].buffer.type = WGPUBufferBindingType_Storage;
			rekenInvoeren[i].buffer.minBindingSize = 16;
		}

		WGPUBindGroupLayoutDescriptor layoutBeschrijving = WGPU_BIND_GROUP_LAYOUT_DESCRIPTOR_INIT;
		layoutBeschrijving.entryCount = 4;
		layoutBeschrijving.entries 		= rekenInvoeren;
		_rekenBindGroepLayout = wgpuDeviceCreateBindGroupLayout(_wgpApparaat, &layoutBeschrijving);

		WGPUBufferDescriptor legeBeschrijving = WGPU_BUFFER_DESCRIPTOR_INIT;
		legeBeschrijving.usage = WGPUBufferUsage_Storage | WGPUBufferUsage_CopyDst;
		legeBeschrijving.size = 16;
		_leegRekenBuffer = wgpuDeviceCreateBuffer(_wgpApparaat, &legeBeschrijving);
	}

	WGPUPipelineLayoutDescriptor layoutBeschrijving = WGPU_PIPELINE_LAYOUT_DESCRIPTOR_INIT;
	layoutBeschrijving.label 				= { shaderNaam.c_str(), shaderNaam.size() };
	layoutBeschrijving.bindGroupLayoutCount = 1;
	layoutBeschrijving.bindGroupLayouts 	= &_rekenBindGroepLayout;

	WGPUPipelineLayout layout = wgpuDeviceCreatePipelineLayout(_wgpApparaat, &layoutBeschrijving);

	WGPUComputeState rekenStaat = WGPU_COMPUTE_STATE_INIT;
	rekenStaat.module 		= module;
	rekenStaat.entryPoint 	= { "main", 4 };

	WGPUComputePipelineDescriptor beschrijving = WGPU_COMPUTE_PIPELINE_DESCRIPTOR_INIT;
	beschrijving.label 		= { shaderNaam.c_str(), shaderNaam.size() };
	beschrijving.layout 	= layout;
	beschrijving.compute 	= rekenStaat;

	WGPUComputePipeline programma = wgpuDeviceCreateComputePipeline(_wgpApparaat, &beschrijving);

	wgpuPipelineLayoutRelease(layout);

	if(!programma)
		throw std::runtime_error("Het maken van het reken-programma \"" + shaderNaam + "\" is mislukt!");

	_rekenProgrammas[shaderNaam] = programma;

	return programma;
}

WGPURenderPipeline weergaveScherm::slaShaderOp(const std::string & naam, WGPURenderPipeline shaderProgramma)
{
	_shaderProgrammas[naam] = shaderProgramma;

	return shaderProgramma;
}

WGPURenderPipeline weergaveScherm::zorgVoorProgramma(wrgvOpslag * puntReeks)
{
	if(_shaderProgrammas.count(_huidigProgrammaNaam))
		return _shaderProgrammas[_huidigProgrammaNaam];

	WGPURenderPipeline programma = _maakPipeline(_huidigProgrammaNaam, _huidigProgrammaNaam, puntReeks->vertexBufferLayouts(), puntReeks->topologie(), puntReeks->indexFormaat());

	slaShaderOp(_huidigProgrammaNaam, programma);

	return programma;
}

WGPURenderPipeline weergaveScherm::geefProgrammaHandvat(const std::string & naam) const
{
	if(_shaderProgrammas.count(naam) > 0)
		return _shaderProgrammas.at(naam);

	if(_shaderModules.count(naam) > 0)
		return nullptr; //bestaat nog niet en wordt pas bij de eerste weergave ge maakt

	throw std::runtime_error("Er wordt gepoogd het handvat op te vragen van een shader programma genaamd \"" + naam + "\" maar dat bestaat niet voor zover het scherm weet...");
}

WGPURenderPipeline weergaveScherm::geefEnigeProgrammaHandvat() const
{
	if(_shaderProgrammas.size() == 1)
		return _shaderProgrammas.begin()->second;

	if(_shaderModules.size() == 1)
		return nullptr;

	throw std::runtime_error("Er wordt gepoogd het enige maar er zijn er '"+ std::to_string(_shaderProgrammas.size())+"'...");
}

glm::ivec2 weergaveScherm::laadTextuurUitPng(const std::string & bestandsNaam, const std::string & textuurNaam, bool herhaalS, bool herhaalT, bool mipmap, unsigned int internalFormat, unsigned char ** imgData /*om png data terug te geven, zelf opruimen!*/)
{
	size_t breedte, hoogte, kanalen;
	png_byte * data = laadPNG(bestandsNaam, breedte, hoogte, kanalen);

	if(!data) 
		throw std::runtime_error("Could not load '" + bestandsNaam + "'!");

	maakTextuur(textuurNaam, breedte, hoogte, herhaalS, herhaalT, mipmap, internalFormat, data, GL_RGBA, GL_UNSIGNED_BYTE);
	
	if(!imgData)	delete data;
	else			*imgData = data;

	return glm::ivec2(breedte, hoogte);
}

WGPUTexture weergaveScherm::maakTextuur(const std::string & textuurNaam, size_t breedte, size_t hoogte, bool herhaalS, bool herhaalT, bool mipmap, unsigned int internalFormat, void * dataB, unsigned int format, unsigned int type)
{
	WGPUTextureFormat formaat = _geefWgpFormaat(internalFormat);

	WGPUExtent3D omvang = { (uint32_t)breedte, (uint32_t)hoogte, 1 };

	WGPUTextureDescriptor beschrijving = WGPU_TEXTURE_DESCRIPTOR_INIT;
	beschrijving.label 		= { textuurNaam.c_str(), textuurNaam.size() };
	beschrijving.usage 		= WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst | WGPUTextureUsage_RenderAttachment;
	beschrijving.dimension 	= WGPUTextureDimension_2D;
	beschrijving.format 	= formaat;
	beschrijving.size 		= omvang;
	beschrijving.mipLevelCount = 1;
	beschrijving.sampleCount 	= 1;

	WGPUTexture textuur = wgpuDeviceCreateTexture(_wgpApparaat, &beschrijving);

	_texturen[textuurNaam] 			= textuur;
	_textuurGroottes[textuurNaam] 	= glm::uvec2(breedte, hoogte);
	_3dTexturen.erase(textuurNaam);
	_1dTexturen.erase(textuurNaam);

	if(dataB)
		laadData(textuurNaam, breedte, hoogte, herhaalS, herhaalT, mipmap, internalFormat, dataB, format, type);

	(void)herhaalS; (void)herhaalT; (void)mipmap;

	wgpFoutControle("weergaveScherm::maakTextuur('" + textuurNaam + "'): ");

	return textuur;
}

void weergaveScherm::laadData(const std::string & textuurNaam, size_t breedte, size_t hoogte, bool herhaalS, bool herhaalT, bool mipmap, unsigned int internalFormat, void * dataB, unsigned int format, unsigned int type)
{
	if(!_texturen.count(textuurNaam))
	{
		maakTextuur(textuurNaam, breedte, hoogte, herhaalS, herhaalT, mipmap, internalFormat, dataB, format, type);
		return;
	}

	WGPUTextureFormat formaat = _geefWgpFormaat(internalFormat);

	_schrijfTextuurData(_texturen[textuurNaam], (uint32_t)breedte, (uint32_t)hoogte, 1, dataB, formaat);

	_textuurGroottes[textuurNaam] = glm::uvec2(breedte, hoogte);

	(void)herhaalS; (void)herhaalT; (void)mipmap; (void)format; (void)type;

	wgpFoutControle("weergaveScherm::laadData('" + textuurNaam + "'): ");
}

void weergaveScherm::_schrijfTextuurData(WGPUTexture textuur, uint32_t breedte, uint32_t hoogte, uint32_t diepte, const void * dataB, WGPUTextureFormat formaat)
{
	if(!dataB)
		return;

	uint32_t bytesPerPixel 	= _bytesPerPixel(formaat);
	uint32_t bytesPerRij 	= breedte * bytesPerPixel;

	WGPUTexelCopyTextureInfo bestemming = WGPU_TEXEL_COPY_TEXTURE_INFO_INIT;
	bestemming.texture 	= textuur;
	bestemming.aspect 	= WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout bron = WGPU_TEXEL_COPY_BUFFER_LAYOUT_INIT;
	bron.offset 		= 0;
	bron.bytesPerRow 	= bytesPerRij;
	bron.rowsPerImage 	= hoogte;

	WGPUExtent3D omvang = { breedte, hoogte, diepte };

	wgpuQueueWriteTexture(_wgpRij, &bestemming, dataB, (uint64_t)breedte * hoogte * diepte * bytesPerPixel, &bron, &omvang);
}

void weergaveScherm::bindTextuur(const std::string & textuurNaam, uint32_t actieveTextuur) const
{
	//In WebGPU wordt een textuur via bind-groep 1 in het shader-schema opgenomen;
	//we onthouden hier welke textuur de shader te zien krijgt
	(void)actieveTextuur;
	const_cast<weergaveScherm*>(this)->_gebondenTextuur = textuurNaam;
}

void weergaveScherm::bindTextuurPlaatje(const std::string & textuurNaam, uint32_t actieveTextuur, bool schrijven, bool lezen) const
{
	//de opslag-textuur (image load/store) is nog niet overgezet
	(void)textuurNaam; (void)actieveTextuur; (void)schrijven; (void)lezen;
}

void weergaveScherm::maakVolumeTextuur(const std::string & textuurNaam, glm::uvec3 dimensies, unsigned char * dataB)
{
	WGPUTextureFormat formaat = WGPUTextureFormat_RGBA8Unorm;

	WGPUExtent3D omvang = { dimensies.x, dimensies.y, dimensies.z };

	WGPUTextureDescriptor beschrijving = WGPU_TEXTURE_DESCRIPTOR_INIT;
	beschrijving.label 		= { textuurNaam.c_str(), textuurNaam.size() };
	beschrijving.usage 		= WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
	beschrijving.dimension 	= WGPUTextureDimension_3D;
	beschrijving.format 	= formaat;
	beschrijving.size 		= omvang;
	beschrijving.mipLevelCount = 1;
	beschrijving.sampleCount 	= 1;

	WGPUTexture textuur = wgpuDeviceCreateTexture(_wgpApparaat, &beschrijving);

	_schrijfTextuurData(textuur, dimensies.x, dimensies.y, dimensies.z, dataB, formaat);

	_texturen[textuurNaam] 		= textuur;
	_textuurGroottes[textuurNaam] = glm::uvec2(dimensies.x, dimensies.y);
	_3dTexturen.insert(textuurNaam);
	_1dTexturen.erase(textuurNaam);

	wgpFoutControle("weergaveScherm::maakVolumeTextuur('" + textuurNaam + "'): ");
}

void weergaveScherm::maakLijnTextuur(const std::string & textuurNaam, size_t lengte, unsigned int internalFormat, void * dataB, unsigned int dataFormat, unsigned int dataType)
{
	//WebGPU heeft geen 1D-texturen; een lijn wordt een 2D-textuur van (lengte x 1)
	WGPUTextureFormat formaat = _geefWgpFormaat(internalFormat);

	WGPUExtent3D omvang = { (uint32_t)lengte, 1, 1 };

	WGPUTextureDescriptor beschrijving = WGPU_TEXTURE_DESCRIPTOR_INIT;
	beschrijving.label 		= { textuurNaam.c_str(), textuurNaam.size() };
	beschrijving.usage 		= WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
	beschrijving.dimension 	= WGPUTextureDimension_2D;
	beschrijving.format 	= formaat;
	beschrijving.size 		= omvang;
	beschrijving.mipLevelCount = 1;
	beschrijving.sampleCount 	= 1;

	WGPUTexture textuur = wgpuDeviceCreateTexture(_wgpApparaat, &beschrijving);

	_schrijfTextuurData(textuur, (uint32_t)lengte, 1, 1, dataB, formaat);

	_texturen[textuurNaam] 		= textuur;
	_textuurGroottes[textuurNaam] = glm::uvec2(lengte, 1);
	_1dTexturen.insert(textuurNaam);
	_3dTexturen.erase(textuurNaam);

	(void)dataFormat; (void)dataType;

	wgpFoutControle("weergaveScherm::maakLijnTextuur('" + textuurNaam + "'): ");
}

WGPUTextureFormat weergaveScherm::_geefWgpFormaat(unsigned int internalFormat) const
{
	switch(internalFormat)
	{
	case GL_RGBA16:
	case GL_RGBA16F:		return WGPUTextureFormat_RGBA16Float;
	case GL_DEPTH_COMPONENT16:	return WGPUTextureFormat_Depth16Unorm;
	case GL_RGBA:
	case GL_RGBA8:
	default:				return WGPUTextureFormat_RGBA8Unorm;
	}
}

uint32_t weergaveScherm::_bytesPerPixel(WGPUTextureFormat formaat) const
{
	switch(formaat)
	{
	case WGPUTextureFormat_RGBA16Float:	return 8;
	case WGPUTextureFormat_Depth16Unorm:	return 2;
	case WGPUTextureFormat_RGBA8Unorm:
	default:							return 4;
	}
}