#pragma once
#include <webgpu.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <set>
#include <array>
#include <algorithm>
#include <functional>
#include "wrgvOnderOpslag.h"

	
const long double 	PI  = 3.141592653589793238L;
const float 		FPI = 3.141592653589793238f;


///Kleine hulp zodat code die nog GL-enum-waarden doorgeeft lekker blijft compileren
namespace wgpGL
{
	inline constexpr unsigned int GL_RGBA 				= 0x1908;
	inline constexpr unsigned int GL_RGBA8 				= 0x8058;
	inline constexpr unsigned int GL_RGBA16 			= 0x805B;
	inline constexpr unsigned int GL_RGBA16F 			= 0x881A;
	inline constexpr unsigned int GL_DEPTH_COMPONENT16 	= 0x81A5;
	inline constexpr unsigned int GL_UNSIGNED_BYTE 		= 0x1401;
	inline constexpr unsigned int GL_FLOAT 				= 0x1406;
	inline constexpr unsigned int GL_2_BYTES			= 0x1407;
}
using namespace wgpGL;


///Instellingen waarmee een render-pipeline gebouwd (of hergebruikt) wordt.
///Elke combinatie die afwijkt van de standaard krijgt een eigen pipeline-cache-sleutel.
struct weergaveInstellingen
{
	bool 				blenden 			= false;				///< alpha-blending (SRC_ALPHA / ONE_MINUS_SRC_ALPHA)
	bool 				diepteSchrijven 		= true;
	WGPUCullMode 		cullMode 			= WGPUCullMode_None;
	WGPUCompareFunction 	diepteVergelijk 	= WGPUCompareFunction_Less;
};


class weergaveScherm
{
public:
	typedef std::function<void(int key, int scancode, int action, int mods)> toetsVerwerkerFunc;

				weergaveScherm(std::string Naam = "weergaveScherm", size_t W = 1280, size_t H = 720, size_t multiSamples = 1, bool volledigScherm = true, bool hoofdloos = false);
				~weergaveScherm();

	void		bereidWeergevenVoor(const std::string & verwerker = "", bool wisScherm = true);
void		bereidRenderVoor(const std::string & verwerker = "", bool wisScherm = true) { bereidWeergevenVoor(verwerker, wisScherm); }
    void		_bereidWeergevenVoor(const std::string &shader, bool wisScherm, int breedte, int hoogte);
	void		laadOmgeving();
    void 		rondWeergevenAf();
void		rondRenderAf() { rondWeergevenAf(); }
	///Beëindigt de huidige weergave-pass maar houdt de commando-encoder en het
	///tekenoppervlak in leven, zodat er direct nog een volgende pass overheen
	///getekend kan worden. Rond ten slotte af met rondWeergevenAf (één submit).
    void		pasRondWeergevenAf();
void		pasRondRenderAf() { pasRondWeergevenAf(); }

	///Zet de instellingen voor de pipelines die de volgende weergave-pass bouwt
	void		zetWeergaveInstellingen(const weergaveInstellingen & instellingen) { _weergaveInstellingen = instellingen; }
	const weergaveInstellingen & geefWeergaveInstellingen() const { return _weergaveInstellingen; }

    bool		stopGewenst() { return _hoofdloos ? false : glfwWindowShouldClose(_glfwScherm); }

	///Of het wgpu-tekenoppervlak het laatst zichtbaar was (niet geOccludeerd).
	///Handig om de hoofdloop te stoppen zolang de planeet buiten beeld is.
	bool		oppervlakZichtbaar() const { return _hoofdloos ? false : _oppervlakZichtbaar; }
	///Blokkeert tot er een venster-gebeurtenis is (gebruiken wanneer het oppervlak
	///onvisible is; een visibil/size-event wekt de loop vanzelf weer op).
	void		wachtOpGebeurtenissen();

	static 	void toetsVerwerkerCentraal(GLFWwindow * scherm, 	int key, int scancode, int action, int mods);
	virtual void toetsVerwerker(								int key, int scancode, int action, int mods);
	static	void zetEigenToetsVerwerker(toetsVerwerkerFunc eigenVerwerker) { _eigenVerwerker = eigenVerwerker; }
void		setCustomKeyhandler(toetsVerwerkerFunc eigenVerwerker) { zetEigenToetsVerwerker(eigenVerwerker); }

