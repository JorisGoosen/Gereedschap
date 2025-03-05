#include "../vrwrkrOpslagDing.h"

struct Dier
{
	Dier();

	float	heteroKracht,	///< Hoe sterk is dit wezen aangetrokken tot de andere? Wolf -> Schaap -> Wolf
			homoKracht,		///< Hoe sterk is dit wezen aangetrokken tot de zelfde? Wolf <> Wolf en Schaap <> Schaap
			honger,			///< Als honger > 100 sterft dit Dier, als honger > 50 komt begint de snelheid van het Dier af te nemen.
			levend;			///< Als !0 leeft het Dier
};

struct PlaatsKleur
{
	PlaatsKleur();

	float	posX,
			posY,
			lichtheid,
			roodheid;
};

class weergaveScherm;
class nepScherm;
class vierkantRooster;

class Dieren
{
public:
	Dieren(weergaveScherm * scherm, int wolven = 10, int schapen = 200, float wereldGrootte = 2);

	void pong();
	void beweeg(bool wolven);

	void teken(bool wolven);
	
protected:
	std::vector<vrwrkrOpslagDing<Dier>*>		_wolvenE ,	///< Ping pong met wolven, eigenschappen
												_schapenE;	///< Ping pong met schapen, eigenschappen
	std::vector<vrwrkrOpslagDing<PlaatsKleur>*>	_wolvenP ,	///< Ping pong met wolven, positie, mss beter vertex arary ofzo?
												_schapenP;	///< Ping pong met schapen, positie
	//vierkantRooster 						*	_vierkant;
	weergaveScherm							*	_scherm;
	float										_wereldGrootte;
	int											_pingPong,
												_aantalWolven,	
												_aantalSchapen;
};