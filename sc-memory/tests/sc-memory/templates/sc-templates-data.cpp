#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_templates.hpp"
#include "template_test_utils.hpp"

using ScTemplateDataTest = ScTemplateTest;

TEST_F(ScTemplateDataTest, smoke)
{
  ScAddr const addr1(10);
  ScAddr const addr2(20);

  ScTemplateData data;

  data.AddTriple(addr1, ScType::EdgeAccessVarFuzPerm >> "_edge", addr2);

  data.AddTriple(ScType::NodeVarAbstract, ScType::EdgeAccessVarPosPerm, "_edge");

  EXPECT_EQ(data.TriplesNum(), 2u);
}

TEST_F(ScTemplateDataTest, Replacements)
{
  ScTemplateData data;
  ScAddr const sourceAddr(21);
  ScAddr const targetAddr(43);

  data.AddTriple(sourceAddr, ScType::EdgeAccessVarFuzPerm >> "_edge", targetAddr >> "addr2");

  EXPECT_TRUE(data.HasReplacement("_edge"));
  EXPECT_TRUE(data.HasReplacement("addr2"));
  EXPECT_TRUE(data.HasReplacement(sourceAddr.ToString()));
  EXPECT_TRUE(data.HasReplacement(targetAddr.ToString()));

  EXPECT_FALSE(data.HasReplacement("non_exist"));

  EXPECT_EQ(data.Replacements().size(), 4u);

  auto const repl1 = data.GetReplacement("_edge");
  EXPECT_TRUE(repl1);
  EXPECT_EQ(repl1->m_replacementName, "_edge");

  auto const repl2 = data.GetReplacement("non-exist");
  EXPECT_FALSE(repl2);
}

TEST_F(ScTemplateDataTest, build_error_double_replacements)
{
  ScTemplateData data;
  data.AddTriple(ScAddr(1), ScType::EdgeAccessVarPosPerm, ScType::NodeVarClass >> "_test");

  EXPECT_ANY_THROW(data.AddTriple(ScType::NodeVarAbstract >> "_test", ScType::EdgeAccessVarFuzPerm, ScAddr(2)));
}

TEST_F(ScTemplateDataTest, SetupReplAlias)
{
  ScTemplateData data;

  EXPECT_FALSE(data.SetupReplAlias("non-exist", "new-alias"));

  std::string const repl = "_repl";
  std::string const alias = "_alias";

  data.AddTriple(ScAddr(1), ScType::EdgeAccessVarFuzTemp >> repl, ScType::NodeVarAbstract);

  EXPECT_FALSE(data.HasReplacement(alias));
  EXPECT_TRUE(data.HasReplacement(repl));

  data.SetupReplAlias(repl, alias);
  EXPECT_TRUE(data.HasReplacement(alias));

  EXPECT_EQ(data.GetReplacement(alias), data.GetReplacement(repl));
}
