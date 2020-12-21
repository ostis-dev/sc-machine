#include <gtest/gtest.h>

#include "builder_test.hpp"

#include "sc-memory/sc_templates.hpp"

TEST_F(ScBuilderTest, Smoke)
{
  ScMemoryContext ctx("Builder_Base");

  std::string const scsData =
    "base"
    " _<- test_element;"
    " _=> nrel_main_idtf::"
    "    _[]"
    "    (* _<- lang_en;; *);;";

  ScTemplatePtr templ = ScTemplateSCsBuilder(ctx).Make(scsData);
  EXPECT_TRUE(templ);

  ScTemplateSearch search(ctx, *templ);
  EXPECT_TRUE(search.begin() != search.end());
}
