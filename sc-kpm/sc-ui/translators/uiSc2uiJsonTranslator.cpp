//
// Created by irina on 9.11.20.
//

#include <sc-memory/utils/sc_log.hpp>

#include "uiSc2uiJsonTranslator.h"

#include "uiKeynodes.h"
#include "uiTranslators.h"
#include "uiUtils.h"


uiSc2uiJsonTranslator::uiSc2uiJsonTranslator() {
}

uiSc2uiJsonTranslator::~uiSc2uiJsonTranslator() {

}

void uiSc2uiJsonTranslator::runImpl() {
    StringStream ss;
    String sys_idtf = "";
    sc_stream *idtf_stream = 0;
    sc_uint32 idtf_length = 0;
    sc_uint32 read_bytes = 0;
    sc_char buffer[32];
    ss << "[{";
    sc_iterator5 *it5 = sc_iterator5_a_a_f_a_f_new(s_default_ctx,
                                                   sc_type_node | sc_type_const,
                                                   sc_type_arc_common | sc_type_const,
                                                   mInputConstructionAddr,
                                                   sc_type_arc_pos_const_perm,
                                                   keynode_question_nrel_answer);
    if (sc_iterator5_next(it5) == SC_TRUE) {
        sc_iterator3 *it3 = sc_iterator3_f_a_a_new(s_default_ctx,

                                                   sc_iterator5_value(it5, 0),
                                                   sc_type_arc_pos_const_perm,
                                                   0);
        while (sc_iterator3_next(it3) == SC_TRUE) {

            sc_addr button = sc_iterator3_value(it3, 2);

            sc_iterator3 *it = sc_iterator3_f_a_f_new(s_default_ctx,
                                                      keynode_concept_button,
                                                      sc_type_arc_pos_const_perm,
                                                      button);
            if (sc_iterator3_next(it) == SC_TRUE) {
                ss << "element : 'button', properties: { style:{";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "color",
                           keynode_nrel_color);

                ss << ", ";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "border",
                           keynode_nrel_border);
                ss << ", ";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "\"border-radius\"",
                           keynode_nrel_border_radius);
                ss << ", ";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "\"font-size\"",
                           keynode_nrel_font_size);
                ss << ", ";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "\"background-color\"",
                           keynode_nrel_background_color);
                ss << ", ";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "\"text-align\"",
                           keynode_nrel_text_align);
                ss << "}}";
                ss << ", ";
                getContent(ss, sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, button, "content",
                           keynode_nrel_content);
                sc_iterator3_free(it3);
                ss << "}]";
                mOutputData = ss.str();
                SC_LOG_INFO(mOutputData);
            }
        }
    }
}

void
uiSc2uiJsonTranslator::getContent(StringStream &ss, String &sys_idtf, sc_stream *idtf_stream, sc_uint32 idtf_length,
                                  sc_uint32 read_bytes, sc_char *buffer, const sc_addr &button, const String keyword,
                                  sc_addr keynode_nrel_content) {
    sc_iterator5 *it5 = sc_iterator5_f_a_a_a_f_new(s_default_ctx,
                                                   button,
                                                   sc_type_arc_common | sc_type_const,
                                                   sc_type_link,
                                                   sc_type_arc_pos_const_perm,
                                                   keynode_nrel_content);

    if (sc_iterator5_next(it5) == SC_TRUE) {
        sc_addr link = sc_iterator5_value(it5, 2);
        get_idtf(sys_idtf, idtf_stream, idtf_length, read_bytes, buffer, link);

        ss << keyword << ":";
        ss << "'" << sys_idtf << "'";

        mOutputData = ss.str();
        sys_idtf = "";
    }
    // }
};

void
uiSc2uiJsonTranslator::get_idtf(String &sys_idtf, sc_stream *&idtf_stream, sc_uint32 &idtf_length,
                                sc_uint32 &read_bytes,
                                sc_char *buffer, const sc_addr &link) const {
    sc_bool result;
    if (sc_memory_get_link_content(s_default_ctx, link, &idtf_stream) == SC_RESULT_OK) {
        sc_stream_get_length(idtf_stream, &idtf_length);
        while (sc_stream_eof(idtf_stream) == SC_FALSE) {
            sc_stream_read_data(idtf_stream, buffer, 32, &read_bytes);
            sys_idtf.append(buffer, read_bytes);
        }
        sc_stream_free(idtf_stream);

        result = SC_TRUE;
    }
}

// ------------------------------------------------------------------------------
sc_result uiSc2uiJsonTranslator::ui_translate_sc2ui_json(const sc_event *event, sc_addr arg) {
    sc_addr cmd_addr, input_addr, format_addr;

    if (sc_memory_get_arc_end(s_default_ctx, arg, &cmd_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_check_cmd_type(cmd_addr, keynode_command_translate_from_sc) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (ui_translate_command_resolve_arguments(cmd_addr, &format_addr, &input_addr) != SC_RESULT_OK)
        return SC_RESULT_ERROR;

    if (format_addr == keynode_format_ui_json) {
        uiSc2uiJsonTranslator translator;
        translator.translate(input_addr, format_addr);
    }
    return SC_RESULT_OK;
}