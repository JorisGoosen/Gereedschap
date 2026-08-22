#pragma once
#include "helpers.h"
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <type_traits>

///Ledig gedeeld apparaat: wordt gevuld door het weergaveScherm zodra dat een apparaat heeft
WGPUDevice 	gedeeldApparaat();
WGPUQueue	gedeeldeRij();

///Hoeveel bytes bevat één element van dit type?
template <typename FT>	inline uint32_t bytesPer			  () { return 4; }
template <> 			inline uint32_t bytesPer< char		> () { return 1; }
template <> 			inline uint32_t bytesPer< short		> () { return 2; }
template <> 			inline uint32_t bytesPer< uint8_t	> () { return 1; }
template <> 			inline uint32_t bytesPer< int32_t	> () { return 4; }
template <> 			inline uint32_t bytesPer< uint16_t	> () { return 2; }
template <> 			inline uint32_t bytesPer< uint32_t	> () { return 4; }

///Bevat de benodigde informatie voor een punt-eigenschap-plek (vertex attribute location) binnen een puntreeks (vertex array)
struct puntEigenschapPlekInfo
{
	puntEigenschapPlekInfo(WGPUBuffer reeksOpslag = nullptr, uint32_t puntEigenschapPlek = 0, uint32_t aantalVelden = 0) :
		reeksOpslag			(reeksOpslag),
		puntEigenschapPlek	(puntEigenschapPlek),
		aantalVelden		(aantalVelden)
		{}

	WGPUBuffer 	reeksOpslag;
	uint32_t 	puntEigenschapPlek, 
				aantalVelden;
	WGPUVertexFormat 	formaat 	= WGPUVertexFormat_Float32;
	uint64_t 			stap 		= 0,	///< stap in eenheden (stride)
						wijzer  	= 0;	///< wijzer in eenheden (offset)
	uint32_t 			reeksIndex 	= 0;	///< welk vertex-buffer (slot)
};

///Stap is in het engels ook wel stride, wijzer pointer en plek in dit specifieke geval een Vertex Attribute Location
struct stapWijzer
{
	stapWijzer(uint32_t plek = 0, size_t stap = 0, size_t wijzer = 0) 
	: plek(plek), stap(stap), wijzer(wijzer) {}

	uint32_t 	plek;
	size_t		stap,
				wijzer;
};

class wrgvOpslag
{
public:
	wrgvOpslag()
	{
		_apparaat 	= gedeeldApparaat();
		_rij 		= gedeeldeRij();
	}

	~wrgvOpslag();

	WGPUBuffer reeksBufferVoorPlek(uint32_t plek)	{ return _plekInfos.at(plek).reeksOpslag; }

	template <typename veldType> WGPUBuffer reeksOpslagErbij(int aantalVelden, std::vector<veldType> & ggvns, std::vector<stapWijzer> stapWijzers)
	{
		const uint32_t eenheid = bytesPer<veldType>();

		uint64_t grootte = ggvns.size() * eenheid;

		WGPUBuffer reeksOpslag = _maakBuffer(grootte, WGPUBufferUsage_Vertex);

		if(grootte > 0)
			wgpuQueueWriteBuffer(_rij, reeksOpslag, 0, ggvns.data(), grootte);

		uint32_t reeksIndex = (uint32_t)_vertexBuffers.size();
		_vertexBuffers.push_back(reeksOpslag);

		_totaalPunten = ggvns.size() / aantalVelden;

		for(const stapWijzer & stapW : stapWijzers)
		{
			WGPUVertexFormat formaat = _krijgFormaat<veldType>(aantalVelden);

			puntEigenschapPlekInfo info(reeksOpslag, stapW.plek, aantalVelden);
			info.formaat 	= formaat;
			info.stap 		= stapW.stap == 0 ? aantalVelden * eenheid : stapW.stap * eenheid;
			info.wijzer 	= stapW.wijzer * eenheid;
			info.reeksIndex = reeksIndex;

			_plekInfos[stapW.plek] = info;
		}

		_herschikLayouts();

		return reeksOpslag;
	}

	template <typename veldType> WGPUBuffer reeksOpslagErbij(int aantalVelden, std::vector<veldType> & ggvns, uint32_t puntEigenschapPlek)
	{
		return reeksOpslagErbij(aantalVelden, ggvns, { stapWijzer(puntEigenschapPlek) } );
	}