	///WebGPU kent geen vlak-verdelings shaders; deze gooit dus een fout
	[[noreturn]] void maakVlakVerdelingsShader(const std::string & /*shaderNaam*/,		const std::string & /*vertshaderbestand*/, 	const std::string & /*fragshaderbestand*/, const std::string & /*vlakEvaluatieBestand*/, const std::string & /*vlakControleBestand*/ = "");
	///WebGPU kent geen geometry shaders; deze gooit dus een fout
	[[noreturn]] void maakGeometrieShader(const std::string & /*shaderNaam*/,		const std::string & /*vertshaderbestand*/, 	const std::string & /*fragshaderbestand*/, const std::string & /*geomshaderbestand*/);

	///Maakt het render-programma aan: de WGSL-modules worden direct gecompileerd, de pipeline pas bij de eerste weergave
	WGPURenderPipeline maakShader(const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand);

	///Maakt een reken-programma (compute) aan van een WGSL-bestand
	WGPUComputePipeline maakRekenShader(const std::string & shaderNaam,		const std::string &  shaderbestand);

	///Zorgt ervoor dat het programma (render pipeline) voor het huidige shader-programma bestaat
	WGPURenderPipeline 		zorgVoorProgramma(wrgvOpslag * puntReeks);

	///De nog lopen de weergave-pass (of null wanneer er geen is)
	WGPURenderPassEncoder 	weergavePass() const { return _weergavePass; }

	///De basis-bind-groep (schermparameters en matrices)
	WGPUBindGroup 			basisBindGroep() const { return _basisBindGroep; }

	///Schrijft de drie bekende matrices naar de matrix-buffer (bind-groep 0, binding 1)
	void zetMatrices(const glm::mat4 & projectie, const glm::mat4 & modelZicht, const glm::mat4 & transInvMV);

	///Zet een extra float op een plek in de extra-buffer (bind-groep 0, binding 2)
	void zetExtraFloat(int plek, float waarde);

	///Zet meerdere extra floats in de extra-buffer (max 16)
	void zetExtraFloats(const float * waarden, int aantal);

	///Laat de volgende bereidWeergevenVoor naar de gegeven textuur tekenen (off-screen)
	void zetWeergaveDoel(WGPUTexture textuur, glm::uvec2 grootte = glm::uvec2(1, 1));

	///Registreert een opslag-buffer voor het reken (compute) programma (binding 0..3)
	void verbindRekenBuffer(uint32_t binding, WGPUBuffer buffer);

	///Maakt de geregistreerde reken-buffers weer leeg
	void ontkoppelRekenBuffers() { _rekenBufferBinden.fill(nullptr); }

	///Zorgt dat het programma voor de huidige shader bestaat met een puntjes-indeling (vec4 op locatie 0)
	WGPURenderPipeline zorgVoorPuntenProgramma();

	///Tekent een ruwe buffer als puntjes (voor de schaap/wolf demo) in de huidige pass
	void tekenAlsPunten(WGPUBuffer buffer, uint32_t puntAantal);

	///Tekent vierkantjes (instanced) op de plekken uit een opslag-buffer
	void tekenKadertjes(WGPUBuffer vierkantje, uint32_t vierkantPunten, WGPUBuffer plekken, uint32_t aantalPlekken);

	///Bindt de actief gebonden textuur aan bind-groep 1 van de pass
	void _bindTextuurAanPass(WGPURenderPassEncoder pass);

	///Bindt de geregistreerde opslag-buffers aan bind-groep 2 van de pass
	void _bindOpslagAanPass(WGPURenderPassEncoder pass);

	WGPURenderPipeline geefProgrammaHandvat(const std::string & naam) 	const;
	WGPURenderPipeline geefEnigeProgrammaHandvat() 					const;
	WGPURenderPipeline huidigProgramma()								const { return _huidigProgramma; }

	///Apparaat en rij die gedeeld worden met de rest van de bibliotheek
	static WGPUDevice 	deelApparaat() 		{ return gedeeldApparaat(); }
	static WGPUQueue 	deelRij() 			{ return gedeeldeRij(); 		}

	WGPUInstance		instantie()			const { return _wgpInstantie; }

