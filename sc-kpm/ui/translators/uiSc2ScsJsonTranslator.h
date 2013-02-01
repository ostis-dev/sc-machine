/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2012 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#ifndef _ui_translator_sc2scs_h_
#define _ui_translator_sc2scs_h_

#include "uiTranslatorFromSc.h"

/*!
 * \brief Class that translates sc-construction into
 * SCs-code.
 */
class uiSc2ScsTranslator : public uiTranslateFromSc
{
public:
    explicit uiSc2ScsTranslator();
    virtual ~uiSc2ScsTranslator();

    static sc_result ui_translate_sc2scs(sc_event *event, sc_addr arg);

protected:
    //! @copydoc uiTranslateFromSc::runImpl
    void runImpl();

    //! Resolve system identifier for specified sc-addr
    void resolveSystemIdentifier(const sc_addr &addr, String &idtf);

protected:
    //! Map of resolved system identifiers
    typedef std::map<sc_addr, String> tSystemIdentifiersMap;
    tSystemIdentifiersMap mSystemIdentifiers;

    typedef std::map<sc_type, String> tScTypeToSCsConnectorMap;
    tScTypeToSCsConnectorMap mTypeToConnector;

};

#endif