	template <typename veldType> void koppelNieuweGegevens(int plek, std::vector<veldType> & ggvns)
	{
		const uint32_t eenheid = bytesPer<veldType>();
		uint64_t grootte = ggvns.size() * eenheid;

		puntEigenschapPlekInfo & info = _plekInfos[plek];

		//Mocht de buffer te klein zijn geworden moet er een nieuwe buffer komen
		if(grootte > wgpuBufferGetSize(info.reeksOpslag))
		{
			WGPUBuffer nieuwe = _maakBuffer(grootte == 0 ? eenheid : grootte, WGPUBufferUsage_Vertex);
			//De oude buffer loslaten vóórdat we hem vervangen (groeien gebeurt
			//alleen tijdens de opbouw van de geometrie, nooit midden in een frame).
			if(info.reeksOpslag)
				wgpuBufferRelease(info.reeksOpslag);
			info.reeksOpslag = nieuwe;
			_vertexBuffers[info.reeksIndex] = nieuwe;
		}

		if(grootte > 0)
			wgpuQueueWriteBuffer(_rij, info.reeksOpslag, 0, ggvns.data(), grootte);
	}

	///In WebGPU hoef je geen punt-reeks te binden; dit blijft hier bestaan zodat bestaande code gewoon compileert
	void bindPuntReeks() { }

	template <typename indexType> void zetIndexGegevens(const std::vector<indexType> & indices)
	{
		uint64_t grootte = indices.size() * bytesPer<indexType>();

		if(_indexBuffer) wgpuBufferRelease(_indexBuffer);

		_indexBuffer 	= _maakBuffer(grootte, WGPUBufferUsage_Index);
		_aantalIndices 	= indices.size();
		_indexFormaat 	= sizeof(indexType) == 2 ? WGPUIndexFormat_Uint16 : WGPUIndexFormat_Uint32;

		if(grootte > 0)
			wgpuQueueWriteBuffer(_rij, _indexBuffer, 0, indices.data(), grootte);
	}

	///Bindt alle vertex-buffers en (als die er is) de index-buffer aan de weergave-pass
	void bindVoorWeergave(WGPURenderPassEncoder weergavePass);

	///Teken deze puntreeks in de huidige weergave-pass van het (huidige) weergaveScherm.
	///Zorgt ervoor dat de pipeline bestaat, bindt buffers en doet de draw-vraag.
	void tekenGeïndexeerd();

	void		zetTopologie(WGPUPrimitiveTopology topologie) 		{ _topologie = topologie; }
	WGPUPrimitiveTopology topologie()						const	{ return _topologie; 		}

	uint64_t 	aantalIndices()									const	{ return _aantalIndices; 	}
	uint64_t 	aantalVertices()								const	{ return _totaalPunten; 	}
	WGPUBuffer	indexBuffer()									const	{ return _indexBuffer; 		}
	WGPUIndexFormat indexFormaat()								const	{ return _indexFormaat; 	}

	const std::vector<WGPUVertexBufferLayout> & vertexBufferLayouts()	const { return _vertexBufferLayouts; }
	const std::vector<WGPUBuffer> &				vertexBuffers() 		const { return _vertexBuffers; 	}

private:
	template <typename veldType> WGPUVertexFormat _krijgFormaat(int aantalVelden)
	{
		if constexpr (std::is_same_v<veldType, float>)
		{
			switch(aantalVelden)
			{
			case 1: return WGPUVertexFormat_Float32;
			case 2: return WGPUVertexFormat_Float32x2;
			case 3: return WGPUVertexFormat_Float32x3;
			default: return WGPUVertexFormat_Float32x4;
			}
		}
		else if constexpr (std::is_same_v<veldType, int32_t>)
		{
			switch(aantalVelden)
			{
			case 1: return WGPUVertexFormat_Sint32;
			case 2: return WGPUVertexFormat_Sint32x2;
			case 3: return WGPUVertexFormat_Sint32x3;
			default: return WGPUVertexFormat_Sint32x4;
			}
		}
		else if constexpr (std::is_same_v<veldType, uint32_t>)
		{
			switch(aantalVelden)
			{
			case 1: return WGPUVertexFormat_Uint32;
			case 2: return WGPUVertexFormat_Uint32x2;
			case 3: return WGPUVertexFormat_Uint32x3;
			default: return WGPUVertexFormat_Uint32x4;
			}
		}
		else if constexpr (std::is_same_v<veldType, uint8_t>)
		{
			return aantalVelden >= 4 ? WGPUVertexFormat_Unorm8x4 : WGPUVertexFormat_Unorm8x2;
		}
		else if constexpr (std::is_same_v<veldType, uint16_t>)
		{
			return aantalVelden >= 3 ? WGPUVertexFormat_Uint16x4 : WGPUVertexFormat_Uint16x2;
		}

		//alles anders schuift maar door als float
		return WGPUVertexFormat_Float32x4;
	}