	glm::ivec2 	laadTextuurUitPng(	const std::string & bestandsNaam, const std::string & textuurNaam,  bool herhaalS = true, bool herhaalT = true, bool mipmap = true, unsigned int internalFormat=GL_RGBA, unsigned char ** imgData = nullptr);
	void 		bindTextuur(		const std::string & textuurNaam, uint32_t actieveTextuur) const;
	void 		bindTextuurPlaatje(	const std::string & textuurNaam, uint32_t actieveTextuur, bool schrijven = true, bool lezen = true) const;
	void		maakVolumeTextuur(	const std::string & textuurNaam, glm::uvec3 dimensies, unsigned char * dataB = nullptr);
	void		maakLijnTextuur(	const std::string & textuurNaam, size_t lengte, unsigned int internalFormat =  GL_RGBA16 , void * dataB = nullptr, unsigned int dataFormat = GL_RGBA, unsigned int dataType = GL_FLOAT);
	WGPUTexture	maakTextuur(		const std::string & textuurNaam, size_t breedte, size_t hoogte, bool herhaalS = false, bool herhaalT = false, bool mipmap = false, unsigned int internalFormat = GL_RGBA, void * dataB = nullptr, unsigned int dataFormat = GL_RGBA, unsigned int dataType = GL_UNSIGNED_BYTE);
	void		laadData(			const std::string & textuurNaam, size_t breedte, size_t hoogte, bool herhaalS = false, bool herhaalT = false, bool mipmap = false, unsigned int internalFormat = GL_RGBA, void * dataB = nullptr, unsigned int dataFormat = GL_RGBA, unsigned int dataType = GL_UNSIGNED_BYTE);
	glm::uvec2	textuurGrootte(		const std::string & textuurNaam) { return _textuurGroottes.at(textuurNaam); }
	WGPUTexture	textuurId(			const std::string & textuurNaam) { return _texturen.at(textuurNaam);		}

	void		doeRekenVerwerker(const std::string & verwerker, glm::uvec3 groepGroottes, std::function<void()> renderVoorbereiding);

	///Kan gebruikt worden door subklasses om nog extra dingen voor te bereiden (uniform buffers bijvullen!)
	virtual void extraVoorbereidingen(WGPURenderPipeline bangsaProgramma) { }

	void		zetWeergaveKleur(float rood, float groen, float blauw, float alfa = 1.0f) { _weergaveKleur[0] = rood; _weergaveKleur[1] = groen; _weergaveKleur[2] = blauw; _weergaveKleur[3] = alfa; }

protected:
	float					_schermVerhouding = 16.0f / 9.0f;
	std::string				_naam;
	bool					_hoofdloos		= false;
	WGPURenderPipeline		_huidigProgramma;
	std::string				_huidigProgrammaNaam;

	///De nog lopen de weergave: command-encoder en de render-pass
	WGPUCommandEncoder 		_commandEncoder		= nullptr;
	WGPURenderPassEncoder 	_weergavePass		= nullptr;

	WGPUDevice 				_wgpApparaat 		= nullptr;
	WGPUQueue 				_wgpRij 			= nullptr;
	WGPUTextureFormat 		_oppervlakFormaat	= WGPUTextureFormat_Undefined;

	WGPUBuffer 				_beeldBuffer 		= nullptr; ///< schermBreedte/Hoogte/Verhouding (binding 0)
	WGPUBuffer 				_matrixBuffer 		= nullptr; ///< projectie/modelView/transInvMV (binding 1)
	WGPUBuffer 				_extraBuffer 		= nullptr; ///< losse parameters voor de demo's (binding 2)

	WGPUBindGroupLayout 	_basisBindGroepLayout 	= nullptr;
	WGPUBindGroup 			_basisBindGroep 		= nullptr;
	WGPUBindGroupLayout 	_matrixBindGroepLayout 	= nullptr;
	WGPUBindGroup 			_matrixBindGroep 		= nullptr;
WGPUBindGroupLayout 		_textuurBindGroepLayout	= nullptr;
	WGPUBindGroup 			_textuurBindGroep 		= nullptr;
	WGPUSampler 			_sampler 				= nullptr;
	WGPUTexture 			_witteTextuur 			= nullptr;	///< 1x1 wit hulpje voor de lege textuur-bind-groep
	WGPUBindGroup 			_witteBindGroep 		= nullptr;	///< bind-groep 1 zonder echte textuur

	WGPUTexture 			_doelTextuur 			= nullptr; ///< off-screen doel
	WGPUTextureFormat 		_doelFormaat 			= WGPUTextureFormat_RGBA8Unorm; ///< formaat van het off-screen doel
	glm::uvec2 				_doelGrootte 			= glm::uvec2(1, 1);			///< grootte van het off-screen doel
	WGPUBindGroupLayout 	_rekenBindGroepLayout 		= nullptr; ///< layout voor opslag-buffers in reken-shaders (read_write)
	WGPUBindGroupLayout 	_renderOpslagBindGroepLayout = nullptr; ///< layout voor opslag-buffers in weergave-shaders (read)
	WGPUBuffer 				_leegRekenBuffer 		= nullptr; ///< opvul-buffer voor ongebruikte opslag-bindings

private:
	WGPURenderPipeline slaShaderOp(const std::string & naam, WGPURenderPipeline shaderProgramma);

