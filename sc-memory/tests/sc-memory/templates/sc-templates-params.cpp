#include <gtest/gtest.h>

#include "sc-memory/sc_templates.hpp"

#include "sc_test.hpp"

using ScTemplateParamsTest = ScMemoryTest;

TEST_F(ScTemplateParamsTest, Smoke)
{
  ScTemplateParams params;

  EXPECT_TRUE(params.IsEmpty());
  EXPECT_FALSE(params.TryGet("_non_exist").IsValid());
  EXPECT_FALSE(params["_non_exist"].IsValid());

  ScAddr const addr(2123);
  params.Add("_exist", addr);

  EXPECT_TRUE(params["_exist"].IsValid());
  EXPECT_FALSE(params[addr].IsValid());

  EXPECT_EQ(params["_exist"], addr);
}

TEST_F(ScTemplateParamsTest, Iterator)
{
  ScTemplateParams params;

  ScAddr const test1(34234);
  ScAddr const test2(3422234);

  params
      .Add("test1", test1)
      .Add("test2", test2);

  size_t counter = 0;
  EXPECT_FALSE(params.IsEmpty());
  for (auto const & it : params)
  {
    counter++;
    EXPECT_TRUE( it.second == test1 || it.second == test2 );
  }

  EXPECT_EQ(counter, 2u);
}

TEST_F(ScTemplateParamsTest, CanAssignParam)
{
  // positive
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::NodeVar, ScType::Node));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::NodeVar, ScType::NodeConst));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::NodeVar, ScType::NodeConstClass));

  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::NodeVarAbstract, ScType::NodeConstAbstract));

  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::LinkVar, ScType::Link));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::LinkVar, ScType::LinkConst));

  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarFuzPerm, ScType::EdgeAccessConstFuzPerm));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarFuzTemp, ScType::EdgeAccessConstFuzTemp));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarNegPerm, ScType::EdgeAccessConstNegPerm));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarNegTemp, ScType::EdgeAccessConstNegTemp));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarPosPerm, ScType::EdgeAccessConstPosPerm));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarPosTemp, ScType::EdgeAccessConstPosTemp));

  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeUCommonVar, ScType::EdgeUCommon));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeUCommonVar, ScType::EdgeUCommonConst));

  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeDCommonVar, ScType::EdgeDCommon));
  EXPECT_TRUE(ScTemplateParams::CanAssignParam(ScType::EdgeDCommonVar, ScType::EdgeDCommonConst));

  // negative
  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::NodeConst, ScType::NodeConstAbstract));
  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::NodeVar, ScType::EdgeAccessConstPosTemp));

  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::NodeVarAbstract, ScType::NodeConst));
  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::NodeVarAbstract, ScType::NodeConstClass));

  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarFuzPerm, ScType::EdgeAccessConstFuzTemp));
  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::EdgeAccessVarFuzPerm, ScType::EdgeAccessConstPosTemp));

  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::EdgeDCommonVar, ScType::EdgeUCommonConst));
  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::EdgeDCommonVar, ScType::EdgeAccessConstPosPerm));

  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::EdgeUCommonVar, ScType::EdgeDCommonConst));
  EXPECT_FALSE(ScTemplateParams::CanAssignParam(ScType::EdgeUCommonVar, ScType::EdgeAccessConstPosPerm));
}