	WGPUBuffer _maakBuffer(uint64_t grootte, WGPUBufferUsage gebruiksSoort);
	void 		_herschikLayouts();

	/// puntEigenschapPlek -> puntEigenschapPlekInfo
	std::map<uint32_t, puntEigenschapPlekInfo> 		_plekInfos;	

	WGPUDevice 				_apparaat 	= nullptr;
	WGPUQueue 				_rij 		= nullptr;

	std::vector<WGPUBuffer>							_vertexBuffers;
	std::vector<std::vector<WGPUVertexAttribute>> 	_attributenPerReeks;
	std::vector<WGPUVertexBufferLayout> 			_vertexBufferLayouts;

	WGPUBuffer 				_indexBuffer 	= nullptr;
	uint64_t 				_aantalIndices 	= 0;
	uint64_t 				_totaalPunten	= 0;
	WGPUIndexFormat 		_indexFormaat 	= WGPUIndexFormat_Uint32;
	WGPUPrimitiveTopology 	_topologie 		= WGPUPrimitiveTopology_TriangleList;
};


inline wrgvOpslag::~wrgvOpslag()
{
	for(WGPUBuffer buffer : _vertexBuffers)
		wgpuBufferRelease(buffer);

	if(_indexBuffer)
		wgpuBufferRelease(_indexBuffer);
}

inline WGPUBuffer wrgvOpslag::_maakBuffer(uint64_t grootte, WGPUBufferUsage gebruiksSoort)
{
	if(grootte == 0)
		grootte = 1;

	WGPUBufferDescriptor beschrijving = WGPU_BUFFER_DESCRIPTOR_INIT;
	beschrijving.usage  	= gebruiksSoort | WGPUBufferUsage_CopyDst;
	beschrijving.size   	= grootte;

	return wgpuDeviceCreateBuffer(_apparaat, &beschrijving);
}

inline void wrgvOpslag::_herschikLayouts()
{
	_attributenPerReeks.clear();
	_attributenPerReeks.resize(_vertexBuffers.size());
	_vertexBufferLayouts.clear();

	for(const auto & [plek, info] : _plekInfos)
	{
		WGPUVertexAttribute attrib{};
		attrib.format 			= info.formaat;
		attrib.offset 			= info.wijzer;
		attrib.shaderLocation 	= info.puntEigenschapPlek;

		_attributenPerReeks[info.reeksIndex].push_back(attrib);
	}

	for(size_t i=0; i<_vertexBuffers.size(); i++)
	{
		if(_attributenPerReeks[i].empty())
			continue;

		uint64_t stap = 0;

		for(const auto & [plek, info] : _plekInfos)
		{
			if(info.reeksIndex == i)
			{
				stap = info.stap;
				break;
			}
		}

		WGPUVertexBufferLayout layout{};
		layout.arrayStride 		= stap;
		layout.stepMode 		= WGPUVertexStepMode_Vertex;
		layout.attributeCount 	= _attributenPerReeks[i].size();
		layout.attributes 		= _attributenPerReeks[i].data();

		_vertexBufferLayouts.push_back(layout);
	}
}

inline void wrgvOpslag::bindVoorWeergave(WGPURenderPassEncoder weergavePass)
{
	for(uint32_t slot = 0; slot < _vertexBuffers.size(); slot++)
		wgpuRenderPassEncoderSetVertexBuffer(weergavePass, slot, _vertexBuffers[slot], 0, wgpuBufferGetSize(_vertexBuffers[slot]));

	if(_indexBuffer)
		wgpuRenderPassEncoderSetIndexBuffer(weergavePass, _indexBuffer, _indexFormaat, 0, wgpuBufferGetSize(_indexBuffer));
}