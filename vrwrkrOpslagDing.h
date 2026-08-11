#pragma once
#include "helpers.h"
#include <map>
#include <list>
#include <vector>
#include <algorithm>

//gedeeld apparaat/rij worden geregeld door het weergaveScherm (definities in weergaveScherm.cpp)
WGPUDevice gedeeldApparaat();
WGPUQueue  gedeeldeRij();

///Houdt een opslag in WebGPU: een buffer die als storage (=reken/shader storage) gebruikt kan worden
template <class Soort>
class vrwrkrOpslagDing
{
public:
	vrwrkrOpslagDing(const std::vector<Soort> & gegevens, uint32_t knooppunt = 0) 
	{
		_apparaat 	= gedeeldApparaat();
		_rij 		= gedeeldeRij();

		_opslag = _maakBuffer(gegevens.size() * sizeof(Soort));

		if(!gegevens.empty())
			wgpuQueueWriteBuffer(_rij, _opslag, 0, gegevens.data(), gegevens.size() * sizeof(Soort));

		_knooppunt = knooppunt;
		_aantalVierTupels = gegevens.size();
	}

	~vrwrkrOpslagDing()
	{
		if(_opslag)
			wgpuBufferRelease(_opslag);
	}

	///De plek (binding) waar de buffer in het reken-schema te vinden is
	void	zetKnooppunt(uint32_t knooppunt)
	{
		_knooppunt = knooppunt;
	}

	uint32_t 	knooppunt()					const { return _knooppunt; }
	WGPUBuffer	opslag()					const { return _opslag; 	}
	uint64_t	aantalVierTupels()			const { return _aantalVierTupels; }

	//voormalig: vertex reeks maken van de buffer (voor punten tekenen); 
	void 	maakReeksOpslag()
	{
		//In WebGPU kan de buffer direct als vertex-buffer worden gebruikt,
		//dus dit hoeft niet meer te gebeuren.
	}

	void bindPuntReeks()
	{
		//in WebGPU doet het niets
	}

private:
	WGPUBuffer _maakBuffer(uint64_t grootte)
	{
		if(grootte == 0)
			grootte = sizeof(Soort);

		WGPUBufferDescriptor beschrijving = WGPU_BUFFER_DESCRIPTOR_INIT;
		beschrijving.usage 	= WGPUBufferUsage_Storage | WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc;
		beschrijving.size 	= grootte;

		return wgpuDeviceCreateBuffer(_apparaat, &beschrijving);
	}

	WGPUDevice 	_apparaat 	= nullptr;
	WGPUQueue 	_rij 		= nullptr;

	WGPUBuffer 	_opslag 	= nullptr;
	uint32_t 	_knooppunt 	= 0;
	uint64_t 	_aantalVierTupels = 0;
};