	GLFWwindow 			*	_glfwScherm 	= nullptr;
	void 				*	_metaalLaag 	= nullptr;

	WGPUInstance 			_wgpInstantie 	= nullptr;
	WGPUAdapter 			_wgpAdapter 	= nullptr;
	WGPUSurface 			_wgpOppervlak 	= nullptr;

	uint32_t 				_oppervlakBreedte 	= 0,
							_oppervlakHoogte 	= 0;

	bool 					_oppervlakZichtbaar = true;	///< of het oppervlak het laatst zichtbaar was (niet geOccludeerd)

	WGPUTexture 			_oppervlakTextuur 	= nullptr;
	WGPUTextureView 		_oppervlakZicht 	= nullptr;

	WGPUTexture 			_diepteTextuur 		= nullptr;	///< de z-buffer
	WGPUTextureView 		_diepteZicht 		= nullptr;
	uint32_t 				_diepteBreedte		= 0,
							_diepteHoogte		= 0;

	float 					_weergaveKleur[4] 	= { 0.0f, 0.0f, 0.0f, 1.0f };

	weergaveInstellingen 	_weergaveInstellingen;

	std::vector<WGPUBindGroup> _gevormdeBindGroepen;

	std::map<std::string, std::pair<WGPUShaderModule, WGPUShaderModule>> 	_shaderModules;
	std::map<std::string, WGPURenderPipeline>								_shaderProgrammas;
	std::map<std::string, WGPUComputePipeline>								_rekenProgrammas;
	std::map<std::string, WGPUTexture>										_texturen;
	std::vector<WGPUBindGroup>												_rekenBindGroepen;	///< al gevormde reken-bind-groepen, netjes opruimen
	std::array<WGPUBuffer, 4>												_rekenBufferBinden = { nullptr, nullptr, nullptr, nullptr };	///< geregistreerde opslag-buffers voor het reken-programma
	std::map<std::string, glm::uvec2>										_textuurGroottes;
	std::map<std::string, WGPUBindGroup>									_textuurBindGroepen;///< per textuur een bind-groep voor groep 1
	std::string																_gebondenTextuur;	///< de textuur die aan bind-groep 1 hangt
	std::set<std::string>													_3dTexturen,
																			_1dTexturen;
	static std::map<GLFWwindow *, weergaveScherm*>							_schermen;

	static toetsVerwerkerFunc _eigenVerwerker;

	void		_configureerOppervlak(uint32_t breedte, uint32_t hoogte);
	void		_beeldenUniformen(int breedte, int hoogte);
	void		_schrijfTextuurData(WGPUTexture textuur, uint32_t breedte, uint32_t hoogte, uint32_t diepte, const void * dataB, WGPUTextureFormat formaat);
	WGPUTextureFormat _geefWgpFormaat(unsigned int internalFormat) const;
	uint32_t	_bytesPerPixel(WGPUTextureFormat formaat) const;
	WGPUAdapter 	_vraagAdapter();
	WGPUDevice		_vraagApparaat();

	///Maakt een render-pipeline met de gegeven vertex-indeling en topologie
	WGPURenderPipeline _maakPipeline(const std::string & programmaNaam, const std::string & moduleNaam, const std::vector<WGPUVertexBufferLayout> & vertexLayouts, WGPUPrimitiveTopology topologie, WGPUIndexFormat stripFormaat);

	///Maakt de (z-buffer) diepte-textuur aan als die er nog niet is of te klein is
	void _zorgDiepteTextuur(uint32_t breedte, uint32_t hoogte);

	///Vraagt (of maakt) de bind-groep voor een textuur (groep 1)
	WGPUBindGroup _bindgroepVoorTextuur(const std::string & textuurNaam);

	///Zorgt dat de opslag-bind-groep-layouts bestaan (reken èn weergave)
	void		_zorgOpslagBindGroep();

	///Maakt een bind-groep van de geregistreerde opslag-buffers (met lege opvulling)
	WGPUBindGroup _maakOpslagBindGroep(WGPUBindGroupLayout layout);

	///Achtervoegsels voor de pipeline-cache-sleutel op basis van de instellingen
	std::string _instellingenSleutel() const;
};