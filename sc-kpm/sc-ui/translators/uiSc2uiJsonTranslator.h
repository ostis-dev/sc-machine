//
// Created by irina on 9.11.20.
//

#ifndef SC_MACHINE_UISC2UIJSONTRANSLATTOR_H
#define SC_MACHINE_UISC2UIJSONTRANSLATTOR_H



#include "uiTranslatorFromSc.h"

class uiSc2uiJsonTranslator : public uiTranslateFromSc
{
public:
    explicit uiSc2uiJsonTranslator();
    virtual ~uiSc2uiJsonTranslator();



    static sc_result ui_translate_sc2ui_json(const sc_event *event, sc_addr arg);

protected:
    //! @copydoc uiTranslateFromSc::runImpl
    void runImpl();

private:

    void get_idtf(String &sys_idtf, sc_stream *&idtf_stream, sc_uint32 &idtf_length, sc_uint32 &read_bytes,
                  sc_char *buffer, const sc_addr &link) const;

    void
    getContent(StringStream &ss, String &sys_idtf, sc_stream *idtf_stream, sc_uint32 idtf_length, sc_uint32 read_bytes,
               sc_char *buffer, const sc_addr &button,String text,sc_addr keynode_nrel_content);

};

#endif //SC_MACHINE_UISC2UIJSONTRANSLATTOR_H