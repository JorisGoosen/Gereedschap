#include "../vrwrkrOpslagDing.h"

struct dier
{
	dier();

	float	heteroKracht,	///< Hoe sterk is dit wezen aangetrokken tot de andere? Wolf -> Schaap -> Wolf
			homoKracht,		///< Hoe sterk is dit wezen aangetrokken tot de zelfde? Wolf <> Wolf en Schaap <> Schaap
			honger;			///< Als honger > 100 sterft dit dier, als honger > 50 komt begint de snelheid van het dier af te nemen.
};

class weergaveScherm;

class dieren
{
public:
	dieren(int wolven = 10, int schapen = 200, float wereldGrootte = 100);

	void bindDingen(weergaveScherm * scherm, int programma, bool pong = false);
	
protected:
	std::vector<vrwrkrOpslagDing<dier>*>	_wolven [2],	///< Ping pong met wolven
											_schapen[2];	///< Ping pong met schapen
	float									_wereldGrootte;
	int										_pingPong,
											_aantalWolven,	
											_aantalSchapen;
};