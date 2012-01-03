#ifndef DATAPREPROCESSING_HPP
#define DATAPREPROCESSING_HPP

#include "database.hpp"
#include "temporarydatabase.hpp"
#include "blockingqueue.hpp"
#include "routingedge.hpp"
#include "osmparser.hpp"

/**
 * @brief Diese Klasse kuemmert sich um jegliche Form der Datenvorverarbeitung
 * 
 * @ingroup preprocessing
 * @author Sebastian Koehlert
 * @date 2011-12-21
 * @copyright GNU GPL v3
 * @todo erstmal noch ein todo schreiben ;)
 */
 
class DataPreprocessing
{
	private:

	public:
    DataPreprocessing();
    ~DataPreprocessing();
    //
	//TODO: sinnvolle Kategorien definieren
	enum Category
	{
	  cat1= 0,
	  cat2,
	  cat3,
	  catn
	};
	
	bool feedParser();
	
	/**
	* @brief Gibt die Kategorie der als Parameter übergebenen Kante aus
	* @param edgeID Die Kante, dessen Kategorie ausgegeben werden soll.
	* @return Kategorie der Kante.
	*/
	Category getCategoryEdgeID(boost::uint64_t edgeID);
};
#endif //DATAPREPROCESSING_HPP
